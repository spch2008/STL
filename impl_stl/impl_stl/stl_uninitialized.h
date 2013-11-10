// ���򿽱�����ڴ�����

//using copy fill fill_n
#include <algorithm>


//�������һ���������ĵ�ַ
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
	                                        Size size, const T& x)
{
	return _uninitialized_fill_n(first, size, x, value_type(first));
}

// T1 �� T ���ܲ�ͬ�� ��һ���Զ����࣬һ��int
template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator _uninitialized_fill_n(ForwardIterator first,
	                                         Size size, const T& x, T1*)
{
	typename typedef type_traits<T1>::is_POD_type is_POD;
	return _uninitialized_fill_n_aux(first, size, x, is_POD());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator _uninitialized_fill_n(ForwardIterator first,
	                                         Size size, const T& x, true_type)
{
	return fill_n(first, n, x);
}

/*ȡ��first��ָ����*first, Ȼ��ȡ��ַ���õ��ڴ��ַ�����¶��󹹽��ڴ�*/
template <class ForwardIterator, class Size, class T>
inline ForwardIterator _uninitialized_fill_n(ForwardIterator first,
	                                         Size size, const T& x, false_type)
{
	ForwardIterator curr = first;
	for(; size > 0; size--, curr++)
		construct(&*first, x);           
	return curr;
}
/* ���̣�
   1. ȡ����������ָ��������
   2. ȡ�������type
   3. ���ݶ���type���д���
*/

template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first,
	                                      InputIterator last, ForwardIterator result)
{
	return _uninitialized_copy(first, last, result, value_type(first));
}


template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last,
	                                       ForwardIterator result, T*)
{
	typedef typename type_traits<InputIterator>::is_POD_type is_POD;
	return _uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                           ForwardIterator result, true_type)
{
	return copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
	                                           ForwardIterator result, false_type)
{
	for(; first < last; ++first, ++result)
		construct(&*result, *first);
	return result;
}

template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& val)
{
	_uninitialized_fill(first, last, val, value_type(first));
}


template <class ForwardIterator, class T, class T1>
inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& val, T1*)
{
	typedef typename type_traits<T1>::is_POD_type is_POD;
	_uninitialized_fill_aux(first, last, val, is_POD() );
}


template <class ForwardIterator, class T>
inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &val, true_type)
{
	fill(first, last, val);
}

template <class ForwardIterator, class T>
inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &val, false_type)
{
	for(; first < last; first++)
		construct(&*first, val);
}