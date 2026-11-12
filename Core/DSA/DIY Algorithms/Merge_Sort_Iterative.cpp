#include <algorithm>
#include <vector>
#include <iostream>
#include <climits>
#include "helper.h"
using namespace std;


//merges a[l...mid] and a[mid+1...r] and puts it into a[l...r]
void _merge(vector<int> &input, vector<int> &output, int l, int mid, int r)
{
	
	int p1=l;
	int p2=mid+1;
	int p=l;
	while(p1<=mid && p2<=r)
	{
		if(input[p1]<input[p2])
			output[p++]=input[p1++];
		else
			output[p++]=input[p2++];
	}
	while(p1<=mid)
		output[p++]=input[p1++];
	while(p2<=r)
		output[p++]=input[p2++];
}

//splits a[l...r] and merges the "halves", puts it back into a[l...r]
void merge(vector<int> &input, vector<int> &output, int l, int r)
{
	int n=input.size();
	int mid=min((l+r)/2,n-1); //takes care of case where mid>=n
	r=min(r,n-1); //takes care of case where r>=n
	_merge(input, output, l, mid, r);
}

void merge_sort(vector<int> &input)
{
	int n=input.size();
	vector<int>temp(n);

	vector<int>* A=&input; //pointer to input vector
	vector<int>* B=&temp; //pointer to temp vector
	for(int i=2;i<=2*n;i*=2) //max cap on total window in which merge is called
	{
		for(int j=0;j<n;j+=i)
			merge(*A,*B,j,j+i-1);
		swap(A,B);
	}
	if(&input != A)
		input=move(temp);
}

int main()
{
	cout<<"Testing Merge Sort\n\n"<<endl;
	test_sort(merge_sort,"nlogn");
}