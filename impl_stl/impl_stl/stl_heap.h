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
void _push_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value)
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
void _pop_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance n, T value)
{
	Distance topIndex = holeIndex;
	Distance secondSonIndex = 2 * holeIndex + 2;
	Distance lastIndex       = n;   //num - 1 = index

	while( secondSonIndex < lastIndex)
	{
		if( *(first + secondSonIndex - 1) > *(first + secondSonIndex) )
			secondSonIndex--;

		*(first + holeIndex) = *(first + secondSonIndex);

		holeIndex = secondSonIndex;
		secondSonIndex = 2 * holeIndex + 2;
	}

	if(secondSonIndex == lastIndex)  //have left son, left = secondSon - 1
	{
		*(first + holeIndex) = *(first + secondSonIndex - 1);
		holeIndex = secondSonIndex - 1;
	}

	_push_heap_aux(first, holeIndex, topIndex, value);
}


template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	while( first < last)
		pop_heap(first, last--);
}

template <class RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	_make_heap(first, last, distance_type(first), value_type(first) );
}


template <class RandomAccessIterator, class Distance, class T>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,
	            Distance*, T*)
{
	Distance n = last - first;
	if( n < 2 ) return;

	Distance parentIndex = (n - 2) / 2;
	while(true)
	{
		_pop_heap_aux(first, parentIndex, n, *(first + parentIndex) );
		if(parentIndex == 0) return;
		parentIndex--;
	}
}



/* add compare func */

template <class RandomAccessIterator, class Compare>
void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
{
	_push_heap(first, last, cmp, distance_type(first), value_type(first));
}


template <class RandomAccessIterator, class Distance, class T, class Compare>
void _push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp,
	            Distance*, T*)
{
	_push_heap_aux(first, cmp, (Distance(last-first)-1), Distance(0), T(*(last-1)));
}



template <class RandomAccessIterator, class Distance, class T, class Compare>
void _push_heap_aux(RandomAccessIterator first, Compare cmp, Distance holeIndex, Distance topIndex, T value)
{
	Distance parentIndex = (holeIndex - 1) / 2;
	while( holeIndex > topIndex && cmp(*(first + parentIndex), value) )
	{
		*(first + holeIndex) = *(first + parentIndex);
		holeIndex = parentIndex;
		parentIndex = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}


template <class RandomAccessIterator, class Compare>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
{
	_pop_heap(first, last, cmp, distance_type(first), value_type(first));
}


template <class RandomAccessIterator, class Distance, class T, class Compare>
void _pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp,
	           Distance*, T*)
{
	T x_copy = *(last - 1);
	*(last-1)    = * first;
	_pop_heap_aux(first, cmp, Distance(0), Distance(last-first)-1, x_copy);
}


template <class RandomAccessIterator, class Distance, class T, class Compare>
void _pop_heap_aux(RandomAccessIterator first, Compare cmp, Distance holeIndex, Distance n, T value)
{
	Distance topIndex = holeIndex;
	Distance secondSonIndex = 2 * holeIndex + 2;
	Distance lastIndex       = n;   //num - 1 = index

	while( secondSonIndex < lastIndex)
	{
		if( !cmp( *(first + secondSonIndex - 1), *(first + secondSonIndex) ))
			secondSonIndex--;

		*(first + holeIndex) = *(first + secondSonIndex);

		holeIndex = secondSonIndex;
		secondSonIndex = 2 * holeIndex + 2;
	}

	if(secondSonIndex == lastIndex)  //have left son, left = secondSon - 1
	{
		*(first + holeIndex) = *(first + secondSonIndex - 1);
		holeIndex = secondSonIndex - 1;
	}

	_push_heap_aux(first, cmp, holeIndex, topIndex, value);
}


template <class RandomAccessIterator, class Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
{
	while( first < last)
		pop_heap(first, last--, cmp);
}

template <class RandomAccessIterator, class Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
{
	_make_heap(first, last, cmp, distance_type(first), value_type(first) );
}


template <class RandomAccessIterator, class Distance, class T, class Compare>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp,
	            Distance*, T*)
{
	Distance n = last - first;
	if( n < 2 ) return;

	Distance parentIndex = (n - 2) / 2;
	while(true)
	{
		_pop_heap_aux(first, cmp, parentIndex, n, *(first + parentIndex) );
		if(parentIndex == 0) return;
		parentIndex--;
	}
}

#endif