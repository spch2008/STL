#include <cstddef>
#include <iostream>


#define __THROW_BAD_ALLOC {std::cout << "BAD ALLOC" << std::endl; exit(1);}


template<int inst>
class _malloc_alloc_template
{
public:
	static void *allocate(size_t size);
	static void *reallocate(void *p, size_t old_size, size_t new_size);
	static void  deallocate(void *p, size_t size);
	static void *set_malloc_handler( void (*malloc_handler)() );  //必须返回之前的handler

private:
	static void *oom_alloc(size_t size);
	static void *oom_realloc(void *p, size_t old_size, size_t new_size);
	
private:
	static void (*malloc_oom_handler)(); //不是用来获取内存，而是释放一些

};

template <int inst>
void (*_malloc_alloc_template<inst>::malloc_oom_handler)() = 0;



template <int inst>
void *_malloc_alloc_template<inst>::set_malloc_handler( void (*malloc_handler)() )
{
	void (*my_handler)() = malloc_oom_handler;
	malloc_oom_handler = malloc_handler;

	return my_handler;
}

template <int inst>
void *_malloc_alloc_template<inst>::allocate(size_t size)
{
	void *result = malloc(size);
	if(result == 0) result = oom_alloc(size);

	return result;
}


template <int inst>
void *_malloc_alloc_template<inst>::reallocate(void *p, size_t old_size, size_t new_size)
{
	void *result = realloc(p, new_size);
	if(result == 0) result = oom_realloc(p, old_size, new_size);

	return result;
}


template <int inst>
void _malloc_alloc_template<inst>::deallocate(void *p, size_t size)
{
	free(p);
}

template <int inst>
void *_malloc_alloc_template<inst>::oom_alloc(size_t size)
{
	void *result = 0;

	for( ; ;)
	{
		if( malloc_oom_handler == 0)
		{
			__THROW_BAD_ALLOC
		}
		(*malloc_oom_handler)();    //做一些清理工作
		result = malloc(size);
		if(result != 0) return result;
	}
}


template <int inst>
void *_malloc_alloc_template<inst>::oom_realloc(void *p, size_t old_size, size_t new_size)
{
	void *result = 0;
	
	for(; ;)
	{
		if( malloc_oom_handler == 0)
		{
			__THROW_BAD_ALLOC
		}
		(*malloc_oom_handler)();
		result = realloc(p, new_size);
		if( result != 0) return result;
	}
}


//typedef _malloc_alloc_template<0> malloc_alloc;



template <bool threads, int inst>
class _default_alloc_template
{
private:
	enum { MAX_BYTES = 128};
	enum { ALIGN     = 8};
	enum { NFREELISTS = MAX_BYTES / ALIGN};

public:
	static void *allocate(size_t size);
	static void *reallocate(void *p, size_t old_size, size_t new_size);
	static void  deallocate(void *p, size_t size);


private:
	union node
	{
		node *next;
		char data[1];
	};

	static node *free_list[NFREELISTS];
	static char *free_start;
	static char *free_end;
	static size_t heap_size;

private:
	size_t ROUND_UP(size_t size)  { return (size + ALIGN - 1) & (ALIGN - 1);}
	int    LIST_INDEX(size_t size){ return size / ALIGN;}            // what type?

	static void *refill(size_t size);
	static char *chunk(size_t size, int &nodes);
};

template <bool threads, int inst>
char *_default_alloc_template<threads, inst>::free_start = 0;

template <bool threads, int inst>
char *_default_alloc_template<threads, inst>::free_end = 0;

template <bool threads, int inst>
char *_default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename _default_alloc_template<threads, inst>::node*
	_default_alloc_template<threads, inst>::free_list[NFREELISTS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                                                    

template <bool threads, int inst>
void *_default_alloc_template<threads, inst>::allocate(size_t size)
{
	if(size > MAX_BYTES)
		return malloc_alloc::allocate(size);

	node *my_free_list = free_list + LIST_INDEX(size);
	void *result = *my_free_list;
	if(result == 0)
	{
		result = refill(size);
		return result;
	}

	*my_free_list = (node*)result -> next;
	return result;
}


template <bool threads, int inst>
void *_default_alloc_template<threads, inst>::reallocate(void *p, size_t old_size, size_t new_size)
{
	return 0;
}


template <bool threads, int inst>
void _default_alloc_template<threads, inst>::deallocate(void *p, size_t size)
{
	if(size > MAX_BYTES)
		malloc_alloc::deallocate(p, size);

	node *my_free_list    = free_list + LIST_INDEX(size);
	(node*)p->next        = (*my_free_list);
	*my_free_list          = p;
}

template <bool threads, int inst>
void *_default_alloc_template<threads, inst>::refill(size_t size)
{
	int nodes = 20;
	char *result = chunk(size, nodes);

	if(nodes == 1)
		return result;

	
	node *my_free_list = free_list + LIST_INDEX(size);
	node *free_node    = *my_free_list;
	node *curr         = (node*)(result + size);
	for(int i = 0; i < nodes-1; i++)
	{
		curr->next = free_node;
		free_node  = curr;
		curr       = (node*)((char*)curr + size);
	}

	return result;
}


template <bool threads, int inst>
char *_default_alloc_template<threads, inst>::chunk(size_t size, int &nodes)
{
	size_t bytes_total = size * nodes;
	size_t bytes_left   = free_end - free_start;   // right ?    + 1

	if( bytes_left > bytes_total)
	{
		void *result = start;
		free_start += bytes_total;
		return result;
	}
	else if(bytes_left >= size)
	{
		nodes = bytes_left / size;
		void *result = free_start;
		free_start += size * nodes;
		return result;
	}
	else
	{
		if(bytes_left > 0)
		{
			node *my_free_list = free_list + FREE_INDEX(bytes_left);
			node *curr         = (node*)start;
			curr->next         = *my_free_list;
			*my_free_list      = curr;
		}


		size_t bytes_to_get = 2 * bytes_total + ROUND_UP(heap_size >> 4);
		void *result = malloc(bytes_to_get);
		if(result != 0)
		{
			free_start = (char*)result + bytes_total;
			end_free   = free_start + bytes_to_get - bytes_total;
			return result;
		}

		for(size_t i = size; i <= MAX_BYTES; i += ALIGN)
		{
			void *my_free_list = free_list + FREE_INDEX(i);
			if(*my_free_list != 0)
			{
				node *result = *my_free_list;
				*my_free_list= result->next;
				free_start = (char*)result + size;
				free_end   = free_start + i - size;
				return result;
			}
		}
	}
}