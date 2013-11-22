

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
	void push(value_type &val);
	void pop();


protected:
	Sequence c;
};