// impl_stl.cpp : Defines the entry point for the console application.
//

#include "memory.h"
#include "vector.h"
#include "list.h"
#include <iostream>

#include "stl_queue.h"
int main(int argc, char* argv[])
{
	/*
	
	list<int> vec;
	vec.push_back(10);
	vec.push_back(2);

	vec.push_back(9);
	vec.push_back(5);
	vec.push_back(8);

	list<int>::iterator iter;
	for(iter = vec.begin(); iter != vec.end(); iter++)
		std::cout << *iter << " ";
	std::cout << std::endl;


	vec.sort();
	
   	

	for(iter = vec.begin(); iter != vec.end(); iter++)
		std::cout << *iter << " ";
	std::cout << std::endl;
   */
	/*
	deque<int> de(2,5);
	std::cout << de.front() << std::endl;

	de.push_back(6);

	std::cout << de.back() << std::endl;

	de.pop_back();
	std::cout << de.back() << std::endl;
	*/
	/*
	std::cout << "stl_stack" << std::endl;

	stack<int> s;
	s.push(1);
	s.push(2);
	s.push(3);

	std::cout << s.top() << std::endl;
	s.pop();

	std::cout << s.top() << std::endl;
	s.pop();

	std::cout << s.top() << std::endl;
	s.pop();
	*/

	/*
	std::cout << "stl_queue" << std::endl;
	queue<int> q;
	q.push(1);
	q.push(2);
	q.push(3);

	std::cout << q.front() << std::endl;
	q.pop();

	std::cout << q.front() << std::endl;
	q.pop();

	std::cout << q.front() << std::endl;
	q.pop();
	*/

	/*
	vector<int> h;
	h.push_back(0);
	h.push_back(1);
	h.push_back(2);
	h.push_back(3);
	h.push_back(4);
	h.push_back(8);
	h.push_back(9);
	h.push_back(3);
	h.push_back(5);
	
	make_heap(h.begin(), h.end());
	vector<int>::iterator iter;

	sort_heap(h.begin(), h.end());
	for(iter = h.begin(); iter != h.end(); ++iter)
		std::cout << *iter << " ";
	std::cout << std::endl;

	*/
	
	
	
	int ia[9] = {0,1,2,3,4,8,9,3,5};
	priority_queue<int> q(ia, ia + 9);

	while(q.empty() == false)
	{
		std::cout << q.top() << " ";
		q.pop();
	}
	std::cout << std::endl;
	
	system("pause");
	return 0;
}

