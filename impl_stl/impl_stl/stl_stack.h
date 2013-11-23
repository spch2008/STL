#ifndef _STL_STACK_H
#define _STL_STACK_H

#include "stl_deque.h"

template <class T, class Sequence = deque<T> >
class stack
{
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type  size_type;
	typedef typename Sequence::reference reference;


public:
	
	bool empty();
	size_type size();
	reference top();
	void push(const value_type &val);
	void pop();


protected:
	Sequence c;
};

template <class T, class Sequence>
bool stack<T, Sequence>::empty()
{
	return c.empty();
}


template <class T, class Sequence>
typename stack<T, Sequence>::size_type stack<T, Sequence>::size()
{
	return c.size();
}

template <class T, class Sequence>
typename stack<T, Sequence>::reference  stack<T, Sequence>::top()
{
	return c.back();
}


template <class T, class Sequence>
void stack<T, Sequence>::push(const value_type &val)
{
	c.push_back(val);
}

template <class T, class Sequence>
void stack<T, Sequence>::pop()
{
	c.pop_back();
}

#endif