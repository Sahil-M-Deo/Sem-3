#include <bits/stdc++.h>
using namespace std;
#define pii pair<int,int>

vector<int> bfs_path(vector<vector<int>>& adj, int s, int t) //returns a shortest path from s to t
{
	int n=adj.size();
	if(n==0)
		return {};

	queue<int>q;
	q.push(s);
	vector<int>pred(n,-1);
	pred[s]=s;
	while(!q.empty())
	{
		int l=q.size();
		while(l--)
		{
			int curr=q.front();
			q.pop();
			for(auto x:adj[curr])
			{
				if(pred[x]!=-1)
					continue;
				q.push(x);
				pred[x]=curr;
			}
		}
	}

	if(pred[t]==-1)
		return {};

	vector<int>result;
	while(t!=s)
	{
		result.push_back(t);
		t=pred[t];
	}
	result.push_back(s);
	reverse(result.begin(),result.end());
	return result;
}

#include <random>
int main()
{
	// Assumes: vector<int> bfs_path(vector<vector<int>>& adj, int s, int t)
	//   adj[u] = list of v for each edge u -> v (undirected graphs are stored both ways), vertices 0..n-1.
	//   Returns one shortest path s, ..., t (fewest edges; any shortest one is accepted),
	//   or an EMPTY vector if t is unreachable. For t == s the answer is {s}.

	int passed = 0, failed = 0;

	auto addEdge = [](vector<vector<int>>& g, int u, int v, bool directed = false) {
		g[u].push_back(v);
		if (!directed) g[v].push_back(u);
	};

	// reference distances: relax every edge until nothing changes (nothing like a BFS)
	auto reference = [](const vector<vector<int>>& g, const vector<int>& sources) {
		int n = g.size();
		vector<int> d(n, -1);
		for (int s : sources) d[s] = 0;
		for (bool changed = true; changed;) {
			changed = false;
			for (int u = 0; u < n; ++u) {
				if (d[u] == -1) continue;
				for (int v : g[u])
					if (d[v] == -1 || d[u] + 1 < d[v]) { d[v] = d[u] + 1; changed = true; }
			}
		}
		return d;
	};

	auto report = [&](const string& name, const string& err) {
		if (err.empty()) ++passed;
		else { ++failed; cout << "FAIL [" << name << "]: " << err << "\n"; }
	};

	auto check = [&](const string& name, const vector<vector<int>>& g, int s) {
		int n = g.size();
		vector<int> exp = reference(g, {s});
		for (int t = 0; t < n; ++t) {
			vector<vector<int>> copy = g;
			vector<int> p = bfs_path(copy, s, t);
			string err;
			if (exp[t] == -1) {
				if (!p.empty()) err = "t is unreachable, expected an empty path, got " + to_string(p.size()) + " vertices";
			} else if (p.empty() || p.front() != s || p.back() != t) {
				err = "path must start at s and end at t";
			} else if ((int)p.size() - 1 != exp[t]) {
				err = "path has " + to_string(p.size() - 1) + " edges, shortest is " + to_string(exp[t]);
			} else {
				for (size_t i = 0; i + 1 < p.size() && err.empty(); ++i)
					if (find(g[p[i]].begin(), g[p[i]].end(), p[i + 1]) == g[p[i]].end())
						err = "uses a missing edge " + to_string(p[i]) + "->" + to_string(p[i + 1]);
			}
			report(name + ", s=" + to_string(s) + " t=" + to_string(t), err);
		}
	};

	// ---- hand-made cases ----
	{
		vector<vector<int>> g(1);
		check("single vertex", g, 0);
	}
	{
		vector<vector<int>> g(5);   // path 0-1-2-3-4
		for (int v = 1; v < 5; ++v) addEdge(g, v - 1, v);
		check("path from end", g, 0);
		check("path from middle", g, 2);
	}
	{
		vector<vector<int>> g(4);   // the long way round is found first in adjacency order
		addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 2, 3); addEdge(g, 0, 3);
		check("square, direct edge listed last", g, 0);
	}
	{
		vector<vector<int>> g(6);   // two components
		addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 3, 4); addEdge(g, 4, 5);
		check("disconnected (from 0)", g, 0);
		check("disconnected (from 4)", g, 4);
	}
	{
		vector<vector<int>> g(4);   // isolated start
		addEdge(g, 1, 2); addEdge(g, 2, 3);
		check("isolated source", g, 0);
	}
	{
		vector<vector<int>> g(3);
		addEdge(g, 0, 1); addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 1, 1); addEdge(g, 2, 2);
		check("parallel edges + self loops", g, 0);
	}
	{
		vector<vector<int>> g(4);   // directed cycle: distances go one way only
		addEdge(g, 0, 1, true); addEdge(g, 1, 2, true); addEdge(g, 2, 3, true); addEdge(g, 3, 0, true);
		check("directed cycle from 0", g, 0);
		check("directed cycle from 2", g, 2);
	}
	{
		vector<vector<int>> g(4);   // directed: 3 reaches 0, not the other way
		addEdge(g, 3, 0, true); addEdge(g, 0, 1, true); addEdge(g, 1, 2, true);
		check("directed, unreachable", g, 0);
	}
	{
		int n = 8;                  // complete graph: everything at distance 1
		vector<vector<int>> g(n);
		for (int u = 0; u < n; ++u) for (int v = u + 1; v < n; ++v) addEdge(g, u, v);
		check("complete graph", g, 3);
	}
	{
		vector<vector<int>> g(7);   // binary tree: levels 0,1,1,2,2,2,2
		addEdge(g, 0, 1); addEdge(g, 0, 2); addEdge(g, 1, 3); addEdge(g, 1, 4); addEdge(g, 2, 5); addEdge(g, 2, 6);
		check("binary tree from root", g, 0);
		check("binary tree from leaf", g, 6);
	}
	{
		int R = 4, C = 5;           // 4x5 grid graph
		vector<vector<int>> g(R * C);
		for (int r = 0; r < R; ++r) for (int c = 0; c < C; ++c) {
			if (r + 1 < R) addEdge(g, r * C + c, (r + 1) * C + c);
			if (c + 1 < C) addEdge(g, r * C + c, r * C + c + 1);
		}
		check("grid from corner", g, 0);
	}

	// ---- random small graphs vs the reference ----
	mt19937 rng(12345);
	for (int it = 0; it < 300; ++it) {
		int n = rng() % 12 + 1;
		bool directed = rng() % 2;
		vector<vector<int>> g(n);
		int m = rng() % (2 * n + 1);
		for (int k = 0; k < m; ++k) addEdge(g, rng() % n, rng() % n, directed);
		string name = "random #" + to_string(it) + (directed ? " (directed)" : "");
		check(name, g, rng() % n);
	}

	// ---- one bigger graph ----
	{
		int n = 2000;
		vector<vector<int>> g(n);
		for (int k = 0; k < 4000; ++k) addEdge(g, rng() % n, rng() % n);
		check("big random", g, 0);
	}

	cout << passed << " passed, " << failed << " failed\n";
	return failed != 0;
}