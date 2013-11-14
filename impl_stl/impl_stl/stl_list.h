#include "memory.h"
#include "stl_iterator.h"

template <class T>
struct _list_node
{
	typedef void* void_pointer;
	void_pointer next;
	void_pointer prev;
	T data;
};


template <class T, class PTR, class REF>
struct _list_iterator
{
	typedef _list_iterator<T, PTR, REF> iterator;

	typedef bidirectional_iterator_tag iterator_category;
	typedef T  value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef _list_node<T>* link_type;
	link_type node;

	_list_iterator() { node = 0; }
	_list_iterator(x) : node(x) {}
	_list_iterator(const iterator& x) : node(x.node) {}

	bool operator==(const iterator& x) { return node == x.node; }
	bool operator!=(const iterator& x) { return node != x.node; }

	reference operator*() { return node->data;   }
	pointer operator->()  { return &operator*(); }

	iterator& operator++()
	{
		node = node->next;
		return *this;
	}

	iterator operator++(int)
	{
		iterator tmp = *this;
		++*this;
		return tmp;
	}

	iterator& operator--()
	{
		node = node->prev;
		return *this;
	}

	iterator operator--(int)
	{
		iterator tmp = *this;
		--*this;
		return tmp;
	}
};


template <class T, class Alloc = alloc>
class list
{
public:
	typedef T  value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef _list_iterator<T, pointer, reference> iterator;
	typedef const _list_iterator<T, pointer, reference> const_iterator;

protected:
	typedef _list_node<T> list_node;
	typedef list_node*    link_type;
	typedef simple_alloc<T, Alloc> data_allocator;

protected:
	link_type node;


public:

	//Member functions
	list()  { empty_initialize(); };
	~list() { clear(); put_node(node); };

	list(const list& that) {};
	list& operator=(const list& that);


	//Iterators:
	iterator begin(){ return node->next; }
	iterator end()  { return node; }


	//Capacity:
	bool empty() { return node->next == node; }
	size_type size(){ return (size_type)distance(begin(), end()); }

	//Element access:
	reference front() { return *begin(); }
	reference back()  { return *( --end() ); }

	//Modifiers:
	void push_front(const value_type& val){ insert(begin(), val); }
	void pop_front() { erase(begin()); }
	void push_back(const value_type& val) { insert(end(), val); }
	void pop_back(){ erase( --end()); }
	iterator insert(iterator position, const value_type& val);
	void insert(iterator position, iterator first, iterator last);
	iterator erase (iterator position);
	iterator erase (iterator first, iterator last);
	void clear(){ erase(begin(), end() ); }


protected:
	link_type get_node() { return data_allocator::allocate(); }
	void      put_node(link_type p) { data_allocator::deallocate(p); }

	link_type create_node(const T& x)
	{
		link_type p = get_node();
		construct(&p->data, x);
		return p;
	}

	void destroy_node(link_type p)
	{
		destroy(&p->data);
		put_node(p);
	}

	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}

};

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list& that)
{
	iterator first1 = begin();
	iterator last1  = end();
	const_iterator first2 = that.begin();
	const_iterator last2  = that.end();

	while( first1 != last && first2 != last2)
		*first1++ = *first2++;
	if( first2 == last2 )
		erase(first1, last1);
	else
		insert(last1, first2, last2);
	
	return *this;
}


template <class T, class Alloc>
iterator list<T, Alloc>::insert(iterator position, const value_type& val)
{
	link_type tmp = create_node(val);
	tmp->next     = position.node;
	tmp->prev     = position.node->prev;
	position.node->prev->next = tmp;
	position.node->prev       = tmp;
	return tmp;
}

template <class T, class Alloc>
void list<T, Alloc>::insert(iterator position, iterator first, iterator last)
{
	for(; first != last; first++)
		insert(position, *first);
}


template <class T, class Alloc>
iterator list<T, Alloc>::erase(iterator position)
{
	link_type next_node = position.node->next;
	link_type prev_node = position.node->prev;
	position.node->prev->next = next_node;
	position.node->next->prev = prev_node;
	
	destroy_node(position.node);
	return next_node;
}

template <class T, class Alloc>
iterator list<T, Alloc>::erase(iterator first, iterator last)
{
	while(first != last)
		erase(++first);
	return last;
}