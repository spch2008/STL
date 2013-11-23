#ifndef _STL_QUEUE_
#define _STL_QUEUE_
#include "stl_deque.h"

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

#endif