#include "memory.h"
#include "stl_iterator.h"

inline size_t _deque_buf_size(size_t n, size_t sz)
	{ return n != 0 ? n : ( sz < 512 ? size_t(512 / sz) : size_t(1) ); }


template <class T, class PTR, class REF, size_t BufSize>
class _deque_iterator
{
	typedef _deque_iterator<T, T*, T&, BufSize> iterator;
	
	typedef random_access_iterator_tag iterator_category;
	typedef T   value_type;
	typedef PTR pointer;
	typedef REF reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T** map_pointer;

	T* cur;
	T* first;
	T* last;
	map_pointer node;

	static size_t buffer_size() { return _deque_buf_size(BufSize, sizeof(T)); }


	_deque_iterator() : cur(0), first(0), last(0), node(0) {}

	reference operator*() { return *cur; }
	pointer   operator->() { return cur; }

	iterator& operator++()
	{
		cur++;
		if( cur == last)
		{
			set_node(node+1);
			cur = first;
		}
		return *this;
	}

	iterator operator++(int)
	{
		iterator tmp = *this;
		++tmp;
		return tmp;
	}

	iterator operator--()
	{
		if( cur == first)
		{
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}

	iterator operator--(int)
	{
		iterator tmp = *this;
		--tmp;
		return tmp;
	}


	difference_type operator-(const iterator& that)
	{
		return difference_type( buffer_size() ) * (node - that.node - 1)
			+ ( cur - first) + (that.last - that.first);
	}

	iterator& operator+=(difference_type n)
	{
		difference_type offset = (cur - first) + n;
		if( offset >= 0 && offset < difference_type( buffer_size() ) )
		{
			cur += n;
		}
		else
		{
			difference_type node_offset = offset > 0 ? ( offset / difference_type(buffer_size()) ) :
				( -( -offset - 1) / difference_type(buffer_size()) - 1);

			set_node(node + node_offset);
			cur = first + ( offset - node_offset * difference_type( buffer_size() ) );
		}
		return *this;
	}

	iterator operator+(difference_type n)
	{
		iterator tmp = *this;
		tmp += n;
		return tmp;
	}

	iterator& operator-=(difference_type n)
	{
		return *this += -n;
	}

	iterator operator-(difference_type n)
	{
		iterator tmp = *this;
		tmp -= n;
		return tmp;
	}


	void set_node(map_pointer new_node)
	{
		node  = new_node;
		first = *node;
		last  = first + difference_type( buffer_size() );
	}


	bool operator==(const iterator& that)
	{ return cur == that.cur; }
	bool operator!=(const iterator& that)
	{ return cur != that.cur; }
	bool operator<(const iterator& that)
	{
		return (node == that.node) ? curr < that.cur : node < that.node;
	}
};



template <class T, class Alloc = alloc, size_t BufSize = 0>
class deque
{
public:
	typedef _deque_iterator<T, T*, T&, BufSize> iterator;
	typedef T   value_type;
	typedef T*  pointer;
	typedef T&  reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef T** map_pointer;
	
	typedef simple_alloc<pointer, Alloc> map_allocator;
	typedef simple_alloc<value_type, Alloc> data_allocator;


	//Member functions
	deque(size_type n, const value_type& value) : start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}



	//Iterators:
	iterator begin() { return start; }
	iterator end() { return finish; }

	//Capacity:
	size_type size() { return size_type(-1); }
	bool empty() { finish == start};  //important  ??? init start cur 

	//Element access:
	reference operator[](size_type n);
	reference front() { return *start; }
	reference back()  {iterator tmp = finish; --tmp; return *tmp;};

	//Modifiers:
	void push_back(const value_type &val);
	void push_front(const value_type &val);
	void pop_back();
	void pop_front();


protected:
	iterator start;
	iterator finish;

	map_pointer map;
	size_type   map_size;


protected:
	void initialize(size_type n, const value_type& value);
	void create_map_and_nodes(size_type num_elems);

	static size_t buffer_size()
	{
		return _deque_buf_size(BufSize, sizeof(value_type));
	}

	static size_type initial_map_size() { return 8; }

	pointer allocate_node() { return data_allocator::allocate( buffer_size() ); }
	void    deallocate_node(pointer p) { data_allocator::deallocate(p, buffer_size()); }
};



template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::initialize(size_type n, const value_type &value)
{
	return;
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elems)
{
	size_type num_nodes = num_elems / buffer_size() + 1;  // num of map_nodes

	//build map
	map_size = max( initial_map_size(), num_nodes + 2 );
	map      = map_allocator::allocate(map_size);

	//set initial map pointer
	map_pointer nstart  = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = start + num_nodes - 1;             //finish pointer to actual node

	map_pointer curr;
	try
	{
		for(curr = nstart; curr <= nfinish; ++curr)
			*curr = allocate_node();
	}
	catch(...)
	{
		for(map_pointer n = nstart; n < curr; ++n)
			deallocate_node(*n);
		map_allocator::deallocate(map, map_size);
		throw;
	}

	//set start, finish
	start.set_node(nstart);
	start.cur = start.first;

	finish.set_node(nfinish);
	finish.cur = finish.first + num_elems % buffer_size();
	
}
