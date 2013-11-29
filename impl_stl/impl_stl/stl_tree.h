#ifndef _STL_TREE
#define _STL_TREE

#include "stl_iterator.h"
#include <stddef.h>
#include "memory.h"
#include "stl_pair.h"

#define NULL 0

typedef bool _rb_tree_color_type;
const _rb_tree_color_type _rb_tree_red = true;
const _rb_tree_color_type _rb_tree_black = false;

struct _rb_tree_node_base
{
	typedef _rb_tree_color_type color_type;
	typedef _rb_tree_node_base* base_ptr;

	color_type color;
	base_ptr   parent;
	base_ptr   left;
	base_ptr   right;

	//but when x is NULL， how to do
	static base_ptr minimum(base_ptr x)
	{
		while( x->left != NULL)
			x = x ->left;
		return x;
	}


	static base_ptr maxmun(base_ptr x)
	{
		while( x->right != NULL)
			x = x->right;
		return x;
	}
};


template <class Value>
struct _rb_tree_node : public _rb_tree_node_base
{
	typedef Value value_type;
	typedef _rb_tree_node<Value>* link_type;

	value_type value;
};


struct _rb_tree_base_iterator
{
	typedef _rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag   iterator_category;
	typedef ptrdiff_t                    difference_type;
	
	base_ptr node;


	void increment()
	{
		if(node->right != NULL)
		{
			node = node->right;
			while(node->left != NULL)
				node = node->left;
		}
		else
		{
			base_ptr parent = node->parent;
			while(parent->right == node)
			{
				parent = parent->parent;
				node   = parent;
			}

			if( node->right != parent)
				node = parent;   //parent = header;
		}
	}

	void decrement()
	{ 
		//header
		if(node->color = _rb_tree_red &&
			node == node->parent->parent)
		{
			node = node->right;
		} 
		else if(node->left != NULL) //have left sons
		{
			node = node->left;
			while(node->right != NULL)
				node = node->right;
		}
		else  //have no left son
		{
			base_ptr parent = node->parent;
			while(parent->left == node)
			{
				node = parent;
				parent = parent->parent;
			}
			node = parent;
		}
	}
};


template <class Value, class Ref, class Ptr>
struct _rb_tree_iterator : public _rb_tree_base_iterator
{
	typedef Value value_type;
	typedef Ref   reference;
	typedef Ptr   pointer;
	typedef _rb_tree_iterator<Value, Value&, Value*> iterator;
	typedef _rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
	typedef _rb_tree_node<Value>* link_type;


	_rb_tree_iterator() {}
	_rb_tree_iterator(link_type x) : node(x) {}
	_rb_tree_iterator(const iterator& that) { node = that.node; }

	reference operator*()  { return link_type(node)->value; }
	pointer   operator->() { return &operator*(); }

	iterator operator++() { increment(); return *this;}
	iterator operator++(int) { iterator i = *this; ++*this; return i; }

	iterator operator--() { decrement(); return *this; }
	iterator operator--(int) { iterator i = *this; --*this; return *this;}
};



template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree
{
protected:
	typedef _rb_tree_node_base*  base_ptr;
	typedef _rb_tree_node<Value> rb_tree_node;
	typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
	typedef _rb_tree_color_type  color_type;


public:
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef rb_tree_node* link_type;
	typedef size_t  size_type;
	typedef ptrdiff_t difference_type;

	typedef _rb_tree_iterator<Value, Value&, Value*> iterator;

protected:
	link_type get_node() { return rb_tree_node_allocator::allocate(); }
	void      put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

	link_type create_node(const value_type& val)
	{
		link_type p = get_node();
		construct(&p->value, val);
	}

	void destroy_node(link_type p)
	{
		destroy(&p->value);
		put_node(p);
	}


protected:
	size_type node_count;
	link_type header;
	Compare   key_compare;

	link_type& /*指针的引用*/ root() { return (link_type&)/* 取地址，转换成link_type*/header->parent; }
	link_type& leftmost()  { return (link_type&) header->left;  }
	link_type& rightmost() { return (link_type&) header->right; } 

	static link_type& left(link_type x)  { return (link_type&)x->left; }
	static link_type& right(link_type x) { return (link_type&)x->right; }
	static link_type& parent(link_type x){ return (link_type&)x->parent; }
	static reference  value(link_type x) { return x->value; }
	static color_type& color(link_type x){ return (color_type&)x->color; }
	static const Key& key(link_type x)   { return KeyOfValue()(value(x)); }

public:
	rb_tree(const Compare& cmp = Compare() ) : node_count(0), key_compare(cmp) { init(); }
	~rb_tree() { clear(); put_node(header); }

	iterator insert_equal(const Value& v);
	iterator insert_unique(const Value& v);

public:
	iterator begin() { return leftmost(); }
	iterator end()   { return header;}
	bool  empty() { return node_count == 0; }
	size_type size() { return node_count; }
	size_type max_size() { return size_type(-1); }

	pair<iterator, bool> insert_unique(const value_type& val);
	//iterator insert_equal(const value_type& val);

	void erase(iterator position);
	void erase(iterator fist, iterator last);
	void clear();

private:
	void init();
	void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root);
	void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root);
	void _rb_tree_balance(_rb_tree_node_base* x, _rb_tree_node_base*& root);

	iterator _insert(base_ptr x_, base_ptr y_, const Value& v);
	_rb_tree_node_base* __rb_tree_rebalance_for_erase(_rb_tree_node_base* z,
                                                       _rb_tree_node_base*& root,
                                                       _rb_tree_node_base*& leftmost,
                                                       _rb_tree_node_base*& rightmost);
	void _erase(link_type x);
	//void _clear();
	

};



template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::init()
{
	header = get_node();
	coloer(header) = _rb_tree_red;

	root() = 0;
	leftmost()  = header;
	rightmost() = header; 
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_rb_tree_rotate_left(_rb_tree_node_base* x,
	                                                                       _rb_tree_node_base*& root)
{
	rb_tree_node_base* y = x->right;

	x->right = y->left;
	if(y->left != NULL)
		y->left->parent = x;

	y->parent = x->parent;
	if(x == root)
		root = y;
	else if( x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_rb_tree_rotate_right(_rb_tree_node_base* x,
	                                                                        _rb_tree_node_base*& root)
{
	rb_tree_node_base* y = x->left;

	x->left = y->right;
	if(y->right != NULL)
		y->right->parent = x;

	y->parent = x->parent;
	if(x == root)
		root = y;
	else if( x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->right = x;
	x->parent = y;
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_rb_tree_balance(_rb_tree_node_base* x, 
	                                                                   _rb_tree_node_base*& root)
{
	x->color = _rb_tree_red;
	while( x != root && x->parent->color == _rb_tree_red)
	{
		if( x->parent == x->parent->parent->left)
		{
			_rb_tree_node_base* y = x->parent->right;
			if(y && y->color == _rb_tree_red)
			{
				x->parent->color = _rb_tree_black;
				y->color         = _rb_tree_black;

				x->parent->parent->color = _rb_tree_red;
				x = x->parent->parent;
			}
			else
			{
				if(x == x->parent->right)
				{
					x = x->parent;
					_rb_tree_rotate_left(x, root);

				}

				x->parent->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				_rb_tree_rotate_right(x->parent->parent, root);

			}
		}
		else
		{
			_rb_tree_node_base* y = x->parent->parent->left;
			if(y && y->color == _rb_tree_red )
			{
				x->parent->color = _rb_tree_black;
			    y->color         = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;

				x = x->parent->prent;

			}
			else
			{
				if(x == x->parent->left)
				{
					x = x->parent;
					_rb_tree_rotate_right(x, root);
				}
				
				x->parent->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;

				_rb_tree_rotate_left(x->parent->parent, root);
			}
		}
	}
}



template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_, base_ptr y_, const Value& v)
{
	link_type x = (link_type)x_;  //insert point
	link_type y = (link_type)y_;  //father of insert point

	link_type z;                  //new node

	if(y == header || key_compare(KeyOfValue()(v), key(y)) )
	{
		z = create_node(v);
		left(y) = z;

		if( y == header)
		{
			root() = z;
			leftmost() = z;
		}
		else if( y == leftmost())
			leftmost() = z;

 	}
	else
	{
		z = create_node(v);
		right(y) = z;

		if( y == rightmost())
			rightmost() = z;
	}

	parent(z) = y;
	left(z) = 0;
	right(z) = 0;

	_rb_tree_balance(z, header->parent);
	++node_count;
	return iterator(z);
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
{
	link_type y = header;
	link_type x = root();

	while(x != NULL)
	{
		y = x;
		x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
	}
	_insert(x, y, v);
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
{
	link_type y = header;
	link_type x = root();
	bool cmp = true;

	while(x != NULL)
	{
		y = x;
		cmp = key_compare(KeyOfValue()(v), key(x));
		x = cmp ? left(x) : right(x);
	}
	
	iterator j = iterator(y);
	if(cmp) //v 小于键值
	{
		if( j == begin() )
			return pair<iterator, bool>(_insert(x, y, v), true);
		else
			--j;
	}

	if(key_compare(key(j.node), KeyOfValue()(v)))
		return pair<iterator, bool>(_insert(x, y, v), true);

	return pair<iterator, bool>(j, false);
}



template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_rb_tree_node_base*
 _rb_tree_rebalance_for_erase(_rb_tree_node_base* z, _rb_tree_node_base*& root,
                               _rb_tree_node_base*& leftmost, _rb_tree_node_base*& rightmost)
{
	_rb_tree_node_base *y = z;
	_rb_tree_node_base *x = NULL;
	_rb_tree_node_base *x_parent = NULL;   // rember x_parent because x may be zero, we cann't get it's parent

	//find y and x
	if(y ->left == NULL)
		x = y->right;
	else if(x->right == NULL)
		x = y->left;
	else
	{
		y = y->right;
		while(y->left != NULL)
			y = y->left;
		x = y->right;
	}


	//delete and link nodes
	if(y != z)  //z has two sons
	{
		y->left = z->left;
		z->left->parent = y;

		if(z->right != y) 
		{
			y->parent->left = x;
			if(x) x->parent = y->parent;

			y->right = z->right;
			z->right->parent = y;

		}
		else
			x_parent = y;
		
		if(root == z)
			root = y;
		else if (z->parent->left == z)
			z->parent->left = y;
		else
			z->parent->right = z;
		y->parent = z->parent;

		std::swap(y->color, z->color);
		y = z;
	}
	else
	{
		x_parent = y->parent;
		if(x) x->parent = y->parent;

		if(z == root)
			root = x;
		else if (z->parent->left == z)
			z->parent->left = x;
		else
			z->parent->right = x;

		if (leftmost == z)
		{
			if(z->right == NULL)
				leftmost = z->parent;
			else
				leftmost = _rb_tree_node_base::minimum(x);
		}
		else if(rightmost == z)
		{
			if(z->left == NULL)
				rightmost = z->parent;
			else
				rightmost = _rb_tree_node_base::maxmun(x);
		}
	}


	//balance tree color
	if (y->color != _rb_tree_red)
	{
		while ( x != root && ( x == NULL || x->color == _rb_tree_black) )
		{
			if(x == x_parent->left)
			{
				_rb_tree_node_base* w = x_parent->right;
				if(w->color == _rb_tree_red)
				{
					std::swap(x_parent->color, w->color);
					_rb_tree_rotate_left(x_parent, root);
					w = x_parent->right;
				}
				
				if( (w->left == NULL || w->left->color == _rb_tree_black) &&
					(w->right == NULL || w->right->color == _rb_tree_black) )
				{
					w->color = _rb_tree_black;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else 
				{
					if( w->right == NULL || w->right->color == _rb_tree_black) 
					{
						if(w->left != NULL) w->left = _rb_tree_black;

						w->color = _rb_tree_red;
						_rb_tree_rotate_right(w, root);
						w = x_parent->right;
					}

					std::swap(x_parent, w);
					if(w->right != NULL) w->right->color = _rb_tree_black;
					_rb_tree_rotate_left(w_parent, root);
					break;
				}
			}
			else
			{
				_rb_tree_node_base* w = x_parent->left;
				if(w->color == _rb_tree_red)
				{
					std::swap(x_parent->color, w->color);
					_rb_tree_rotate_right(x_parent, root);
					w = x_parent->left;
				}
				
				if( (w->right == NULL || w->right->color == _rb_tree_black) &&
					(w->left == NULL || w->left->color == _rb_tree_black) )
				{
					w->color = _rb_tree_black;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else 
				{
					if( w->left == NULL || w->left->color == _rb_tree_black) 
					{
						if(w->right != NULL) w->right = _rb_tree_black;

						w->color = _rb_tree_red;
						_rb_tree_rotate_left(w, root);
						w = x_parent->left;
					}

					std::swap(x_parent, w);
					if(w->left != NULL) w->left->color = _rb_tree_black;
					_rb_tree_rotate_right(w_parent, root);
					break;
				}
			}
		}
	}

	if(x) x->color = _rb_tree_black;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position)
{
	_rb_tree_node_base *y = _rb_tree_rebalance_for_erase(position.node, header->parent,
		                                                 header->left, header->right);

	destroy(y);
	node_count--;
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last)
{
	if(first == begin() && last == end())
		clear();
	else
		while(first != last)
			erase(first++)
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_erase(link_type x)
{
	// erase without reblance
	while( x != NULL)
	{
		_erase(right(x));
		y = left(x);
		destroy_node(x);
		x = y;
	}
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear()
{
	if (node_count != 0)
	{
		_erase(root());
		node_count = 0;
		leftmost() = header;
		rightmost() = header;
		root() = 0;
	}
}

#endif