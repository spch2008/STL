
struct input_iterator_tag  {};   //readonly
struct output_iterator_tag {};   //writeonly
struct forward_iterator_tag : public input_iterator_tag {};  // re and wr
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


template <class Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type        value_type;
	typedef typename Iterator::difference_type   difference_type;
	typedef typename Iterator::pointer           pointer;
	typedef typename Iterator::reference         reference;
};


template <class T>
struct iterator_traits<T*>
{
	typedef random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef T*                         pointer;
	typedef T&                         reference;
};


template <class T>
struct iterator_traits<const T*>
{
	typedef random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef const T*                   pointer;
	typedef const T&                   reference;
};


template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category 
	iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();     // why return obj
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type *
	distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}


template <class Iterator>
inline typename iterator_traits<Iterator>::value_type *
	value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type
	distance(Iterator first, Iterator last)
{
	//typedef typename iterator_traits<Iterator>::iterator_category category;
	//return _distance(first, last, category());
	return _distance(first, last, iterator_category(first));
}


template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
	_distance(InputIterator first, InputIterator last, input_iterator_tag)
{
	iterator_traits<InputIterator>::difference_type n = 0;
	while( first < last)
	{
		++n;
		++first;
	}
	return n;
}


template <class RandomIterator>
inline typename iterator_traits<RandomIterator>::difference_type
	_distance(RandomIterator first, RandomIterator last, random_access_iterator_tag)
{
	return last - first;
}



template <class Iterator, class Distance>
inline void advance(Iterator& iter, Distance n)
{
	_advance( iter, n, iterator_category(iter) );
}

template <class InputIterator, class Distance>
inline void _advance(InputIterator &iter, Distance n, input_iterator_tag)
{
	while(n--)
		++iter;
}


template <class BidirectorIterator, class Distance>
inline void _advance(BidirectorIterator &iter, Distance n, bidirectional_iterator_tag)
{
	if(n >= 0)
		while(n--)
			++iter;
	else
		while(n++)
			--iter;
}


template <class RandomIterator, class Distance>
inline void _advance(RandomIterator &iter, Distance n, random_access_iterator_tag)
{
	iter += n;
}
