#include <cstddef>
#include <iostream>


#define __THROW_BAD_ALLOC std::cout << "BAD ALLOC" << std::endl; exit(1);


template<class inst>
class _malloc_alloc_template
{
public:
	static void *allocate(size_t size);
	static void *reallocate(void *p, size_t old_size, size_t new_size);
	static void  deallocate(void *p, size_t size);
	static void  set_malloc_handler( void (*malloc_handler)() );

private:
	static void *oom_alloc(size_t size);
	static void *oom_realloc(void *p, size_t old_size, size_t new_size);
	
private:
	static void (*malloc_oom_handler)();

};

template <class inst>
void (*_malloc_alloc_template<inst>::malloc_oom_handler)() = 0;



template <class inst>
void _malloc_alloc_template<inst>::set_malloc_handler( void (*malloc_handler)() )
{
	malloc_oom_handler = malloc_handler;
}

template <class inst>
void *_malloc_alloc_template<inst>::allocate(size_t size)
{
	void *result = malloc(size);
	if(result == 0) result = oom_alloc(size);

	return result;
}


template <class inst>
void *_malloc_alloc_template<inst>::reallocate(void *p, size_t old_size, size_t new_size)
{
	void *result = realloc(p, new_size);
	if(result == 0) result = oom_realloc(p, old_size, new_size);

	return result;
}


template <class inst>
void _malloc_alloc_template<inst>::deallocate(void *p, size_t size)
{
	free(p);
}

template <class inst>
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


template <class inst>
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