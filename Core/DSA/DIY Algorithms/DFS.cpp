#include <bits/stdc++.h>
using namespace std;
#define pii pair<int,int>

//We will just do an application for dfs coz plain dfs 
// literally doesn't do anything its just an exploration 

vector<int> topoSort_iterative(vector<vector<int>>& adj)
{
	int n=adj.size();
	if(n==0)
		return {};

	vector<int> activated(n);
	vector<int> active(n);
	stack<pii> s;

	vector<int> result;
	for(int i=0;i<n;i++)
	{
		if(activated[i]&&!active[i])
			continue;
		s.push({i,0});
		activated[i]=active[i]=1;
		while(!s.empty())
		{	
			auto &[curr,level]=s.top();
			if(level==adj[curr].size())
			{
				s.pop();
				result.push_back(curr);
				active[curr]=0;
			}
			else
			{
				int nbr=adj[curr][level];
				level++;
				if(active[nbr])
					return {}; //cycle detected
				if(!activated[nbr])
				{
					s.push({nbr,0});
					activated[nbr]=active[nbr]=1;
				}
			}
		}
	}
	reverse(result.begin(),result.end());
	return result;
}

vector<int> topoSort_recursive(vector<vector<int>>& adj)
{
	int n=adj.size();
	if(n==0)
		return {};

	vector<int> activated(n);
	vector<int> active(n);

	vector<int> result;

	function<void(int)> dfs=[&](int i){	
		activated[i]=active[i]=1;
		for(auto x:adj[i])
		{	
			if(activated[x]&&!active[x])
				continue;
			if(active[x])
			{
				result={};
				return;
			}
			dfs(x);
			if(result.empty())
				return;
		}
		active[i]=0;
		result.push_back(i);
	};

	for(int i=0;i<n;i++)
	{
		if(activated[i]&&!active[i])
			continue;
		dfs(i);
		if(result.empty())
			return {}; //cycle detected
	}
	reverse(result.begin(),result.end());
	return result;
}

#include <random>
#include <random>
int main()
{
	// Assumes: vector<int> topoSort_iterative(vector<vector<int>>& adj)
	//     and  vector<int> topoSort_recursive(vector<vector<int>>& adj)   (every case runs on both)
	//   adj[u] = list of v for each directed edge u -> v, vertices 0..n-1.
	//   Returns a topological order of ALL n vertices, or an EMPTY vector if the graph has a cycle.
	// Any valid order is accepted (topological orders aren't unique).

	int passed = 0, failed = 0;

	auto addEdge = [](vector<vector<int>>& g, int u, int v) { g[u].push_back(v); };

	// reference 1: is there a cycle? (Kahn's algorithm: peel off in-degree-0 vertices)
	auto hasCycle = [](const vector<vector<int>>& g) {
		int n = g.size();
		vector<int> indeg(n, 0);
		for (int u = 0; u < n; ++u) for (int v : g[u]) indeg[v]++;
		vector<int> q;
		for (int u = 0; u < n; ++u) if (indeg[u] == 0) q.push_back(u);
		for (size_t h = 0; h < q.size(); ++h)
			for (int v : g[q[h]]) if (--indeg[v] == 0) q.push_back(v);
		return (int)q.size() != n;
	};

	auto checkOne = [&](const string& name, vector<int> (*algo)(vector<vector<int>>&), const vector<vector<int>>& g) {
		int n = g.size();
		bool cyc = hasCycle(g);
		vector<vector<int>> copy = g;
		vector<int> got = algo(copy);

		string err;
		if (cyc) {
			if (!got.empty()) err = "graph has a cycle, expected an empty vector, got " + to_string(got.size()) + " vertices";
		} else if ((int)got.size() != n) {
			err = "expected all " + to_string(n) + " vertices, got " + to_string(got.size());
		} else {
			vector<int> pos(n, -1);
			for (int i = 0; i < n && err.empty(); ++i) {
				int v = got[i];
				if (v < 0 || v >= n) err = "vertex " + to_string(v) + " out of range";
				else if (pos[v] != -1) err = "vertex " + to_string(v) + " appears twice";
				else pos[v] = i;
			}
			for (int u = 0; u < n && err.empty(); ++u)
				for (int v : g[u])
					if (pos[u] > pos[v]) { err = "edge " + to_string(u) + " -> " + to_string(v) + " goes backwards"; break; }
		}
		if (err.empty()) ++passed;
		else { ++failed; cout << "FAIL [" << name << "]: " << err << "\n"; }
	};

	auto check = [&](const string& name, const vector<vector<int>>& g) {
		checkOne("iterative: " + name, topoSort_iterative, g);
		checkOne("recursive: " + name, topoSort_recursive, g);
	};

	// ---- hand-made cases ----
	{
		vector<vector<int>> g(1);
		check("single vertex", g);
	}
	{
		vector<vector<int>> g(4);   // no edges at all: every vertex must still be in the answer
		check("no edges", g);
	}
	{
		vector<vector<int>> g(5);
		addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 2, 3); addEdge(g, 3, 4);
		check("chain", g);
	}
	{
		vector<vector<int>> g(5);   // chain given backwards: 4 -> 3 -> 2 -> 1 -> 0
		addEdge(g, 4, 3); addEdge(g, 3, 2); addEdge(g, 2, 1); addEdge(g, 1, 0);
		check("reverse chain", g);
	}
	{
		vector<vector<int>> g(4);   // diamond: 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3
		addEdge(g, 0, 1); addEdge(g, 0, 2); addEdge(g, 1, 3); addEdge(g, 2, 3);
		check("diamond", g);
	}
	{
		// cross edge to an already-FINISHED vertex is NOT a cycle (the classic active/activated mix-up)
		vector<vector<int>> g(3);
		addEdge(g, 0, 1); addEdge(g, 0, 2); addEdge(g, 2, 1);
		check("cross edge (no cycle)", g);
	}
	{
		vector<vector<int>> g(6);   // two separate DAGs, second one not reachable from 0
		addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 5, 4); addEdge(g, 4, 3);
		check("disconnected", g);
	}
	{
		vector<vector<int>> g(3);   // parallel edges
		addEdge(g, 0, 1); addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 1, 2);
		check("parallel edges", g);
	}
	{
		vector<vector<int>> g(2);
		addEdge(g, 1, 1);
		check("self loop = cycle", g);
	}
	{
		vector<vector<int>> g(2);
		addEdge(g, 0, 1); addEdge(g, 1, 0);
		check("2-cycle", g);
	}
	{
		vector<vector<int>> g(5);   // cycle hidden in a part not reachable from 0
		addEdge(g, 0, 1); addEdge(g, 2, 3); addEdge(g, 3, 4); addEdge(g, 4, 2);
		check("cycle not reachable from 0", g);
	}
	{
		vector<vector<int>> g(6);   // DAG part first, cycle deep down
		addEdge(g, 0, 1); addEdge(g, 0, 2); addEdge(g, 2, 3); addEdge(g, 3, 4); addEdge(g, 4, 5); addEdge(g, 5, 3);
		check("cycle deep in the graph", g);
	}
	{
		// classic "getting dressed" DAG
		vector<vector<int>> g(9);
		addEdge(g, 0, 1); addEdge(g, 0, 7); addEdge(g, 1, 2); addEdge(g, 1, 7); addEdge(g, 2, 5);
		addEdge(g, 3, 2); addEdge(g, 3, 4); addEdge(g, 4, 5); addEdge(g, 6, 7);
		check("textbook DAG", g);
	}

	// ---- random graphs: half DAGs (edges follow a hidden permutation), half may have cycles ----
	mt19937 rng(12345);
	for (int it = 0; it < 300; ++it) {
		int n = rng() % 12 + 1;
		bool dag = it % 2 == 0;
		vector<int> perm(n);
		iota(perm.begin(), perm.end(), 0);
		shuffle(perm.begin(), perm.end(), rng);
		vector<int> rankOf(n);
		for (int i = 0; i < n; ++i) rankOf[perm[i]] = i;
		vector<vector<int>> g(n);
		int m = rng() % (2 * n + 1);
		for (int k = 0; k < m; ++k) {
			int u = rng() % n, v = rng() % n;
			if (u == v) continue;
			if (dag && rankOf[u] > rankOf[v]) swap(u, v);
			addEdge(g, u, v);
		}
		check("random #" + to_string(it) + (dag ? " (DAG)" : ""), g);
	}

	// ---- big: a long chain (recursion depth 20000) ----
	{
		int n = 20000;
		vector<vector<int>> g(n);
		for (int v = 0; v + 1 < n; ++v) addEdge(g, v, v + 1);
		check("chain of 20000", g);
		addEdge(g, n - 1, n / 2);
		check("chain of 20000 + back edge", g);
	}

	cout << passed << " passed, " << failed << " failed\n";
	return failed != 0;
}