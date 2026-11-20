#include <bits/stdc++.h>
#include <random>
#include "helper.h"
using namespace std;

const long long INF=(long long)INT_MAX+1;
const long long NEGINF=(long long)INT_MIN-1;

struct Node{
	long long val;
	vector<Node*> next; //depending on which level
};

struct SkipList{
	int memory_used; //number of total entries in next vector of all nodes
	int levels;
	int size; 
	Node* start;
	Node* end;

	mt19937 rng;

	SkipList()
	{
		start=new Node{NEGINF};
		end=new Node{INF};
		start->next.push_back(end);

		memory_used=1; 
		levels=1;
		size=0;
		rng=mt19937(time(0));
	}

	void insert(int x)
	{
		if(find(x).first)
			return;

		int height=promote_height();
		Node* niu=new Node{x};
		niu->next.resize(height+1);

		Node* curr=start;
		for(int i=levels-1;i>=0;i--)
		{
			while(curr->next[i]->val<x)
				curr=curr->next[i];
			if(i<=height)
			{
				insert_after(curr,niu,i);
				memory_used+=1;
			}
		}

		for(int i=levels;i<=height;i++)
			init_level(niu),memory_used+=2;
		size++;
	}

	pair<Node*,int> find(int x) //returns predecessor
	{
		Node* curr=start;
		for(int i=levels-1;i>=0;i--)
		{
			while(curr->next[i]->val<x)
				curr=curr->next[i];
			if(curr->next[i]->val==x)
				return {curr,i};
		}
		return {nullptr,0};
	}

	void erase(int x)
	{
		Node* curr=start;
		Node* target=nullptr;
		for (int i=levels-1;i>=0;i--)
		{
			while(curr->next[i]->val<x)
				curr=curr->next[i];
			if(curr->next[i]->val==x) 
			{
				target=curr->next[i];
				curr->next[i]=target->next[i];
				memory_used--;
			}
		}
		if(target)
		{
			delete target; 
			size--;
		}
	}

	private:

	void init_level(Node* x)
	{
		start->next.push_back(x);
		x->next[levels]=end;
		levels++;
	}

	int promote_height()
	{
		int h=0;
		while(rng()%2) 
			h++;
		return h;
	}

	void insert_after(Node* curr, Node* x, int lvl)
	{
		x->next[lvl]=curr->next[lvl];
		curr->next[lvl]=x;
	}
};







bool check(SkipList s1, set<int>s2)
{
	mt19937 rng(time(0));
	auto report_error=[&]{
		cout<<"Failed test of size "<<s2.size()<<endl;
		cout<<"Structure reported size: "<<s1.size<<endl;

		auto curr=s1.start->next[0];
		int size_of_level_0=1;

		cout<<"Structure Level 0:"<<endl;
		while(curr!=s1.end)
		{
			cout<<curr->val<<" ";
			size_of_level_0++,curr=curr->next[0];
		}
		cout<<endl;
		size_of_level_0--;

		cout<<"Traversed level 0 size: "<<size_of_level_0<<endl;

		for(auto y:s2)
			cout<<y<<" ";
		cout<<endl;
	};

	int size_of_level_0=1;
	auto curr=s1.start->next[0];
	while(curr!=s1.end)
		size_of_level_0++,curr=curr->next[0];
	size_of_level_0--;

	if(s1.size!=s2.size() || s1.size != size_of_level_0)
	{
		report_error();
		return 0;
	}
	
	for(auto x:s2)
	{
		if(!s1.find(x).first)
		{
			report_error();
			return 0;
		}
	}

	for(int i=0;i<10*s2.size();i++)
	{
		int random=rng();
		bool found1=s1.find(random).first;
		bool found2=s2.count(random); //contains() needs c++20
		if(found1!=found2)
		{
			report_error();
			return 0;
		}
	}

	return 1;
}

#include <ctime>
int main()
{
	mt19937 rng(time(0));
	int tests=100;

	#define rng() rng()%1
	//brute force all sizes
	for(int sz=1;sz<=tests;sz++)
	{
		SkipList s1;
		set<int>s2;
		vector<int>l;
		for(int i=0;i<sz;i++)
		{
			int random=rng();
			s1.insert(random);
			s2.insert(random);
			l.push_back(random);
		}

		for(int i=0;i<sz/2;i++)
		{
			int random=rng()%l.size();
			s1.erase(l[random]);
			s2.erase(l[random]);
		}

		for(int i=0;i<10;i++)
		{
			int random=rng();
			s1.erase(random);
			s2.erase(random);
		}
		
		check(s1,s2);
	}

	//edge cases
	{
		SkipList s1;
		set<int>s2;
		check(s1,s2);
	}

	cout<<endl;cout<<endl;cout<<endl;cout<<endl;

	cout << "\033[32m" << "ALL TESTS PASSED" << "\033[0m" << endl;

	cout<<endl;cout<<endl;cout<<endl;cout<<endl;
}