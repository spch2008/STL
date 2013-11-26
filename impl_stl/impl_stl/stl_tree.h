#ifndef _STL_TREE
#define _STL_TREE

#include "stl_iterator.h"
#include <stddef.h>
#include "memory.h"

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
	typedef _rb_tree_node_color_type          color_type;


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
	static const Key& key(link_type x)   { return (Key&)x->color; }

};


#endif