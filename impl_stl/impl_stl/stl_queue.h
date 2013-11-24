#ifndef _STL_QUEUE_
#define _STL_QUEUE_
//#include <functional>
#include "stl_heap.h"

template <class T> struct less {
  bool operator() (const T& x, const T& y) const {return x<y;}
};

template <class T, class Sequence = deque<T> >
class queue
{
public:
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::value_type value_type;


	void push(const value_type &val);
	void pop();
	reference front();
	reference back();
	size_type size();
	bool empty();

protected:
	Sequence c;
};


template <class T, class Sequence>
void queue<T, Sequence>::push(const value_type &val)
{
	c.push_back(val);
}


template <class T, class Sequence>
void queue<T, Sequence>::pop()
{
	c.pop_front();
}

template <class T, class Sequence>
typename queue<T, Sequence>::reference queue<T, Sequence>::front()
{
	return c.front();
}


template <class T, class Sequence>
typename queue<T, Sequence>::reference queue<T, Sequence>::back()
{
	return c.back();
}

template <class T, class Sequence>
typename queue<T, Sequence>::size_type size()
{
	return c.size();
}

template <class T, class Sequence>
bool queue<T, Sequence>::empty()
{
	return c.empty();
}




template <class T, class Sequence = vector<T>, class Compare = less<T> >
class priority_queue
{
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;


public:
	priority_queue() : c() {}
	explicit priority_queue(const Compare &x) : c(), cmp(x) {}

	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last, const Compare &x)
		: c(first, last), cmp(x) { make_heap(c.begin(), c.end(), cmp); }

	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last)
		: c(first, last) { make_heap(c.begin(), c.end());}

public:
	bool empty() { return c.empty(); }
	size_type size() { return c.size(); }
	reference top() { return c.front(); }

	void push(const value_type &val);
	void pop();

protected:
	Compare cmp;
	Sequence c;
};



template <class T, class Sequence, class Compare>
void priority_queue<T, Sequence, Compare>::push(const value_type &val)
{
	c.push_back(val);
	push_heap(c.begin(), c.end(), cmp);
}

template <class T, class Sequence, class Compare>
void priority_queue<T, Sequence, Compare>::pop()
{
	pop_heap(c.begin(), c.end(), cmp);
	c.pop_back();
}


#endif