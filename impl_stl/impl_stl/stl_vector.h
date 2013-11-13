#include "memory.h"

template <class T>
class vector
{
public:
	typedef T  value_type;
	typedef T* pointer;
	typedef T* iterator;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

public:
	vector() : begin(0), finish(0), end_of_storage(0){}
	vector(size_type n, const T& value) {}
	explicit vector(size_type n) {}

	~vector(){destroy(begin, finish); deallocate();}

	iterator begin() { return begin;}
	iterator end()   { return finish;}

	size_type size() { return (size_type)(finish - begin); }
	size_type capacity() { return (size_type)(end_of_storage - begin); }
	bool empty() { return begin == finish; }

	reference operator[](size_type n) { return *(begin + n); }
	reference front() { return *begin; }
	reference back()  { return *(end - 1); }

	inline void push_back(const value_type& val);
	inline void pop_back();

protected:
	inline void deallocate();

private:
	iterator begin;
	iterator finish;
	iterator end_of_storage;
};


template <class T>
void vector<T>::pop_back()
{
	--finish;
	destroy(finish);
}

template <class T>
void vector<T>::push_back(const value_type& val)
{
	if(finish != end_of_storage)
		construct(finish++, val);
	else
		insert_aux(end(), val);
}

template <class T>
void vector<T>::deallocate()
{
	
}