#include "helper.h"
#include <utility>
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>
using namespace std;

void gate(int &i, int &j)
{
	if(i>j)
		swap(i,j);
}

void enable_odd(vector<int>&a)
{
	for(int i=1;i+1<a.size();i+=2)
		gate(a[i],a[i+1]);
}

void enable_even(vector<int>&a)
{
	for(int i=0;i+1<a.size();i+=2)
		gate(a[i],a[i+1]);
}

void sorting_network(vector<int>&a)
{
	for(int i=0;i<=a.size()/2;i++)
	{
		enable_even(a);
		enable_odd(a);
	}
}

int main()
{
	cout<<"Testing network\n\n"<<endl;
	test_sort(sorting_network,"n");
}