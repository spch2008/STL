#include "memory.h"
#include "stl_iterator.h"
#include <algorithm>

inline size_t _deque_buf_size(size_t n, size_t sz)
	{ return n != 0 ? n : ( sz < 512 ? size_t(512 / sz) : size_t(1) ); }


template <class T, class PTR, class REF, size_t BufSize>
struct _deque_iterator
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


	reference operator[](difference_type n) { return *(*this + n); }

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

	deque() : start(), finish(), map(0), map_size(0)
	{
		create_map_and_nodes(0);
	}


	//Iterators:
	iterator begin() { return start; }
	iterator end() { return finish; }

	//Capacity:
	size_type size() { return finish - start; }
	bool empty() { finish == start};  //important  ??? init start cur 

	//Element access:
	reference operator[](size_type n) { return start[difference_type(n)]; }
	reference front() { return *start; }
	reference back()  {iterator tmp = finish; --tmp; return *tmp;};

	//Modifiers:
	void push_back(const value_type &val);
	void push_front(const value_type &val);
	void pop_back();
	void pop_front();

	iterator insert (iterator position, const value_type& val);
	iterator erase (iterator position);
	iterator erase (iterator first, iterator last);
	void clear();


protected:
	iterator start;
	iterator finish;

	map_pointer map;  //point to map
	size_type   map_size;


protected:
	void fill_initialize(size_type num_elems, const value_type& value);
	void create_map_and_nodes(size_type num_elems);
	void destroy_map_and_nodes();

	static size_t buffer_size()
	{
		return _deque_buf_size(BufSize, sizeof(value_type));
	}

	static size_type initial_map_size() { return 8; }

	pointer allocate_node() { return data_allocator::allocate( buffer_size() ); }
	void    deallocate_node(pointer p) { data_allocator::deallocate(p, buffer_size()); }

	void push_back_aux(const value_type& val);
	void push_front_aux(const value_type& val);

	void pop_back_aux();
	void pop_front_aux();

	void reserve_map(size_type nodes_to_add, bool add_at_front);
	void reserve_map_at_back(size_type nodes_to_add = 1);
	void reserve_map_at_front(size_type nodes_to_add = 1);

	iterator insert_aux(iterator position, const value_type& val);
};



template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type num_elems, const value_type &value)
{
	create_map_and_nodes(num_elems);

	
	for(map_pointer curr = start.node; curr < finish.node; ++curr)
		uninitialized_fill(*curr, *curr + buffer_size(), value);
	uninitialized_fill(finish.first, finish.cur, value);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elems)
{
	size_type num_nodes = num_elems / buffer_size() + 1;  // num of user-wanted map_nodes

	//build map
	map_size = std::max( initial_map_size(), num_nodes + 2 ); 
	map      = map_allocator::allocate(map_size);

	//set initial map pointer
	map_pointer nstart  = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_nodes - 1;             //finish pointer to actual node

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


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes()
{
	for(map_pointer curr = start.node; curr <= finish.node; ++curr)
		deallocate_node(*curr);
	map_allocator::deallocate(map, map_size);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back(const value_type &val)
{
	if( finish.cur != finish.last - 1)
	{
		construct(finish.cur, val);
		++finish;
	}
	else
		push_back_aux(val);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type &val)
{
	reserve_map_at_back();

	*(finish.node + 1 ) = allocate_node();
	try
	{
		construct(finish.cur, val);
	}
	catch(...)
	{
		deallocate_node(*(finish.node + 1));
		throw;
	}
	
	finish.set_node(finish.node + 1);
	finish.cur = finish.first;

}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front(const value_type &val)
{
	if(start.cur != start.first)
	{
		--start;
		construct(start.cur, val); 
	}
	else
		push_front_aux(val);

}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type &val)
{
	reserve_map_at_front();

	*(start.node - 1) = allocate_node();
	start.set_node(start.node - 1);
	start.cur = start.last - 1;

	try
	{
		construct(start.cur, val);
	}
	catch(...)
	{
		start.set_node(start.node + 1);
		start.cur = start.first;

		deallocate_node(*(start.node-1));
		throw;
	}
}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back()
{
	if(finish.cur != finish.first)
	{
		--finish;
		destroy(finish.cur);
	}
	else
		pop_back_aux();
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux()
{
	deallocate_node(finish.first);

	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;

	destroy(finish.cur);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front()
{
	if(start.cur != start.last - 1)
	{
		destroy(start.cur);
		++start;
	}
	else
		pop_front_aux();
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux()
{
	destroy(start.cur);
	deallocate_node(start.first);

	start.set_node(start.node + 1);
	start.cur = start.first;
}


//疑问：如果默认参数声明与定义不同，何如？
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add)
{
	//used : finish.node - map + 1
	//free ：map_size - (finish.node - map + 1)
	//if free >= nodes_to_add  just return else handle   ==> if ( nodes_to_add > free) ==> do
	//nodes_to_add can be think as （num of min-left）

	if( nodes_to_add + 1 > map_size - (finish.node - map) )
		reserve_map(nodes_to_add, false);
}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add)
{
	if( nodes_to_add > start.node - 1)
		reserve_map(nodes_to_add, true);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map(size_type nodes_to_add, bool add_at_front)
{
	size_type old_num_nodes = finish.node - start.node + 1;
	size_type new_num_nodes = old_num_nodes + nodes_to_add;

	map_pointer new_start;
	if(map_size > 2 * new_num_nodes)  //one edian used heaviley
	{
		new_start = map + (map_size - old_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);

		if(new_start < start.node)
			std::copy(start.node, finish.node + 1, new_start);
		else
			std::copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);
	}
	else
	{
		size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
		map_pointer new_map    = map_allocator::allocate(new_map_size);

		new_start = new_map + ( new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		
		std::copy(start.node, finish.node + 1, new_start);
		map_allocator::deallocate(map, map_size);

		map = new_map;
		map_size = new_map_size;
	}

	start.set_node(new_start);
	finish.set_node(new_start + old_num_nodes - 1);
}

//? all iterator need typename ?
template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, const value_type& value)
{
	if(position.cur == start.cur)
	{
		push_front(value);
		return start;
	}
	else if(position.cur == finish.cur)
	{
		push_back(value);
		iterator tmp = finish;
		return --tmp;
	}
	else
		insert_aux(position, value);
}


template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert_aux(iterator position, const value_type& value)
{
	different_type index = position - start; 
	if( index < size() / 2)
	{
		push_front( front() );
		iterator front1 = start;
		++front1;
		iterator front2 = front1;
		++front2;
		
		copy(front2, position+1, front1);
	}
	else
	{
		push_back( back() );
		iterator back1 = finsih;
		 --back1;
		iterator back2 = back1;
		 --back2;

		copy_backward(pos, back2, back1);
	}
	*position = value;
	return position;
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator position)
{
	different_type index = position - start;

	iterator next = position;
	++next;
	if(index < size() / 2)
	{
		copy_backward(start, position, next);
		pop_front();
	}
	else
	{
		copy(next, finish, pos);
		pop_back();
	}

	return start + index;  
}
// cannot return position, due to position can be invalid

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
{
	if(first == start && last == finish)
	{
		clear();
		return finsih;
	}
	
	difference_type n           = last - first;
	difference_type elem_before = first - start;
	if( elem_before < (size() - n) / 2)
	{
		std::copy_backward(start, first, last);
		iterator new_start = start + n;

		destroy(start, new_start);
		for(map_pointer curr = start.node; curr < new_start.node; ++curr)
			data_allocator::deallocate(*curr, buffer_size());
		
		start = new_start;
	}
	else
	{
		std::copy(last, finish, first);
		iterator new_finish = finish - n;

		destroy(new_finish, finish);
		for(map_pointer curr = new_finish.node+1; curr <= finish.node; ++curr)  //important +1 <=
			data_allocator::deallocate(*curr, buffer_size() );

		finish = new_finish;
	}
	return start + elem_before; //important return val
}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear()
{
	map_pointer curr;
	for(curr = start.node + 1; curr < finish.node; ++curr)
	{
		destroy(*curr, *curr + buffer_size());
		data_allocator::deallocate(*curr, buffer_size());
	}

	if(start.node != finish.node)
	{
		destroy(start.cur, start.last);
		destroy(finish.first, finish.cur);
		data_allocator::deallocate(finish.first, buffer_size());
	}
	else
		destroy(start.cur, start.last);

    //cannot deallocate start.first， because at least reverse one buffer
	
	finish = start;
}



