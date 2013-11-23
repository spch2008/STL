#ifndef _STL_HEAP
#define _STL_HEAP
#include "stl_iterator.h"

template <class RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	_push_heap(first, last, distance_type(first), value_type(first));
}


template <class RandomAccessIterator, class Distance, class T>
void _push_heap(RandomAccessIterator first, RandomAccessIterator last,
	            Distance*, T*)
{
	_push_heap_aux(first, (Distance(last-first)-1), Distance(0), T(*(last-1)) );
}



template <class RandomAccessIterator, class Distance, class T>
void _push_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T& value)
{
	Distance parentIndex = (holeIndex - 1) / 2;
	while( holeIndex > topIndex && *(first + parentIndex) < value )
	{
		*(first + holeIndex) = *(first + parentIndex);
		holeIndex = parentIndex;
		parentIndex = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}


template <class RandomAccessIterator>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	_pop_heap(first, last, distance_type(first), value_type(first));
}


template <class RandomAccessIterator, class Distance, class T>
void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
	           Distance*, T*)
{
	T x_copy = *(last - 1);
	*(last-1)    = * first;
	_pop_heap_aux(first, Distance(0), Distance(last-first)-1, x_copy);
}


template <class RandomAccessIterator, class Distance, class T>
void _pop_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance n, T& value)
{
	Distance topIndex = holeIndex;
	Distance secondSonIndex = 2 * holeIndex + 2;

	while( secondSonIndex < n)
	{
		if( *(first + secondSonIndex - 1) > *(first + secondSonIndex) )
			secondSonIndex--;

		*(first + holeIndex) = *(first + secondSonIndex);

		holeIndex = secondSonIndex;
		secondSonIndex = 2 * holeIndex + 2;
	}

	if(secondSonIndex - 1 == n)
	{
		secondSonIndex--;
		*(first + holeIndex) = *(first + secondSonIndex);
		holeIndex = secondSonIndex - 1;
	}

//	_push_heap_aux(first, holeIndex, topIndex, value);
}


template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	while( first < last)
		pop_heap(first, last--);
}

#endif