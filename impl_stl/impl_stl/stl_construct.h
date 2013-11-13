
#include <new.h>
#include "stl_iterator.h"
#include "type_traits.h"
template <class T1, class T2>
inline void construct(T1 *p, const T2 &value)
{
	new (p) T1(value);   // ? if value is a class type
}

template <class T>
inline void destroy(T *p)
{
	p->~T();
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	_destroy(first, last, value_type(first));
}

template <class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typename typedef  type_traits<T>::has_trival_destructor trival_destructor;
	_destroy_aux(first, last, trival_destructor());
}


template <class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, true_type)
{
	for(; first < last; ++first)
		destroy(&*first);
}

template <class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, false_type)
{
	//do nothing
}

/*
 ����:
 1. ���ݵ�������ȡԪ������
 2. ��Ԫ��������ȡԪ��type
 3. ����Ԫ��type�ֱ���д���
*/