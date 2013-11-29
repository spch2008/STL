#ifndef _PAIR_H
#define _PAIR_H


template <class T1, class T2>
struct pair
{
	typedef T1 first_type;
	typedef T2 second_type;

	T1 first;
	T2 second;

	pair() : first(T1()), second(T2()) {}
	pair(const T1& a, const T2& b) : first(a), second(b) {}
	pair(const pair<T1, T2>& that) : first(that.first), second(that.second) {}

	bool operator==(const pair<T1, T2>& that) const
	{ return first == that.first && second == that.second; }

	bool operator<(const pair<T1, T2>& that) const
	{ return first < that.first || !( that.first < first) && second == that.second; }

	pair<T1, T2> make_pair(const T1 &a, const T2 &b)
	{ return pair(a, b); }
};

















#endif