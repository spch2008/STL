// impl_stl.cpp : Defines the entry point for the console application.
//

#include "memory.h"
#include "vector.h"
#include <iostream>
int main(int argc, char* argv[])
{
	vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);

	vector<int>::iterator iter;
	iter = vec.begin();

	std::cout << *iter << std::endl;

	system("pause");
	return 0;
}

