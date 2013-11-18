
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
	size_t _deque_buf_size(size_t n, size_t sz)
	{ return n != 0 ? n : ( sz < 512 ? size_t(512 / sz) : size_t(1) ); }


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



