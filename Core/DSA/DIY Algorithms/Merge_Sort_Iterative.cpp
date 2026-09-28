#include <algorithm>
#include <vector>
#include <iostream>
#include <climits>
#include "helper.h"
using namespace std;


//merges a[l...mid] and a[mid+1...r] and puts it into a[l...r]
void _merge(vector<int> &input, int l, int mid, int r)
{
	vector<int>output;
	int p1=l;
	int p2=mid+1;
	while(p1<=mid && p2<=r)
	{
		if(input[p1]<input[p2])
			output.push_back(input[p1++]);
		else
			output.push_back(input[p2++]);
	}
	while(p1<=mid)
		output.push_back(input[p1++]);
	while(p2<=r)
		output.push_back(input[p2++]);
	copy(output.begin(),output.end(),input.begin()+l);
}

//splits a[l...r] and merges the "halves", puts it back into a[l...r]
void merge(vector<int> &a, int l, int r)
{
	int n=a.size();
	int mid=min((l+r)/2,n-1); //takes care of case where mid>=n
	r=min(r,n-1); //takes care of case where r>=n
	_merge(a, l, mid, r);
}

void merge_sort(vector<int> &a)
{
	int n=a.size();
	for(int i=2;i<=2*n;i*=2)
	{
		for(int j=0;j<n;j+=i)
			merge(a,j,j+i-1);
	}
}

int main()
{
	cout<<"Testing Merge Sort\n\n"<<endl;
	test_sort(merge_sort,"nlogn");
}