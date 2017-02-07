/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_LIST_H
#define INC_LIST_H

#include <cstddef>

namespace std
{

template <class T>
class list
{
	class Node
	{
	public:
		T value;
		Node *prev;
		Node *next;

		Node() : prev(NULL), next(NULL){}
		Node(const T& value) : prev(NULL), next(NULL), value(value) {}

	private:

		friend class list;
	};

	Node *head, *tail;
	size_t sz;
	bool loop;

	Node *tmp;

public:
	friend class list;

	list(list &that)
	{
		this->loop = that.loop;
		this->head = NULL;
		this->tail = NULL;
		this->sz = 0;
		this->tmp = NULL;

		for(iterator it = that.begin(); it != that.end(); it++)
		{
			this->push_back(*it);
		}
	}
	
	list &operator=(list &that)
	{
		clear();
		
		this->loop = that.loop;

		for(iterator it = that.begin(); it != that.end(); it++)
		{
			this->push_back(*it);
		}
		
		return *this;
	}

	list(bool circular = false) : head(NULL), tail(NULL), sz(0), loop(circular)
	{
		
	}
	
	~list()
	{
		clear();
	}
	
	inline void clear()
	{
		if(tail)
			tail->next = NULL;

		while(head != NULL)
		{
			tmp = head->next;
			delete head;
			head = tmp;
		}

		tail = NULL;
		sz = 0;
	}

	inline void detach()
	{
		head = tail = NULL;
		sz = 0;
	}

	inline bool empty() const
	{
		return sz == 0;
	}

	inline size_t size() const
	{
		return sz;
	}

	inline T& front()
	{
		return head->value;
	}

	inline T& back()
	{
		return tail->value;
	}

	void push_front(const T& value)
	{
		tmp = new Node(value);

		if(empty())
		{
			head = tail = tmp;
		}
		else
		{
			tmp->next = head;
			head->prev = tmp;
			head = tmp;
		}

		if(loop)
		{
			tmp->prev = tail;
			tail->next = head;
		}

		sz++;
	}

	void push_back(const T& value)
	{
		tmp = new Node(value);

		if(empty())
		{
			head = tail = tmp;
		}
		else
		{
			tail->next = tmp;
			tmp->prev = tail;
			tail = tmp;
		}

		if(loop)
		{
			tmp->next = head;
			head->prev = tail;
		}

		sz++;
	}

	void pop_front()
	{
		tmp = head;

		if(head == tail)
		{
			head = tail = NULL;
		}
		else
		{
			head = head->next;

			if(loop)
			{
				head->prev = tail;
				tail->next = head;
			}
			else
			{
				head->prev = NULL;
			}
		}

		delete tmp;
		sz--;
	}

	void pop_back()
	{
		tmp = tail;

		if(head == tail)
		{
			head = tail = NULL;
		}
		else
		{
			tail = tail->prev;

			if(loop)
			{
				tail->next = head;
				head->prev = tail;
			}
			else
			{
				tail->next = NULL;
			}
		}

		delete tmp;
		sz--;
	}

	class iterator
	{
		Node *node;

		friend class list;

	public:

		iterator(Node *n = NULL) : node(n) {}

		inline bool operator==(const iterator& rhs) const
		{
			return node == rhs.node;
		}

		inline bool operator!=(const iterator& rhs) const
		{
			return node != rhs.node;
		}

		inline T& operator*() const
		{
			return node->value;
		}

		inline iterator operator+(int n)
		{
			Node *tmp = node;
			
			if(n < 0)
			{
				n = -n;
				for(int i = 0; i < n && tmp != NULL; i++)
					tmp = tmp->prev;
			}
			else
			{
				for(int i = 0; i < n && tmp != NULL; i++)
					tmp = tmp->next;
			}

			return tmp;
		}

		inline iterator& operator++()
		{
			if(node)
				node = node->next;
			
			return *this;
		}

		inline iterator operator++(int)
		{
			iterator tmp = *this;
 			if(node) node = node->next;
			
			return tmp;
		}

		inline iterator& operator+=(int n)
		{
			if(n < 0)
			{
				n = -n;
				for(int i = 0; i < n && node != NULL; i++)
					node = node->prev;
			}
			else
			{
				for(int i = 0; i < n && node != NULL; i++)
					node = node->next;
			}

			return *this;
		}

		inline iterator operator-(int n)
		{
			Node *tmp = node;

			if(n < 0)
			{
				n = -n;
				for(int i = 0; i < n && tmp != NULL; i++)
					tmp = tmp->next;
			}
			else
			{
				for(int i = 0; i < n && tmp != NULL; i++)
					tmp = tmp->prev;
			}

			return tmp;
		}

		inline iterator& operator--()
		{
			if(node)
				node = node->prev;

			return *this;
		}

		inline iterator operator--(int)
		{
			iterator tmp = *this;
			if(node) node = node->prev;
			return tmp;
		}
		
		inline iterator& operator-=(int n)
		{
			if(n < 0)
			{
				n = -n;
				for(int i = 0; i < n && node != NULL; i++)
					node = node->next;
			}
			else
			{
				for(int i = 0; i < n && node != NULL; i++)
					node = node->prev;
			}

			return *this;
		}

		inline iterator previous()
		{
			return node->prev;
		}
	
		inline iterator next()
		{
			return node->next;
		}
	};

	inline iterator begin()
	{
		return head;
	}

	inline iterator rbegin()
	{
		return tail;
	}

	inline iterator end()
	{
		return NULL;
	}

	inline iterator rend()
	{
		return NULL;
	}

	iterator erase(iterator pos)
	{
		tmp = pos.node;
		pos++;

		if(tmp == head)
		{
			pop_front();
		}
		else if(tmp == tail)
		{
			pop_back();
		}
		else
		{
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			delete tmp;
			sz--;
		}

		return pos;
	}

	iterator insert(iterator pos, const T& value)
	{
		if(pos == begin())
		{
			push_front(value);
			return head;
		}
		else if(pos == end())
		{
			push_back(value);
			return tail;
		}
		else
		{
			tmp = new Node(value);

			tmp->next = pos.node;
			tmp->prev = pos.node->prev;

			pos.node->prev->next = tmp;
			pos.node->prev = tmp;

			sz++;
			return tmp;
		}
	}

	void remove( const T& value )
	{
		if(loop && tail)
			tail->next = NULL;

		iterator it(head);
		while(it != end())
		{
			if((*it) == value)
				it = erase(it);
			else
				it++;
		}

		if(loop && tail)
			tail->next = head;
	}

	iterator find(const T& value)
	{
		if(loop && tail)
			tail->next = NULL;

		iterator it(head);

		while(it != end() && (*it) != value)
			it++;

		if(loop && tail)
			tail->next = head;

		return it;
	}

	iterator move_backward(iterator pos)
	{
		T tmpVal		= pos.node->value;
		iterator oldPos = pos;
		
		pos += 2;
		
		erase(oldPos);

		return insert(pos, tmpVal);
	}

	iterator move_forward(iterator pos)
	{
		T tmpVal		= pos.node->value;
		iterator oldPos = pos;
		
		pos--;

		erase(oldPos);

		return insert(pos, tmpVal);
	}

	void splice(iterator pos, list<T> &lst)
	{
		if(pos == begin())
		{
			pos.node->prev = lst.tail;
			lst.tail->next = pos.node;
			head = lst.head;
		}
		else if(pos == end())
		{
			tail->next = lst.head;
			lst.head->prev = tail;
			tail = lst.tail;
		}
		else
		{
			lst.tail->next = pos.node;
			lst.head->prev = pos.node->prev;
			pos.node->prev->next = lst.head;
			pos.node->prev = lst.tail;
		}
		
		if(loop)
		{
			head->prev = tail;
			tail->next = head;
		}
		else
		{
			head->prev = NULL;
			tail->next = NULL;
		}

		sz += lst.sz;
		lst.sz = 0;

		lst.head = NULL;
		lst.tail = NULL;
	}

	void set_circular(bool enable)
	{
		if(enable == true)
		{
			if(loop == false)
			{
				if(!empty())
				{
					head->prev = tail;
					tail->next = head;
				}

				loop = true;
			}
		}
		else
		{
			if(loop == true)
			{
				if(!empty())
				{
					head->prev = NULL;
					tail->next = NULL;
				}

				loop = false;
			}
		}
	}
};

} /* namespace std */

#endif /* INC_LIST_H */
