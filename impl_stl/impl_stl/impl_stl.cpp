// impl_stl.cpp : Defines the entry point for the console application.
//

#include "memory.h"
#include "vector.h"
#include "list.h"
#include <iostream>

#include "stl_deque.h"
int main(int argc, char* argv[])
{
	
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
   

	deque<int> de(2,5);
	std::cout << de.front() << std::endl;

	de.push_back(6);

	std::cout << de.back() << std::endl;

	de.pop_back();
	std::cout << de.back() << std::endl;
	system("pause");
	return 0;
}

