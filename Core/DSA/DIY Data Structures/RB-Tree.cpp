#include <bits/stdc++.h>
#include "helper.h"
using namespace std;

enum Color {RED,BLACK};

struct node
{
	int key;
	Color color;
	node* left;
	node* right;
	node* parent;

	node()
	{
		key=0;
		color=RED;
		left=nullptr;
		right=nullptr;
		parent=nullptr;
	}

	node(int key,Color color,node* parent)
	{
		this->key=key;
		this->color=color;
		this->parent=parent;
		left=nullptr;
		right=nullptr;
	}
};

struct RBTree{
	node* root=nullptr;

	private:

	Color colorOf(node* n) 
	{ 
		return (n==nullptr)?(BLACK):(n->color); 
	}

	node* &where(node* p) //returns leash by which p is owned by its parent or the tree in case p is the root.
	{
		if(p==root)
			return root;
		
		if(p->parent->left==p)
			return p->parent->left;
		else
			return p->parent->right;
	}

	node* &whichChild(node* parent, node* child) //returns leash by which child is owned by its parent
	{
		if(parent->left==child)
			return parent->left;
		else
			return parent->right;
	}

	/*  rotateLeft(x): x's right child y moves up.
	 *
	 *        x                   y
	 *       / \                 / \
	 *      A   y      ==>      x   C
	 *         / \             / \
	 *        B   C           A   B 
	 */

	void rotateLeft(node* x) //Assumes x is not null and x->right is not null
	{
		node* &leash=where(x); //leash is how x is owned - either by its parent or as the root.
		node* y=x->right;
		node* B=y->left;

		leash=y;
		y->parent=x->parent;
		y->left=x;
		x->parent=y;

		x->right=B;
		if(B!=nullptr)
			B->parent=x;
	}


	/*  rotateRight(y): y's left child x moves up.  (mirror of rotateLeft)
	 *
	 *          y               x
	 *         / \             / \
	 *        x   C    ==>    A   y
	 *       / \                 / \
	 *      A   B               B   C
	 */

	void rotateRight(node* y) //assumes y is not null and y->left is not null
	{
		node* &leash=where(y); //leash is how y is owned - either by its parent or as the root.
		node* x=y->left;
		node* B=x->right;

		leash=x;
		x->right=y;
		x->parent=y->parent;

		y->left=B;
		y->parent=x;
		if(B!=nullptr)
			B->parent=y;
	}

	public:

	/*  Prints the tree top-down (see visualise_binary in helper.h).
	 *  Labels show key + colour; red nodes are also printed in red unless
	 *  colour is false (use false when output goes to a file).
	 */
	void visualise(bool colour = true)
	{
		visualise_binary<node>(root,
			[](node* n) { return to_string(n->key) + (n->color == RED ? "R" : "B"); },
			colour ? function<bool(node*)>([](node* n) { return n->color == RED; }) : nullptr);
	}

	node* find(int x) //standard BST search, nothing new here
	{
		node* p=root;
		while(p!=nullptr)
		{
			if(p->key==x)
				return p; //key found
			else if(x<p->key)
				p=p->left;
			else
				p=p->right;
		}
		return nullptr; //key not found
	}

	void insert(int x) //ignores duplicates
	{
		if(find(x)!=nullptr)
			return; //key already exists

		//----------------------- normal insertion in BST as a red node -------------------------------
		if(root==nullptr)
		{
			root=new node();
			root->key=x;
			root->color=BLACK; //root is always black
			return;
		}

		node* curr=root;
		node* prev=nullptr;
		while(curr!=nullptr)
		{
			prev=curr;
			if(x<curr->key)
				curr=curr->left;
			else
				curr=curr->right;
		}
		curr=new node(x,RED,prev); //key,color,parent
		prev->left=(x<prev->key)?(curr):(prev->left);
		prev->right=(x>prev->key)?(curr):(prev->right);
		// ----------------------- normal insertion done -----------------------------------------


		// ----------------------- Fix-up -----------------------

		node *z=curr;
		while(colorOf(z->parent)==RED) //the only violation possible
		{
			node* p=z->parent; //parent
			node* g=p->parent; //grandparent, has to exist since p is red and root is black
			bool is_p_LeftChild=(p==g->left); 
			node* u=(is_p_LeftChild)?(g->right):(g->left); //uncle of z, could be null

			if(colorOf(u)==RED)
			{
				/*  CASE 1: uncle red -> recolour, continue from g.
				 *  (2-3-4: g's node was a full 4-node; split it, g moves up.)
				 *  Same whether p is a left or right child, and whichever
				 *  side of p z is on.
				 *
				 *          g(B)                  g(R)  <- new z
				 *         /    \                /    \
				 *       p(R)   u(R)    ==>    p(B)   u(B)
				 *       /                     /
				 *     z(R)                  z(R)
				 */
				
				g->color=RED;
				p->color=BLACK;
				u->color=BLACK;
				z=g;
			}
			else if(is_p_LeftChild)
			{
				/*  CASE 2a: uncle black, p is g's LEFT child.
				 *  (2-3-4: g's node was a 3-node and had room; just reshape it.)
				 *
				 *  Zig-zag first (z is p's RIGHT child): rotateLeft(p).
				 *
				 *        g(B)               g(B)
				 *        /   \              /   \
				 *      p(R)  u(B)    ==>  z(R)  u(B)
				 *        \                /
				 *        z(R)          p(R)
				 *
				 *  Then rename: z = old p (now at the bottom), p = z->parent.
				 *  It's now a straight line.
				 */

				if(z==p->right)
				{
					rotateLeft(p);
					swap(p,z); 
				}
				
				/*
				 *
				 *  Straight line (z is p's LEFT child): rotateRight(g), recolour.
				 *
				 *            g(B)                 p(B)
				 *           /    \               /    \
				 *         p(R)   u(B)  ==>     z(R)   g(R)
				 *         /                              \
				 *       z(R)                             u(B)
				 *
				 *  Subtree top is black again (p instead of g): done.
				 */
				
				rotateRight(g);
				p->color=BLACK;
				g->color=RED;
			}
			else
			{
				/*  CASE 2b: uncle black, p is g's RIGHT child. (mirror of 2a)
				 *
				 *  Zig-zag first (z is p's LEFT child): rotateRight(p).
				 *
				 *      g(B)               g(B)
				 *         \                  \
				 *         p(R)     ==>       z(R)
				 *         /                     \
				 *       z(R)                    p(R)
				 */

				if(z==p->left)
				{
					rotateRight(p);
					swap(p,z);
				}

				 /*  Straight line (z is p's RIGHT child): rotateLeft(g), recolour.
				 *
				 *          g(B)                     p(B)
				 *         /    \                   /    \
				 *       u(B)   p(R)     ==>      g(R)   z(R)
				 *                 \              /
				 *                 z(R)         u(B)
				 *
				 *  Subtree top is black again: done.
				 */

				rotateLeft(g);
				p->color=BLACK;
				g->color=RED;
			}
		}
		//----------------------- Fix-up done -----------------------

		// root is always black, could have turned red in case 1 so recolour it to black. Preserves both invariants.
		root->color=BLACK;
	}
};

// ----------------------------- testing -----------------------------

/*  Checks every red-black invariant in n's subtree and returns its
 *  black-height (counting the null leaves as 1). Fails an assert on the
 *  first violation found.
 *    - BST order: every key strictly between lo and hi
 *    - parent pointers: n->parent == par
 *    - no red node has a red child
 *    - left and right subtrees have equal black-heights
 */
int checkSubtree(node* n, node* par, long lo, long hi)
{
	if (n == nullptr)
		return 1;
	assert(n->parent == par);
	assert(lo < n->key && n->key < hi);
	if (n->color == RED)
	{
		assert(n->left  == nullptr || n->left->color  == BLACK);
		assert(n->right == nullptr || n->right->color == BLACK);
	}
	int lh = checkSubtree(n->left,  n, lo, n->key);
	int rh = checkSubtree(n->right, n, n->key, hi);
	assert(lh == rh);
	return lh + (n->color == BLACK ? 1 : 0);
}

int validate(RBTree& t)   // returns the root's black-height
{
	assert(t.root == nullptr || t.root->color == BLACK);
	return checkSubtree(t.root, nullptr, LONG_MIN, LONG_MAX);
}

void collectKeys(node* n, vector<int>& out)   // in-order
{
	if (n == nullptr) return;
	collectKeys(n->left, out);
	out.push_back(n->key);
	collectKeys(n->right, out);
}

int height(node* n)
{
	return n == nullptr ? 0 : 1 + max(height(n->left), height(n->right));
}

int main()
{
	// 1. The pen-and-paper exercise, step by step
	cout << "=== Exercise: 5 30 40 70 35 45 60 65 ===\n\n";
	RBTree a;
	for (int k : {5, 30, 40, 70, 35, 45, 60, 65})
	{
		a.insert(k);
		cout << "after inserting " << k << ":\n";
		a.visualise();
		validate(a);
	}

	// 2. Sorted input: a plain BST would become a straight line
	cout << "=== Sorted input 1..15 ===\n\n";
	RBTree b;
	for (int k = 1; k <= 15; k++)
		b.insert(k);
	b.visualise();
	validate(b);

	// 3. Duplicates and find
	b.insert(7);
	b.insert(7);
	vector<int> keys;
	collectKeys(b.root, keys);
	assert(keys.size() == 15);                 // duplicates ignored
	assert(b.find(7) != nullptr && b.find(7)->key == 7);
	assert(b.find(100) == nullptr);
	cout << "duplicates and find: OK\n";

	// 4. Random stress test against std::set
	mt19937 rng(42);
	for (int trial = 0; trial < 200; trial++)
	{
		RBTree t;
		set<int> ref;
		int n = rng() % 2000;
		for (int i = 0; i < n; i++)
		{
			int k = rng() % 5000;
			t.insert(k);
			ref.insert(k);
			validate(t);
		}
		vector<int> got;
		collectKeys(t.root, got);
		assert(got == vector<int>(ref.begin(), ref.end()));
	}
	cout << "random stress test (200 trees): OK\n";

	// 5. Height stays logarithmic: h <= 2 log2(n+2) - 2
	RBTree big;
	int N = 1000000;
	for (int k = 0; k < N; k++)
		big.insert(k);                         // worst case for a plain BST
	int h = height(big.root);
	cout << "1,000,000 sorted inserts: height = " << h
	     << "  (bound 2*log2(n+2)-2 = " << 2 * log2(N + 2.0)-2 << ")\n";
	assert(h <= 2 * log2(N + 2.0) - 2);
	validate(big);

	cout << "\nall tests passed\n";
}