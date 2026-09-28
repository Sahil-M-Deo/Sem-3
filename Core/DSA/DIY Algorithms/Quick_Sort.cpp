#include "helper.h"
#include <utility>
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>
using namespace std;

int pick_pivot(vector<int>&a, int l=-1, int r=-1)
{
	if(l==-1)
		l=0,r=a.size()-1;

	return a[l];
	//other choices are middle, random, median of median, etc etc
}

void qsort(vector<int>&a)
{
	if(a.size()==0)
		return;

	vector<int>L,G,E;
	int pivot=pick_pivot(a);
	for(int i=0;i<a.size();i++)
	{
		if(a[i]<pivot)
			L.push_back(a[i]);
		else if(a[i]>pivot)
			G.push_back(a[i]);
		else
			E.push_back(a[i]);
	}
	qsort(L);
	qsort(G);
	copy(L.begin(),L.end(),a.begin());
	copy(E.begin(),E.end(),a.begin()+L.size());
	copy(G.begin(),G.end(),a.begin()+L.size()+E.size());
}

void _inplace_qsort(vector<int>&a, int l,int r)
{
	if(l>=r)
		return;
	int pivot=pick_pivot(a,l,r);

	int L=l,R=r; //L is where the next element of the smaller partition is to be placed and R correspondingly to the greater partition
	for(int i=l;i<=R;) //end at R, not r! Stop when we reach the left boundary of > pivot partition
	{
		if(a[i]>pivot)
			swap(a[i],a[R--]); 
		else if(a[i]<pivot)
			swap(a[i++],a[L++]); //we know that all elements at <=indices have been processed already
		else
			i++;
	}
	_inplace_qsort(a,l,L-1);
	_inplace_qsort(a,R+1,r);
}

void inplace_qsort(vector<int>&a)
{
	_inplace_qsort(a, 0, a.size()-1);
}

int main()
{
	cout<<"Testing normal qsort\n\n"<<endl;
	test_sort(qsort,"nlogn");
	cout<<"\n\nTesting inplace:\n\n"<<endl;
	test_sort(inplace_qsort,"nlogn");
}