#include <vector>
#include <algorithm>
#include "helper.h"
using namespace std;

void selectionSort(vector<int> &a)
{
	bool sorted=false;
	for(int k=a.size();k>1;k--) //number of items left to sort (0...k-1)
	{
		int mx=0;
		sorted=true;
		for(int i=1;i<k;i++)
		{
			if(a[i]>=a[mx])
				mx=i;
			else
				sorted=false; //since a[greater idx] is smaller than a[smaller idx], counter example
		}
		swap(a[mx],a[k-1]);
		if(sorted)
			break;
	}
}

int main()
{
	cout<<"Testing selectionSort\n\n"<<endl;
	test_sort(selectionSort,"n^2");
}