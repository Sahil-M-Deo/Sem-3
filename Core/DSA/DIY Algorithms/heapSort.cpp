#include <vector>
#include <iostream>
#include <climits>
#include "helper.h"
using namespace std;

void bubbleDown(vector<int> &a, int size, int start, int key)
{
	auto parent=[](int i)
	{
		return (i-1)/2;
	};

	auto left=[](int i)
	{
		return (2*i+1);
	};

	auto right=[](int i)
	{
		return (2*i+2);
	};

	int curr=start;
	a[curr]=key;
	while (left(curr)<size && right(curr)<size) 
	{
		if(a[left(curr)]>a[right(curr)])
		{
			if(a[left(curr)]>a[curr])
			{
				swap(a[left(curr)],a[curr]);
				curr=left(curr);
			}
			else
				break;				
		}
		else 
		{
			if(a[right(curr)]>a[curr])
			{
				swap(a[right(curr)],a[curr]);
				curr=right(curr);
			}
			else
				break;	
		}
	}
	if(left(curr)<size && a[left(curr)]>a[curr])
		swap(a[left(curr)],a[curr]);
};

void heapSort(vector<int> &a)
{
	auto makeMaxHeap=[](vector<int> &a)
	{
		int n=a.size();
		for(int j=n/2-1;j>=0;j--)
			bubbleDown(a,n,j,a[j]);
	};

	makeMaxHeap(a);
	int n=a.size();
	for(int i=n-1;i>0;i--)
	{
		swap(a[0],a[i]);
		bubbleDown(a,i,0,a[0]);
	}
}

int main()
{
	cout<<"Testing heap sort...\n\n"<<endl;
	test_sort(heapSort, "nlogn");
}