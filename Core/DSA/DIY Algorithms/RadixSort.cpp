#include <vector>
#include <iostream>
#include <climits>
#include "helper.h"
#include <random>
using namespace std;

void LSD_Radix(vector<vector<int>>& A, int D, int B)
{
	int n=A.size();
	vector<vector<int>> aux(n);
	for(int i=D-1;i>=0;i--)
	{
		vector<int>offset(B);
		for(auto &x: A)
			offset[x[i]]++;

		int sum=0;
		for(int i=0;i<B;i++)
		{
			int tmp=offset[i];
			offset[i]=sum;
			sum+=tmp;
		}

		for(auto &x: A)
			aux[offset[x[i]]++]=std::move(x); //otherwise it is O(nD^2)
		swap(A,aux);
	}
}

// n random keys, each with D digits in [0, B)
vector<vector<int>> random_keys(int n,int D,int B,mt19937 &rng)
{
	vector<vector<int>> A(n,vector<int>(D));
	for(auto &x:A)
		for(int &d:x)
			d=uniform_int_distribution<int>(0,B-1)(rng);
	return A;
}

int main()
{
	cout<<"Testing Radix Sort\n\n"<<endl;
	mt19937 rng(696967);
 
	// Correctness tests: {A, D, B}
	struct Test { vector<vector<int>> A; int D,B; };
	vector<Test> tests={
		{{},3,10},
		{{{5}},1,10},
		{{{2},{1}},1,10},
		{{{1},{2}},1,10},
		{{{1,1},{1,1},{1,1}},2,10},
		{{{0,1},{0,0},{1,0},{1,1}},2,2},
		{{{9,9,9},{0,0,0},{5,5,5},{0,0,9},{9,0,0}},3,10},
		{{{3,1},{1,3},{3,0},{0,3},{2,2}},2,4}
	};
	for(int t=0;t<100;t++)
	{
		int n=rng()%100, D=1+rng()%5, B=2+rng()%20;
		tests.push_back({random_keys(n,D,B,rng),D,B});
	}
 
	for(int tc=0;tc<(int)tests.size();tc++)
	{
		auto a=tests[tc].A, b=a;
		sort(b.begin(),b.end());     // lexicographic = digit 0 most significant
		LSD_Radix(a,tests[tc].D,tests[tc].B);
		if(a!=b)
		{
			cout<<"FAILED test "<<tc<<endl;
			return 1;
		}
	}
	cout<<"All correctness tests passed!\n\n";
 
	// Time complexity test: D and B fixed, so O(D*(n+B)) = O(n)
	//This checks only n and not D,B
	const int D=4,B=256;
	auto c=estimate([&](int n){ return random_keys(n,D,B,rng); },
	                [&](vector<vector<int>>& A){ LSD_Radix(A,D,B); });
	if(c<=Complexity("nlogn"))
		cout<<"Time complexity correct!\n";
	else
		cout<<"Time complexity check failed, your complexity is \n"<<c; 
 
	return 0;
}
