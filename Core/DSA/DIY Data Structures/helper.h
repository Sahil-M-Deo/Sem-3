#pragma once
/*  helper.h - ASCII tree visualisers
 *
 *  visualise_binary(root)  any binary tree whose nodes have key/left/right
 *                          (BST, red-black tree, ...)
 *  visualise_234(root)     any multiway tree whose nodes have keys/children
 *                          (2-3-4 tree, B-tree, ...)
 *  visualise_radix(root)   any radix tree / trie whose nodes have
 *                          children (vector<pair<Node*, string>>) and is_end;
 *                          sideways, one edge per line, full words shown
 *  visualise_radix_topdown(root)
 *                          same trees, drawn top-down with each edge's
 *                          string written on the edge
 *
 *  All are templates, so they work with whatever your node struct is called,
 *  as long as it has the fields above. Output uses box-drawing characters,
 *  so the terminal must be UTF-8.
 */

#include <bits/stdc++.h>

namespace viz_detail
{
	// Prints a grid of cells (one string per cell, since box-drawing
	// characters are multi-byte), trimming trailing spaces on each row.
	inline void printRow(const std::vector<std::string>& row)
	{
		int last = (int)row.size() - 1;
		while (last >= 0 && row[last] == " ")
			last--;
		for (int x = 0; x <= last; x++)
			std::cout << row[x];
		std::cout << "\n";
	}

	// Writes s into row so that it is centred on column c.
	inline void writeCentred(std::vector<std::string>& row, int c, const std::string& s,
	                         const std::string& before = "", const std::string& after = "")
	{
		int start = c - (int)s.size() / 2;
		for (int i = 0; i < (int)s.size(); i++)
			row[start + i] = std::string(1, s[i]);
		if (!s.empty())
		{
			row[start] = before + row[start];
			row[start + s.size() - 1] += after;
		}
	}

	// ---------------- binary trees ----------------

	// In-order walk: each node's depth and its left-to-right position.
	template <class Node>
	void layoutBinary(Node* n, int d, std::vector<Node*>& order, std::unordered_map<Node*, int>& depth)
	{
		if (n == nullptr) return;
		layoutBinary(n->left, d + 1, order, depth);
		depth[n] = d;
		order.push_back(n);
		layoutBinary(n->right, d + 1, order, depth);
	}

	// ---------------- multiway (2-3-4) trees ----------------

	const int GAP = 2;    // spaces between sibling subtrees

	template <class Node>
	std::string label234(Node* n)          // e.g. "[30 40]"
	{
		std::string s = "[";
		for (int i = 0; i < (int)n->keys.size(); i++)
			s += (i ? " " : "") + std::to_string(n->keys[i]);
		return s + "]";
	}

	// Width of the column block that n's subtree needs.
	template <class Node>
	int width234(Node* n, std::unordered_map<Node*, int>& w)
	{
		int total = 0;
		for (int i = 0; i < (int)n->children.size(); i++)
			total += width234(n->children[i], w) + (i ? GAP : 0);
		return w[n] = std::max(total, (int)label234(n).size());
	}

	// Lays out n's subtree in columns [x0, x0+w[n]); records each label's centre.
	template <class Node>
	void place234(Node* n, int x0, int d, std::unordered_map<Node*, int>& w,
	              std::unordered_map<Node*, int>& centre, std::vector<std::vector<Node*>>& levels)
	{
		if ((int)levels.size() <= d) levels.resize(d + 1);
		levels[d].push_back(n);
		if (n->children.empty())
		{
			centre[n] = x0 + w[n] / 2;
			return;
		}
		int childTotal = -GAP;
		for (Node* c : n->children) childTotal += w[c] + GAP;
		int x = x0 + (w[n] - childTotal) / 2;          // centre the children under n
		for (Node* c : n->children)
		{
			place234(c, x, d + 1, w, centre, levels);
			x += w[c] + GAP;
		}
		centre[n] = (centre[n->children.front()] + centre[n->children.back()]) / 2;
	}
}

/*  Prints a binary tree top-down. Each node gets its own column, chosen by
 *  its in-order position, so labels never overlap and keys read sorted left
 *  to right:
 *
 *                40
 *         ┌───────┴───────┐
 *        30              60
 *     ┌───┴───┐       ┌───┴───┐
 *    5       35      45      70
 *
 *  label     : text for a node (default: its key)
 *  highlight : nodes for which it returns true are printed in red
 *              (ANSI colour; pass nothing if output goes to a file)
 *
 *  Red-black example:
 *    visualise_binary<node>(t.root,
 *        [](node* n){ return std::to_string(n->key) + (n->color == RED ? "R" : "B"); },
 *        [](node* n){ return n->color == RED; });
 */
template <class Node>
void visualise_binary(Node* root,
                      std::function<std::string(Node*)> label = nullptr,
                      std::function<bool(Node*)> highlight = nullptr)
{
	using namespace viz_detail;
	if (root == nullptr) { std::cout << "(empty)\n\n"; return; }
	if (!label) label = [](Node* n) { return std::to_string(n->key); };

	std::vector<Node*> order;
	std::unordered_map<Node*, int> depth;
	layoutBinary(root, 0, order, depth);

	int w = 0, H = 0;
	for (Node* n : order) { w = std::max(w, (int)label(n).size()); H = std::max(H, depth[n]); }
	int step = w + 1;                       // column width per node

	std::unordered_map<Node*, int> col;     // centre column of each label
	for (int i = 0; i < (int)order.size(); i++)
		col[order[i]] = i * step + w / 2;

	std::vector<std::vector<std::string>> grid(2 * H + 1, std::vector<std::string>(order.size() * step, " "));

	for (Node* n : order)
	{
		int r = 2 * depth[n], c = col[n];
		bool red = highlight && highlight(n);
		writeCentred(grid[r], c, label(n), red ? "\033[31m" : "", red ? "\033[0m" : "");

		if (n->left || n->right)            // connector row:  ┌──┴──┐  ┌──┘  └──┐
		{
			int L = n->left  ? col[n->left]  : c;
			int R = n->right ? col[n->right] : c;
			for (int x = L; x <= R; x++) grid[r + 1][x] = "─";
			if (n->left)  grid[r + 1][L] = "┌";
			if (n->right) grid[r + 1][R] = "┐";
			grid[r + 1][c] = (n->left && n->right) ? "┴" : (n->left ? "┘" : "└");
		}
	}

	for (auto& row : grid)
		printRow(row);
	std::cout << "\n";
}

/*  Prints a multiway tree top-down. Parents sit centred over their children,
 *  and the connector row joins them:
 *
 *         [30 40 60]
 *     ┌─────┬──┴──┬──────┐
 *    [5]  [35]  [45]  [65 70]
 *
 *  ┼ appears where the parent sits directly above a child.
 */
template <class Node>
void visualise_234(Node* root)
{
	using namespace viz_detail;
	if (root == nullptr) { std::cout << "(empty)\n\n"; return; }

	std::unordered_map<Node*, int> w, centre;
	std::vector<std::vector<Node*>> levels;
	width234(root, w);
	place234(root, 0, 0, w, centre, levels);

	int cols = w[root];
	for (int d = 0; d < (int)levels.size(); d++)
	{
		std::vector<std::string> row(cols, " "), conn(cols, " ");
		bool anyChildren = false;
		for (Node* n : levels[d])
		{
			writeCentred(row, centre[n], label234(n));

			if (n->children.empty()) continue;
			anyChildren = true;
			int L = centre[n->children.front()], R = centre[n->children.back()];
			for (int x = L; x <= R; x++) conn[x] = "─";
			for (Node* c : n->children) conn[centre[c]] = "┬";
			conn[L] = "┌";
			conn[R] = "┐";
			int c = centre[n];
			conn[c] = (conn[c] == "┬") ? "┼" : "┴";   // parent may sit right above a child
		}
		printRow(row);
		if (anyChildren)
			printRow(conn);
	}
	std::cout << "\n";
}

namespace viz_detail
{
	// ---------------- radix trees ----------------

	template <class Node>
	void printRadix(Node* n, const std::string& indent, const std::string& soFar,
	                bool showWords, std::unordered_set<Node*>& seen)
	{
		// children sorted by edge label so the output is stable
		std::vector<std::pair<Node*, std::string>> kids(n->children.begin(), n->children.end());
		std::sort(kids.begin(), kids.end(),
		          [](const auto& a, const auto& b) { return a.second < b.second; });

		for (int i = 0; i < (int)kids.size(); i++)
		{
			Node* c = kids[i].first;
			const std::string& edge = kids[i].second;
			bool last = (i == (int)kids.size() - 1);

			std::cout << indent << (last ? "└── " : "├── ");
			std::cout << "\"" << edge << "\"";
			if (c == nullptr) { std::cout << "  (null child!)\n"; continue; }
			if (edge.empty()) std::cout << "  (empty label!)";
			if (c->is_end)
			{
				std::cout << " \033[32m●\033[0m";
				if (showWords) std::cout << "  " << soFar + edge;
			}
			if (!seen.insert(c).second) { std::cout << "  (already visited: cycle or shared node!)\n"; continue; }
			std::cout << "\n";

			printRadix(c, indent + (last ? "    " : "│   "), soFar + edge, showWords, seen);
		}
	}
}

/*  Prints a radix tree sideways, one edge per line, edge labels in quotes.
 *  ● (green) marks a node where a stored word ends:
 *
 *    (root)
 *    └── "t"
 *        ├── "e" ●  te
 *        │   ├── "am" ●  team
 *        │   └── "st" ●  test
 *        │       └── "ing" ●  testing
 *        └── "oast" ●  toast
 *
 *  showWords : also print the full word next to each ●
 *  Problems that would break a radix tree (empty labels, null children,
 *  a node reached twice) are flagged inline instead of crashing.
 */
template <class Node>
void visualise_radix(Node* root, bool showWords = true)
{
	using namespace viz_detail;
	if (root == nullptr) { std::cout << "(empty)\n\n"; return; }

	std::cout << "(root)";
	if (root->is_end) std::cout << " \033[32m●\033[0m" << (showWords ? "  \"\"" : "");
	std::cout << "\n";
	if (root->children.empty()) std::cout << "(no words)\n";

	std::unordered_set<Node*> seen{root};
	printRadix(root, "", "", showWords, seen);
	std::cout << "\n";
}

namespace viz_detail
{
	// ---------------- radix trees, top-down ----------------

	template <class Node>
	using RadixKids = std::unordered_map<Node*, std::vector<std::pair<Node*, std::string>>>;

	// Width of the column block for n's subtree; it must also fit the label
	// on the edge coming into n (inLen).
	template <class Node>
	int widthRadix(Node* n, int inLen, RadixKids<Node>& kids, std::unordered_map<Node*, int>& w)
	{
		auto& k = kids[n];
		for (auto& e : n->children)
			if (e.first) k.push_back(e);
		std::sort(k.begin(), k.end(),
		          [](const auto& a, const auto& b) { return a.second < b.second; });
		int total = 0;
		for (int i = 0; i < (int)k.size(); i++)
			total += widthRadix(k[i].first, (int)k[i].second.size(), kids, w) + (i ? GAP : 0);
		return w[n] = std::max({total, inLen, 1});
	}

	template <class Node>
	void placeRadix(Node* n, int x0, int d, RadixKids<Node>& kids, std::unordered_map<Node*, int>& w,
	                std::unordered_map<Node*, int>& centre, std::vector<std::vector<Node*>>& levels)
	{
		if ((int)levels.size() <= d) levels.resize(d + 1);
		levels[d].push_back(n);
		auto& k = kids[n];
		if (k.empty())
		{
			centre[n] = x0 + w[n] / 2;
			return;
		}
		int childTotal = -GAP;
		for (auto& e : k) childTotal += w[e.first] + GAP;
		int x = x0 + (w[n] - childTotal) / 2;
		for (auto& e : k)
		{
			placeRadix(e.first, x, d + 1, kids, w, centre, levels);
			x += w[e.first] + GAP;
		}
		centre[n] = (centre[k.front().first] + centre[k.back().first]) / 2;
	}

	// Like writeCentred, but grows the row instead of writing out of bounds.
	inline void writeCentredSafe(std::vector<std::string>& row, int c, const std::string& s)
	{
		int start = std::max(0, c - (int)s.size() / 2);
		if (start + (int)s.size() > (int)row.size()) row.resize(start + s.size(), " ");
		for (int i = 0; i < (int)s.size(); i++)
			row[start + i] = std::string(1, s[i]);
	}
}

/*  Prints a radix tree top-down with each edge's string written on the edge.
 *  ● (green) is a node where a stored word ends, ○ is any other node:
 *
 *                ○
 *                │
 *                t
 *           ┌────┴────┐
 *           e        oast
 *           ●         ●
 *        ┌──┴──┐
 *        am    st
 *        ●     ●
 *              │
 *             ing
 *              ●
 *
 *  Reading the labels from the root down to a ● spells a stored word.
 */
template <class Node>
void visualise_radix_topdown(Node* root)
{
	using namespace viz_detail;
	if (root == nullptr) { std::cout << "(empty)\n\n"; return; }

	RadixKids<Node> kids;
	std::unordered_map<Node*, int> w, centre;
	std::vector<std::vector<Node*>> levels;
	widthRadix(root, 0, kids, w);
	placeRadix(root, 0, 0, kids, w, centre, levels);
	if (kids[root].empty()) { std::cout << (root->is_end ? "● (only \"\")\n\n" : "(no words)\n\n"); return; }

	int cols = w[root];
	for (int d = 0; d < (int)levels.size(); d++)
	{
		std::vector<std::string> nodes(cols, " "), conn(cols, " "), labels(cols, " ");
		bool anyChildren = false;
		for (Node* n : levels[d])
		{
			nodes[centre[n]] = n->is_end ? "\033[32m●\033[0m" : "○";

			auto& k = kids[n];
			if (k.empty()) continue;
			anyChildren = true;
			int L = centre[k.front().first], R = centre[k.back().first];
			for (int x = L; x <= R; x++) conn[x] = "─";
			for (auto& e : k) conn[centre[e.first]] = "┬";
			int c = centre[n];
			if (L == R) conn[c] = "│";                         // single child, straight down
			else
			{
				conn[L] = "┌";
				conn[R] = "┐";
				conn[c] = (conn[c] == "┬") ? "┼" : "┴";
			}
			for (auto& e : k)
				writeCentredSafe(labels, centre[e.first], e.second.empty() ? "\"\"" : e.second);
		}
		printRow(nodes);
		if (anyChildren)
		{
			printRow(conn);
			printRow(labels);
		}
	}
	std::cout << "\n";
}
