#include <algorithm>
#include <vector>
#include <iostream>
#include <climits>
using namespace std;
#include <bits/stdc++.h>

#include <queue>
#define pii pair<int,int>

struct Entry{
	int u,v,weight;
	bool operator<(const Entry &other)const{ 
		return weight<other.weight;
	}

	bool operator>(const Entry &other)const{  //can refer to this using greater<Entry>
		return weight>other.weight;
	}
};

struct CompareWeight{ //the only point of this is to be used in the priority queue 
    bool operator()(const Entry& a,const Entry& b) const{
        return a.weight>b.weight; 
    }
};

vector<pii> Prims_MST(vector<vector<pii>>& adj)
{
	int n=adj.size();
	vector<bool>visited(n,0);
	vector<pii>result;
	priority_queue<Entry,vector<Entry>,greater<Entry>>pq; 
	int s=0;
	visited[s]=1;
	while(result.size()<n-1)
	{
		for(auto &e:adj[s])
			if(!visited[e.first])
				pq.push(Entry{e.first,s,e.second}); //discoveree, discoverer, weight
		while(visited[pq.top().u]) 
			pq.pop();

		auto [discoveree,discoverer,weight]=pq.top();
		pq.pop();
		result.push_back({discoverer,discoveree});
		visited[discoveree]=1;
		s=discoveree;
	}

	return result;
}



struct DSU {
	vector<int>p;
	DSU(int n)
	{ 
		p=vector<int>(n,-1); //all sizes 1
	}

	void unite(int a,int b)
	{
		a=find(a);
		b=find(b);
		if(a==b) 
			return;
		if(p[a]>p[b])
			swap(a,b); 
		//a is now bigger
		p[a]+=p[b];
		p[b]=a;
	}

	int find(int a)
	{
		vector<int>seen;
		while(p[a]>=0)
		{
			seen.push_back(a);
			a=p[a];
		}
		for(int x:seen)
			p[x]=a;
		return a;
	}
};

vector<pii> Kruskal_MST(vector<vector<pii>>& adj)
{
	int n=adj.size();
	DSU hi(n);
	vector<pii>result;
	vector<Entry>edges;
	for(int i=0;i<n;i++)
		for(auto &[x,y]:adj[i])
			edges.push_back({i,x,y});
	sort(edges.begin(),edges.end());

	int e=0;
	while(result.size()<n-1)
	{
		auto [u,v,weight]=edges[e];
		if(hi.find(u)!=hi.find(v))
		{
			hi.unite(u,v);
			result.push_back({u,v});
		}
		e++;
	}
	return result;
}

#include <random>
int main()
{
	// Assumes: adj[u] holds {v, w} (undirected, stored both ways); MST returns edges as {u, v}.
	// If your adj stores {w, v}, set this to true.
	const bool WEIGHT_FIRST = false;

	int passed = 0, failed = 0;

	auto nbr = [&](const pii& e) { return WEIGHT_FIRST ? e.second : e.first; };
	auto wt  = [&](const pii& e) { return WEIGHT_FIRST ? e.first : e.second; };

	auto addEdge = [&](vector<vector<pii>>& g, int u, int v, int w) {
		if (WEIGHT_FIRST) { g[u].push_back({w, v}); g[v].push_back({w, u}); }
		else              { g[u].push_back({v, w}); g[v].push_back({u, w}); }
	};

	// iterative DSU find so it can be a plain lambda
	auto find = [](vector<int>& p, int x) {
	    int root = x;
	    while (p[root] != root) 
			root = p[root];     // first pass: find the root
	    while (p[x] != root)
		{  // second pass: point every node on the path at the root
	        int next = p[x];
	        p[x] = root;
	        x = next;
	    }
	    return root;
	};

	// reference answer: Kruskal total weight
	auto kruskal = [&](const vector<vector<pii>>& g) {
		int n = g.size();
		vector<array<int, 3>> edges;
		for (int u = 0; u < n; ++u)
			for (auto& e : g[u])
				if (u < nbr(e)) edges.push_back({wt(e), u, nbr(e)});
		sort(edges.begin(), edges.end());
		vector<int> p(n);
		iota(p.begin(), p.end(), 0);
		long long total = 0;
		for (auto& [w, u, v] : edges) {
			int a = find(p, u), b = find(p, v);
			if (a != b) { p[a] = b; total += w; }
		}
		return total;
	};

	auto checkOne = [&](const string& name, vector<pii> (*algo)(vector<vector<pii>>&),
                    vector<vector<pii>> g, long long expected) {
		int n = g.size();
		vector<vector<pii>> copy = g;
		vector<pii> tree = algo(copy);

		string err;
		if ((int)tree.size() != max(0, n - 1))
			err = "expected " + to_string(max(0, n - 1)) + " edges, got " + to_string(tree.size());

		vector<int> p(n);
		iota(p.begin(), p.end(), 0);
		long long total = 0;
		for (auto [u, v] : tree) {
			if (!err.empty()) break;
			if (u < 0 || u >= n || v < 0 || v >= n) { err = "vertex out of range"; break; }
			int best = INT_MAX;
			bool found = false;
			for (auto& e : g[u])
				if (nbr(e) == v) { found = true; best = min(best, wt(e)); }
			if (!found) { err = "edge " + to_string(u) + "-" + to_string(v) + " not in graph"; break; }
			total += best;
			int a = find(p, u), b = find(p, v);
			if (a == b) { err = "cycle at edge " + to_string(u) + "-" + to_string(v); break; }
			p[a] = b;
		}
		if (err.empty() && total != expected)
			err = "total weight " + to_string(total) + ", expected " + to_string(expected);

		if (err.empty()) ++passed;
		else { ++failed; cout << "FAIL [" << name << "]: " << err << "\n"; }
	};

		auto check = [&](const string& name, const vector<vector<pii>>& g, long long expected) {
	    checkOne("Prim's: " + name, Prims_MST, g, expected);
	    checkOne("Kruskal: " + name, Kruskal_MST, g, expected);
	};

	// ---- hand-made cases ----
	{
		vector<vector<pii>> g(1);
		check("single node", g, 0);
	}
	{
		vector<vector<pii>> g(2);
		addEdge(g, 0, 1, 7);
		check("two nodes", g, 7);
	}
	{
		vector<vector<pii>> g(3);
		addEdge(g, 0, 1, 1); addEdge(g, 1, 2, 2); addEdge(g, 0, 2, 3);
		check("triangle", g, 3);
	}
	{
		// textbook 9-node graph, MST = 37
		vector<vector<pii>> g(9);
		addEdge(g, 0, 1, 4);  addEdge(g, 0, 7, 8);  addEdge(g, 1, 7, 11); addEdge(g, 1, 2, 8);
		addEdge(g, 2, 8, 2);  addEdge(g, 2, 5, 4);  addEdge(g, 2, 3, 7);  addEdge(g, 3, 5, 14);
		addEdge(g, 3, 4, 9);  addEdge(g, 4, 5, 10); addEdge(g, 5, 6, 2);  addEdge(g, 6, 8, 6);
		addEdge(g, 6, 7, 1);  addEdge(g, 7, 8, 7);
		check("CLRS graph", g, 37);
	}
	{
		vector<vector<pii>> g(5);   // already a tree (path)
		addEdge(g, 0, 1, 5); addEdge(g, 1, 2, 3); addEdge(g, 2, 3, 9); addEdge(g, 3, 4, 1);
		check("path", g, 18);
	}
	{
		vector<vector<pii>> g(6);   // star
		for (int i = 1; i < 6; ++i) addEdge(g, 0, i, i);
		check("star", g, 15);
	}
	{
		vector<vector<pii>> g(3);
		addEdge(g, 0, 1, 10); addEdge(g, 0, 1, 2); addEdge(g, 0, 1, 5);
		addEdge(g, 1, 2, 4);  addEdge(g, 1, 2, 1);
		check("parallel edges", g, 3);
	}
	{
		vector<vector<pii>> g(3);
		addEdge(g, 0, 0, -100); addEdge(g, 1, 1, -50);   // self loops must be ignored
		addEdge(g, 0, 1, 3); addEdge(g, 1, 2, 4); addEdge(g, 0, 2, 6);
		check("self loops", g, 7);
	}
	{
		vector<vector<pii>> g(4);
		for (int u = 0; u < 4; ++u) for (int v = u + 1; v < 4; ++v) addEdge(g, u, v, 1);
		check("all equal weights", g, 3);
	}
	{
		vector<vector<pii>> g(4);
		addEdge(g, 0, 1, -5); addEdge(g, 1, 2, 0); addEdge(g, 2, 3, -2);
		addEdge(g, 0, 3, 1);  addEdge(g, 0, 2, -1);
		check("zero/negative weights", g, -8);
	}
	{
		int n = 8;   // complete graph, w = |u-v|, MST = n-1
		vector<vector<pii>> g(n);
		for (int u = 0; u < n; ++u) for (int v = u + 1; v < n; ++v) addEdge(g, u, v, v - u);
		check("complete graph", g, n - 1);
	}

	// ---- random small connected graphs vs Kruskal ----
	mt19937 rng(12345);
	for (int it = 0; it < 100; ++it) {
		int n = rng() % 10 + 1;
		vector<vector<pii>> g(n);
		for (int v = 1; v < n; ++v) addEdge(g, rng() % v, v, (int)(rng() % 41) - 10);   // keeps it connected
		int extra = rng() % (3 * n + 1);
		for (int k = 0; k < extra; ++k) {
			int u = rng() % n, v = rng() % n;
			if (u != v) addEdge(g, u, v, (int)(rng() % 41) - 10);
		}
		check("random #" + to_string(it), g, kruskal(g));
	}

	cout << passed << " passed, " << failed << " failed\n";
	return failed != 0;
}

