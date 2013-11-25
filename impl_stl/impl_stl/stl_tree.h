#ifndef _STL_TREE
#define _STL_TREE

#include "stl_iterator.h"
#include <stddef.h>

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

	//but when x is NULL£¬ how to do
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

#endif