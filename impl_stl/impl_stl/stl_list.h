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
	_list_iterator(link_type x) : node(x) {}
	_list_iterator(const iterator& x) : node(x.node) {}

	bool operator==(const iterator& x) { return node == x.node; }
	bool operator!=(const iterator& x) { return node != x.node; }

	reference operator*() { return node->data;   }
	pointer operator->()  { return &operator*(); }

	iterator& operator++()
	{
		node = (link_type)node->next;
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
	typedef simple_alloc<list_node, Alloc> data_allocator;

protected:
	link_type node;


public:

	//Member functions
	list()  { empty_initialize(); };
	~list() { clear(); put_node(node); };

	list(const list& that) {};
	list& operator=(const list& that);


	//Iterators:
	iterator begin(){ return link_type(node->next); }
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
	void insert(iterator position, size_type n, const T& val);
	iterator erase (iterator position);
	iterator erase (iterator first, iterator last);
	void clear(){ erase(begin(), end() ); }
	void swap (list& x);
	void resize(size_type new_size, const T& val);
    void resize(size_type new_size) { resize(new_size, T()); }

	//Operations:
	void remove (const value_type& val);
	void unique();
	void merge (list& x);
	void reverse();
	void sort();

	
	void splice (iterator position, list& x);
	void splice (iterator position, list& x, iterator i);
	void splice (iterator position, list& x, iterator first, iterator last);

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

	void transfer(iterator position, iterator first, iterator last);

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
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const value_type& val)
{
	link_type tmp = create_node(val);
	tmp->next     = position.node;
	tmp->prev     = position.node->prev;
	link_type(position.node->prev)->next = tmp;
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
void list<T, Alloc>::insert(iterator position, size_type n, const T& val)
{
	for(; n > 0; n--)
		insert(position, val);
}


template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position)
{
	link_type next_node = (link_type)position.node->next;
	link_type prev_node = (link_type)position.node->prev;
	link_type(position.node->prev)->next = next_node;
	link_type(position.node->next)->prev = prev_node;
	
	destroy_node(position.node);
	return next_node;
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last)
{
	while(first != last)
		erase(first++);
	return last;
}


template <class T, class Alloc>
void list<T, Alloc>::remove(const value_type & val)
{
	iterator first = begin();
	iterator last  = end();

	while( first != last)
	{
		if( *first == val)
			first = erase(first);
		++first;
	}
}


template <class T, class Alloc>
void list<T, Alloc>::unique()
{
	iterator first = begin();
	iterator last  = end();
	if(first == last) return;

	iterator next = first;
	while(++next != last)
	{
		if( *first == *next)
			erase(next);
		else
			first = next;
		next = first;
	}
}

template <class T, class Alloc>
void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
{
	if( position == last)
		return;

	link_type(position.node->prev)->next = first.node;
	link_type(first.node->prev)->next    = last.node;
	link_type(last.node->prev)->next     = position.node;

	link_type  tmp      = (link_type)position.node->prev;
	position.node->prev = last.node->prev;
	last.node->prev     = first.node->prev;
	first.node->prev    = tmp;
	

	//1. 先让前一个节点指向position，last，first，以防破坏pre指针
	//2. 记录的tmp肯定要最后处理
}


template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc> &that)
{
	iterator first1 = begin();
	iterator last1  = end();
	iterator first2 = that.begin();
	iterator last2  = that.end();

	while(first1 != last1 && first2 != last2)
	{
		if( *first2 < *first1)
		{
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	}
	if(first2 != last2)
			transfer(last1, first2, last2);
}

template <class T, class Alloc>
void list<T, Alloc>::reverse()
{
	if(size() == 0 || size() == 1)
		return;

	iterator first = begin();
	iterator last  = end();

	++first;
	while(first != last)
	{
		iterator old = first;
		++first;
		transfer(begin(), first, next);
	}
}

//拼接两个链表，两个链表必须不同
template <class T, class Alloc>
void list<T, Alloc>::splice(iterator position, list &that)
{
	if( that.size() != 0)
		transfer(position, that.begin(), that.end());
}


//将迭代器i指向的元素拼接在position处
template <class T, class Alloc>
void list<T, Alloc>::splice(iterator position, list& x, iterator i)
{
	iterator j = i;
	++j;
	if( position == i || position == j)  //已经符合要求
		return;
	transfer(position, i, j);
}


template <class T, class Alloc>
void list<T, Alloc>::splice(iterator position, list& x, iterator first, iterator last)
{
	if(first != last)
		transfer(position, first, last);
}


template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T &val)
{
	size_type len = 0;
	iterator  i   = begin();

	for( ; i != end() && len < new_size; len++, i++)
		;

	if(len == new_size)
		erase(i, end());
	else
		insert(i, new_size - i, val);
}

template <class T, class Alloc>
void list<T, Alloc>::swap (list<T, Alloc>& that)
{
	link_type tmp = node;
	node          = that.node;
	that.node     = tmp;

	//node为指向每个链表的头指针，交换指针即可更换链表
}


template <class T, class Alloc>
void list<T, Alloc>::sort()
{
	if( size() == 0 || size() == 1)
		return;

	list<T, Alloc> carry;
	list<T, Alloc> counter[64];  // 排序元素总数: 1 + 2 + 4 + 8 + ... + 2^64
	int fill = 0;                // fill记录counter中使用的最远空间索引

	while( !empty() )
	{
		carry.splice( carry.begin(), *this, begin() ); //一次取一个元素
		int i = 0;
		while( i < fill && !counter[i].empty() )       //将carry中元素合并到counter中
		{
			counter[i].merge(carry);                   //将carry中元素合并到counter[i]
			carry.swap(counter[i++]);                  //将合并后链表给carry
		}

		carry.swap(counter[i]);                        //合并后链表给counter
		if( i == fill) fill++;                         //合并到counter中，如果添加到counter的末尾，则fill+1
	}                                                  //还有一种情况是，比如i = 3， fill = 8, 
	                                                   //说明fill前有空洞，即被合并后空出的位置

	for(int i = 1; i < fill; i++)
		counter[i].merge(counter[i-1]);                //收集counter中元素
	swap(counter[fill-1]);
}