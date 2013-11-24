#include "memory.h"
#include <algorithm>

template <class T, class Alloc = alloc>
class vector
{
public:
	typedef T  value_type;
	typedef T* pointer;
	typedef T* iterator;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

protected:
	typedef simple_alloc<T, Alloc> data_allocator;

public:
	// Member functions
	vector() : start(0), finish(0), end_of_storage(0){}
	vector(size_type n, const T& value) { fill_initialize(n, value); }
	explicit vector(size_type n) { fill_initialize(n, T() ); }

	template <class InputIterator>
	vector(InputIterator first, InputIterator last) : start(0), finish(0), end_of_storage(0)
	{
		range_initialize(first, last);
	}

	vector(const vector& x);
	vector& operator=(const vector& x);

	~vector(){ destroy(start, finish); deallocate(); }

	//Iterators:
	iterator begin() { return start;}
	iterator end()   { return finish;}

	//Capacity:
	size_type size() { return (size_type)(finish - start); }
	size_type capacity() { return (size_type)(end_of_storage - start); }
	bool empty() { return start == finish; }
	void resize (size_type n);
    void resize (size_type n, const value_type& val);

	//Element access:
	reference operator[](size_type n) { return *(start + n); }
	reference front() { return *start; }
	reference back()  { return *(end - 1); }

	//Modifiers:
	inline void push_back(const value_type& val);
	inline void pop_back();
	void insert(iterator position, size_type n, const value_type& val);
	iterator insert(iterator position, const value_type& val);
	iterator erase(iterator position);
	iterator erase(iterator first, iterator last);
	void clear() { erase(begin(), end()); }

protected:
	inline void deallocate();
	void insert_aux(iterator position, const T& val);
	void fill_initialize(size_type n, const T& val);
	iterator allocate_and_fill(size_type n, const T& val);
	iterator allocate_and_copy(size_type n, iterator first, iterator last);
	
	template <class InputIterator>
	void range_initialize(InputIterator first, InputIterator last)
	{
		for(; first != last; ++first)
			push_back(*first);
	}

private:
	iterator start;
	iterator finish;
	iterator end_of_storage;
};


template <class T, class Alloc>
vector<T, Alloc>::vector(const vector<T, Alloc>& that)
{
	start = allocate_and_copy(that.end() - that.begin(), that.begin(), that.end());
	finish = start + (that.end() - that.begin() );
	end_of_storage = finish;
}


template <class T, class Alloc>
vector<T, Alloc> & vector<T, Alloc>::operator=(const vector<T, Alloc>& that)
{
	if(this != &that)
	{
		if( that.size() > capacity() )
		{
			iterator i = allocate_and_copy(that.size(), that.begin(), that.end());
			
			destroy(start, finish);
			deallocate();

			start = i;
			end_of_storage = start + that.size();
		}
		else if( that.size() > size() )
		{
			copy(that.begin(), that.begin() + size(), start);
			uninitialized_copy(that.begin() + size(), that.end(), finish);
		}
		else
		{
			iterator i = copy(that.begin(), that.end(), start);
			destroy(i, finish);
		}
		finish = start + that.size();
	}
	return *this;
}


template <class T, class Alloc>
void vector<T, Alloc>::pop_back()
{
	--finish;
	destroy(finish);
}

template <class T, class Alloc>
void vector<T, Alloc>::push_back(const value_type& val)
{
	if(finish != end_of_storage)
		construct(finish++, val);
	else
		insert_aux(end(), val);
}


template <class T, class Alloc>
void vector<T, Alloc>::deallocate()
{
	if(start)
	{
		data_allocator::deallocate(start, finish - start);
	}
}


template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
{
	if( position + 1 != end() )
		copy(position + 1, end(), position);
	--finish;
	destroy(finish);
	return position;
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
{
	iterator i = copy(last, finish, first);
	destroy(i, finish);
	finish = finish - (last - first);
	return first;
}


//为什么不直接返回position，因为insert_aux可能导致重新开辟空间，导致position迭代器失效;
template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& val)
{
	size_type n = position - begin();
	if( finish != end_of_storage && position == end() )
	{
		construct(finish, val);
		++finish;
	}
	else
	{
		insert_aux(position, val);
	}
	return begin() + n;
}


template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
{
	if( n != 0)
	{
		value_type x_copy = val;

		if( n < end_of_storage - finish)
		{
			size_type elem_after = finish - position;
			iterator old_finish = finish;
			if(elem_after <= n)  //全部移动到初始化区域, p127分析
			{
				uninitialized_fill_n(finish, n - elem_after, x_copy);
				finish += n - elem_after;
				uninitialized_copy(position, old_finish, finish);
				fill(position, old_finish, x_copy);
			}
			else  //部分移动到未初始化区域
			{
				uninitialized_copy(finish-n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, old_finish, x_copy);
			}
		}
		else
		{
			const size_type old_size = size();
			const size_type new_size = old_size + max(old_size, n);

			iterator new_start  = data_allocator::allocate(new_size);
			iterator new_finish = new_start;

			try
			{
				new_finish = uninitialized_copy(start, position, new_start);
				uninitialized_fill_n(new_finish, n, x_copy);
				new_finish += n;
				new_finish  = uninitialized_copy(position, finish, new_finish);
			}
			catch(...)
			{
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, new_size);
				throw;
			}

			destroy(start, finish);
			deallocate();

			start  = new_start;
			finish = new_finish;
			end_of_storage = start + new_size;
		}
	}
}


template <class T, class Alloc>
void vector<T, Alloc>::resize(size_type new_size)
{
	resize( new_size, T());
}


template <class T, class Alloc>
void vector<T, Alloc>::resize(size_type new_size, const value_type& val)
{
	if( new_size < size() )
		erase(begin() + new_size, end());
	else
		insert(end(), new_size - size(), x);
}


template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& val)
{
	if( finish != end_of_storage)
	{
		construct( finish, *(finish - 1) );
		++finish;
		std::copy_backward(position, finish - 2, finish - 1);
		
		//construct val
		T val_copy = val;   //拷贝一份，因为传的引用，元对象可能会被处理
		*position = val_copy;
	}
	else
	{
		const size_type old_size = size();
		const size_type new_size = (old_size != 0) ? old_size * 2 : 1;

		iterator new_start  = data_allocator::allocate(new_size);
		iterator new_finish = new_start;

		try
		{
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, val);
			++new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch(...)
		{
			destroy(new_start, new_finish);
			data_allocator::deallocate(new_start, new_size);
			throw;
		}

		destroy(start, finish);
		deallocate();

		start  = new_start;
		finish = new_finish;
		end_of_storage = new_start + new_size;
	}
}


template <class T, class Alloc>
void vector<T, Alloc>::fill_initialize(size_type n, const T& val)
{
	start = allocate_and_fill(n, val);
	finish = start + n;
	end_of_storage = finish;
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_fill(size_type n, const T& val)
{
	iterator result = data_allocator::allocate(n);
	uninitialized_fill_n(result, n, val);
	return result;
}


template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_copy(size_type n, iterator first, iterator last)
{
	iteator result = data_allocator::allocate(n);
	uninitialized_copy(result, first, last);
	return result;
}