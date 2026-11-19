using namespace std;
#include <bits/stdc++.h>

#define pii pair<long long,long long>

struct Entry{
	long long u,dist;
	bool operator<(const Entry &other)const{ 
		return dist<other.dist;
	}

	bool operator>(const Entry &other)const{  //can refer to this using greater<Entry>
		return dist>other.dist;
	}
};

#define INF LLONG_MAX

vector<long long>Dijkstra(vector<vector<pii>>& adj, long long s)
{
	int n=adj.size();
	vector<long long>result(n,INF);
	priority_queue<Entry,vector<Entry>,greater<Entry>> pq;

	for(int i=0;i<n;i++)
		pq.push({i,INF});
	
	result[s]=0;
	pq.push({s,0});

	while(!pq.empty() && pq.top().dist!=INF)
	{
		auto [extracted,curr]=pq.top();
		pq.pop();
		if(curr>result[extracted])
			continue;
		for(auto [next,dist]:adj[extracted])
		{	
			if(dist+curr<result[next])
			{
				result[next]=dist+curr;
				pq.push({next,result[next]});
			}
		}
	}
	return result;
}

#include <random>
int main()
{
	// Assumes: vector<long long> Dijkstra(vector<vector<pii>>& adj, int s)
	//   adj[u] holds {v, w}; returns dist[v] for every v, UNREACHABLE if v can't be reached.
	// If your adj stores {w, v}, set WEIGHT_FIRST to true.
	// If you return vector<int> with INT_MAX for unreachable, set UNREACHABLE = INT_MAX
	//   (the "long path" case will then show you the int overflow).
	const bool WEIGHT_FIRST = false;
	const long long UNREACHABLE = LLONG_MAX;
	// Optional: if you also wrote  vector<int> Dijkstra_path(vector<vector<pii>>& adj, int s, int t)
	//   (s ... t, empty if unreachable), change this to 1 to check paths too.
	#define HAVE_PATH 0

	int passed = 0, failed = 0;

	auto nbr = [&](const pii& e) { return WEIGHT_FIRST ? e.second : e.first; };
	auto wt  = [&](const pii& e) { return WEIGHT_FIRST ? e.first : e.second; };

	auto addEdge = [&](vector<vector<pii>>& g, int u, int v, int w, bool directed = false) {
		if (WEIGHT_FIRST) { g[u].push_back({w, v}); if (!directed) g[v].push_back({w, u}); }
		else              { g[u].push_back({v, w}); if (!directed) g[v].push_back({u, w}); }
	};

	// reference answer: Bellman-Ford (slow but simple, and nothing like Dijkstra)
	auto bellmanFord = [&](const vector<vector<pii>>& g, int s) {
		int n = g.size();
		vector<long long> d(n, INF);
		d[s] = 0;
		for (int round = 0; round < n; ++round) {
			bool changed = false;
			for (int u = 0; u < n; ++u) {
				if (d[u] == INF) continue;
				for (auto& e : g[u])
					if (d[u] + wt(e) < d[nbr(e)]) { d[nbr(e)] = d[u] + wt(e); changed = true; }
			}
			if (!changed) break;
		}
		return d;
	};

	auto check = [&](const string& name, vector<vector<pii>> g, int s) {
		int n = g.size();
		vector<long long> expected = bellmanFord(g, s);
		vector<vector<pii>> copy = g;
		auto got = Dijkstra(copy, s);

		string err;
		if ((int)got.size() != n)
			err = "expected " + to_string(n) + " distances, got " + to_string(got.size());
		for (int v = 0; v < n && err.empty(); ++v) {
			long long d = (long long)got[v];
			bool unreachable = (expected[v] == LLONG_MAX);
			if (unreachable && d != UNREACHABLE)
				err = "dist[" + to_string(v) + "] = " + to_string(d) + ", but " + to_string(v) + " is unreachable";
			else if (!unreachable && d != expected[v])
				err = "dist[" + to_string(v) + "] = " + to_string(d) + ", expected " + to_string(expected[v]);
		}
#if HAVE_PATH
		for (int t = 0; t < n && err.empty(); ++t) {
			vector<vector<pii>> c2 = g;
			vector<int> path = Dijkstra_path(c2, s, t);
			if (expected[t] == LLONG_MAX) {
				if (!path.empty()) err = "path to unreachable " + to_string(t) + " should be empty";
				continue;
			}
			if (path.empty() || path.front() != s || path.back() != t) {
				err = "path to " + to_string(t) + " must start at s and end at t"; break;
			}
			long long len = 0;
			for (size_t i = 0; i + 1 < path.size() && err.empty(); ++i) {
				long long best = LLONG_MAX;
				for (auto& e : g[path[i]]) if (nbr(e) == path[i + 1]) best = min(best, (long long)wt(e));
				if (best == LLONG_MAX) err = "path to " + to_string(t) + " uses a missing edge " + to_string(path[i]) + "-" + to_string(path[i + 1]);
				else len += best;
			}
			if (err.empty() && len != expected[t])
				err = "path to " + to_string(t) + " has length " + to_string(len) + ", expected " + to_string(expected[t]);
		}
#endif
		if (err.empty()) ++passed;
		else { ++failed; cout << "FAIL [" << name << "]: " << err << "\n"; }
	};

	// ---- hand-made cases ----
	{
		vector<vector<pii>> g(1);
		check("single node", g, 0);
	}
	{
		vector<vector<pii>> g(2);
		addEdge(g, 0, 1, 7);
		check("two nodes", g, 0);
		check("two nodes, start at 1", g, 1);
	}
	{
		vector<vector<pii>> g(3);   // direct edge is NOT the shortest
		addEdge(g, 0, 2, 10); addEdge(g, 0, 1, 3); addEdge(g, 1, 2, 4);
		check("triangle, detour wins", g, 0);
	}
	{
		// a node's first-found distance is not final: needs a decrease-key / re-push
		vector<vector<pii>> g(5);
		addEdge(g, 0, 1, 100); addEdge(g, 0, 2, 1); addEdge(g, 2, 3, 1); addEdge(g, 3, 1, 1); addEdge(g, 1, 4, 1);
		check("improve after discovery", g, 0);
	}
	{
		// textbook 9-node graph
		vector<vector<pii>> g(9);
		addEdge(g, 0, 1, 4);  addEdge(g, 0, 7, 8);  addEdge(g, 1, 7, 11); addEdge(g, 1, 2, 8);
		addEdge(g, 2, 8, 2);  addEdge(g, 2, 5, 4);  addEdge(g, 2, 3, 7);  addEdge(g, 3, 5, 14);
		addEdge(g, 3, 4, 9);  addEdge(g, 4, 5, 10); addEdge(g, 5, 6, 2);  addEdge(g, 6, 8, 6);
		addEdge(g, 6, 7, 1);  addEdge(g, 7, 8, 7);
		check("CLRS graph from 0", g, 0);
		check("CLRS graph from 4", g, 4);
	}
	{
		vector<vector<pii>> g(6);   // two components
		addEdge(g, 0, 1, 2); addEdge(g, 1, 2, 2); addEdge(g, 3, 4, 1); addEdge(g, 4, 5, 1);
		check("disconnected (from 0)", g, 0);
		check("disconnected (from 5)", g, 5);
	}
	{
		vector<vector<pii>> g(4);   // isolated source
		addEdge(g, 1, 2, 3); addEdge(g, 2, 3, 3);
		check("isolated source", g, 0);
	}
	{
		vector<vector<pii>> g(3);
		addEdge(g, 0, 1, 10); addEdge(g, 0, 1, 2); addEdge(g, 0, 1, 5);
		addEdge(g, 1, 2, 4);  addEdge(g, 1, 2, 1);
		check("parallel edges", g, 0);
	}
	{
		vector<vector<pii>> g(3);
		addEdge(g, 0, 0, 5); addEdge(g, 1, 1, 0);   // self loops
		addEdge(g, 0, 1, 3); addEdge(g, 1, 2, 4);
		check("self loops", g, 0);
	}
	{
		vector<vector<pii>> g(5);   // zero-weight edges (allowed in Lab 11)
		addEdge(g, 0, 1, 0); addEdge(g, 1, 2, 0); addEdge(g, 2, 3, 5); addEdge(g, 0, 3, 5); addEdge(g, 3, 4, 0);
		check("zero weights", g, 0);
	}
	{
		vector<vector<pii>> g(4);
		for (int u = 0; u < 4; ++u) for (int v = u + 1; v < 4; ++v) addEdge(g, u, v, 1);
		check("all equal weights", g, 2);
	}
	{
		int n = 8;   // complete graph, w = (v-u)^2: going step by step is always shorter
		vector<vector<pii>> g(n);
		for (int u = 0; u < n; ++u) for (int v = u + 1; v < n; ++v) addEdge(g, u, v, (v - u) * (v - u));
		check("complete graph", g, 0);
	}
	{
		vector<vector<pii>> g(4);   // directed: the way back doesn't exist
		addEdge(g, 0, 1, 1, true); addEdge(g, 1, 2, 1, true); addEdge(g, 2, 3, 1, true); addEdge(g, 3, 0, 1, true);
		check("directed cycle from 0", g, 0);
		check("directed cycle from 2", g, 2);
	}
	{
		vector<vector<pii>> g(4);   // directed: 3 can reach 0 but 0 can't reach 3
		addEdge(g, 3, 0, 1, true); addEdge(g, 0, 1, 2, true); addEdge(g, 1, 2, 2, true);
		check("directed, unreachable", g, 0);
	}
	{
		int n = 6;   // long path of weight 1e9 edges: total 5e9 overflows int
		vector<vector<pii>> g(n);
		for (int v = 1; v < n; ++v) addEdge(g, v - 1, v, 1000000000);
		check("long path (int overflow)", g, 0);
	}

	// ---- random small graphs vs Bellman-Ford ----
	mt19937 rng(12345);
	for (int it = 0; it < 300; ++it) {
		int n = rng() % 10 + 1;
		bool directed = rng() % 2;
		vector<vector<pii>> g(n);
		int m = rng() % (3 * n + 1);          // may be disconnected
		for (int k = 0; k < m; ++k) {
			int u = rng() % n, v = rng() % n;
			int w = (it % 3 == 0) ? (int)(rng() % 3) : (int)(rng() % 50);   // many ties / zeros in some
			addEdge(g, u, v, w, directed);
		}
		check("random #" + to_string(it) + (directed ? " (directed)" : ""), g, rng() % n);
	}

	// ---- one bigger graph, all sources' worth of variety ----
	{
		int n = 300;
		vector<vector<pii>> g(n);
		for (int k = 0; k < 2000; ++k) addEdge(g, rng() % n, rng() % n, (int)(rng() % 1000000000));
		for (int s = 0; s < 5; ++s) check("big random from " + to_string(s), g, s);
	}

	cout << passed << " passed, " << failed << " failed\n";
	return failed != 0;
}