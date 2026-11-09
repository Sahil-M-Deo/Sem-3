#include <bits/stdc++.h>
#include "helper.h"
using namespace std;

struct node
{
	vector<int>keys;
	vector<node*>children; //contains either no children or keys.size()+1 children
	node* parent=nullptr;
};

struct Tree_234{
	node* root=nullptr;
	private:

	int find_index(node* p, int x)
	{
	    for(int i=0;i<p->keys.size();i++)
	        if (p->keys[i]==x)
	            return i; // key already exists

	    if (p->children.empty())
	        return -1; // bottom node

	    int i=0;
	    while (i<p->keys.size() && p->keys[i]<x)
	        i++;
	    return i;
	}

	void insertChild(node* p, node* newChild)
	{
		if(newChild==nullptr)
			return;
		p->children.push_back(newChild);
		sort(p->children.begin(),p->children.end(),[](node* a,node* b){return a->keys[0]<b->keys[0];});
		newChild->parent=p;
	}

	void insertKey(node* p, int x)
	{
		p->keys.push_back(x);
		sort(p->keys.begin(),p->keys.end());
	}

	node* inorder_successor(node* p, int x) //returns the node containing the in-order successor of the key x assuming the right subtree exists
	{
		int child_index=find_index(p,x);
		p=p->children[child_index+1];
		while(!p->children.empty())
			p=p->children[0];
		return p;
	}

	public:

	node* find(int x)
	{
		node* p=root;
		while(p!=nullptr)
		{
			int child_index=find_index(p,x);
			if (child_index!=-1 && child_index<(int)p->keys.size() && p->keys[child_index] == x)
				return p; //key found
			else if(child_index==-1)
				return nullptr; //key not found
			p=p->children[child_index];
		}
		return nullptr; //empty tree case
	}

	void insert(int x)
	{
		if(find(x)!=nullptr) //key already exists
			return;

		node* p=root;
		if(root==nullptr)
		{
			root=new node();
			insertKey(root, x);
			return;
		}

		//Lets find leaf to insert into
		int child_index=find_index(p,x);
		while(child_index!=-1)
		{
			p=p->children[child_index];
			child_index=find_index(p,x);
		}
		//found relevant leaf

		node* newChild=nullptr;
		while(p!=nullptr && p->keys.size()==3)
		{	
			if(newChild)
				insertChild(p,newChild);

			int old_middle=p->keys[1]; //store old middle
			insertKey(p, x); //add the new key

			newChild=new node(); //the newChild which is passed up the "recursion" becomes the right child
			newChild->parent=p->parent;

			vector<int>tmp_keys=p->keys;
			p->keys.clear();
			for(auto key:tmp_keys)
			{
				if(key<old_middle)
					insertKey(p, key);
				else if(key>old_middle)
					insertKey(newChild, key);
				//else : ignore the middle key
			}			

			if(!p->children.empty())
			{
				int left_size=p->keys.size()+1;
				for(int i=left_size;i<p->children.size();i++)
					insertChild(newChild,p->children[i]);
				p->children.resize(left_size);
			}

			p=p->parent;
			x=old_middle;
		}

		if(p!=nullptr) //p is not null, meaning we have not reached the root
		{
			insertKey(p, x);
			insertChild(p,newChild);
		}
		else //p is null, meaning we have reached the root and it was full
		{
			node* newRoot=new node();
			insertKey(newRoot, x);
			insertChild(newRoot,root);
			insertChild(newRoot,newChild);
			root=newRoot;
		}
	}


	/*  DELETE
	 *  1. If x is in an internal node, replace it with its in-order successor
	 *     (leftmost key of the bottom node reached by going one step right,
	 *     then left all the way down) and delete that key instead.
	 *
	 *         [.. x ..]                [.. s ..]
	 *            \             ==>        \
	 *            ...                      ...
	 *            /                        /
	 *         [s t]                     [t]      <- now delete from here
	 *
	 *  2. Remove the key from the bottom node. If keys remain: done.
	 *  3. Otherwise repair the empty node e (may repeat upward).
	*/

	void remove(int x) //deletes x assuming only one copy of x exists in the tree
	{
		node* p = find(x);
		if(p==nullptr) //early return if key not found
			return;

		if(p->children.empty())
		{
			erase(p->keys,x);
			if(!p->keys.empty())
				return;
		}
		else
		{
			node* s=inorder_successor(p,x);
			int successor_key=s->keys[0];

			int idx=find_index(p,x);
			p->keys[idx]=successor_key;

			erase(s->keys,successor_key);
			if(!s->keys.empty())
				return;

			p=s; //the empty node to be fixed
		}

		//at this point, p is the empty node to be repaired. It may be the root, in which case we are done.

		node* e = p; //holds the empty node to be repaired
		while (e->keys.empty())
		{
			if(e==root) //guaranteed that root has only one child, which becomes the new root
			{
				/*  ROOT IS EMPTY: its only child becomes the root.
				 *  The tree shrinks by one level, evenly on every path.
				 *
				 *        [ ]
				 *         |          ==>      [b d]   (new root)
				 *       [b d]
				 *	
				 *  (If e has no child, the tree is now empty.)
				 */
				
				if(!e->children.empty())
				{
					root=e->children[0];
					root->parent=nullptr;
				}
				else 
					root=nullptr;
				delete e;
				break; //equivalent to return
			}

			node* P = e->parent;
			int j = std::find(P->children.begin(),P->children.end(), e) - P->children.begin();
			node* left = (j>0)?(P->children[j-1]):(nullptr);
			node* right = (j+1<(int)P->children.size())?(P->children[j+1]):(nullptr);

			if (right && right->keys.size() >= 2)
			{
				/*  TRANSFER FROM RIGHT SIBLING (separator b = P->keys[j])
				 *
				 *     [.. a  b  c ..]              [.. a  d  c ..]
				 *          /   \                        /   \
				 *        [ ]   [d e]       ==>        [b]   [e]
				 *         |    / | \                  / \   / \
				 *         A   B  C  D                A   B C   D
				 *
				 *  b comes down into e, d goes up, and child B moves from the
				 *  sibling's front to e's back (fix B->parent).
				 */
				e->keys.push_back(P->keys[j]);                 // b comes down
				P->keys[j]=right->keys.front();                // d goes up
				right->keys.erase(right->keys.begin());
				if(!right->children.empty())                   // internal level: move B
				{
					node* B=right->children.front();
					right->children.erase(right->children.begin());
					e->children.push_back(B);
					B->parent=e;
				}
				break; //return
			}
			if (left && left->keys.size() >= 2)
			{
				/*  TRANSFER FROM LEFT SIBLING (separator b = P->keys[j-1])
				 *
				 *     [.. a  b ..]                 [.. a  e ..]
				 *         /    \                       /    \
				 *      [d e]   [ ]         ==>       [d]    [b]
				 *      / | \    |                    / \    / \
				 *     A  B  C   D                   A   B  C   D
				 *
				 *  b comes down into e, e goes up, and child C moves from the
				 *  sibling's back to e's front (fix C->parent).
				 */
				e->keys.push_back(P->keys[j-1]);               // b comes down
				P->keys[j-1]=left->keys.back();                // e goes up
				left->keys.pop_back();
				if(!left->children.empty())                    // internal level: move C
				{
					node* C=left->children.back();
					left->children.pop_back();
					e->children.insert(e->children.begin(),C);
					C->parent=e;
				}
				break;
			}

			if (right) //and right has only one key
			{
				/*  MERGE WITH RIGHT SIBLING (separator b = P->keys[j])
				 *
				 *     [.. a  b  c ..]              [.. a  c ..]
				 *          /   \                        |
				 *        [ ]   [d]         ==>        [b d]
				 *         |    / \                    / | \
				 *         A   B   C                  A  B  C
				 *
				 *  b comes down to the front of the sibling, e's child A goes to
				 *  the sibling's front (fix A->parent), then remove b from P and
				 *  e from P->children, and delete e.
				 */
				right->keys.insert(right->keys.begin(),P->keys[j]);   // b comes down
				if(!e->children.empty())                              // internal level: move A
				{
					node* A=e->children[0];
					right->children.insert(right->children.begin(),A);
					A->parent=right;
				}
				P->keys.erase(P->keys.begin()+j);
				P->children.erase(P->children.begin()+j);
				delete e;
			}
			else
			{
				/*  MERGE WITH LEFT SIBLING (separator b = P->keys[j-1])
				 *
				 *     [.. a  b ..]                 [.. a ..]
				 *         /    \                       |
				 *       [d]    [ ]         ==>       [d b]
				 *       / \     |                    / | \
				 *      A   B    C                   A  B  C
				 *
				 *  b comes down to the back of the sibling, e's child C goes to
				 *  the sibling's back (fix C->parent), then remove b from P and
				 *  e from P->children, and delete e.
				 */
				left->keys.push_back(P->keys[j-1]);                   // b comes down
				if(!e->children.empty())                              // internal level: move C
				{
					node* C=e->children[0];
					left->children.push_back(C);
					C->parent=left;
				}
				P->keys.erase(P->keys.begin()+j-1);
				P->children.erase(P->children.begin()+j);
				delete e;
			}
			e = P;   // P lost a key and may now be empty: repeat
		}
	}
};




// ------------------------------- testing --------------------------------

/*  Checks every 2-3-4 invariant in n's subtree and returns its height.
 *    - 1 to 3 keys, sorted, all strictly between lo and hi
 *    - either no children or exactly keys+1 children
 *    - parent pointers correct
 *    - all bottom nodes at the same depth (perfect balance)
 */
int check(node* n, node* par, long lo, long hi)
{
	assert(n->parent==par);
	assert(1<=n->keys.size() && n->keys.size()<=3);
	for(int i=0;i<(int)n->keys.size();i++)
	{
		assert(lo<n->keys[i] && n->keys[i]<hi);
		if(i) assert(n->keys[i-1]<n->keys[i]);
	}
	if(n->children.empty()) return 1;
	assert(n->children.size()==n->keys.size()+1);
	int h=-1;
	for(int i=0;i<(int)n->children.size();i++)
	{
		long l=(i==0)?lo:n->keys[i-1];
		long r=(i==(int)n->keys.size())?hi:n->keys[i];
		int ch=check(n->children[i],n,l,r);
		if(h==-1) h=ch;
		assert(ch==h);
	}
	return h+1;
}

int validate(Tree_234& t)                      // returns the height (0 if empty)
{
	return t.root==nullptr ? 0 : check(t.root,nullptr,LONG_MIN,LONG_MAX);
}

void collectKeys(node* n, vector<int>& out)    // in-order
{
	if(n==nullptr) return;
	for(int i=0;i<(int)n->keys.size();i++)
	{
		if(!n->children.empty()) collectKeys(n->children[i],out);
		out.push_back(n->keys[i]);
	}
	if(!n->children.empty()) collectKeys(n->children.back(),out);
}

int main()
{
	// 1. The pen-and-paper exercise, step by step
	cout<<"=== Exercise: 5 30 40 70 35 45 60 65 ===\n\n";
	Tree_234 a;
	for(int k:{5,30,40,70,35,45,60,65})
	{
		a.insert(k);
		cout<<"after inserting "<<k<<":\n";
		visualise_234(a.root);
		validate(a);
	}

	// 2. Internal split + root split (the "insert 75" example)
	cout<<"=== then 80, 75: internal split, root splits, tree grows ===\n\n";
	a.insert(80);
	visualise_234(a.root);
	a.insert(75);
	visualise_234(a.root);
	validate(a);

	// 3. Sorted input
	cout<<"=== Sorted input 1..20 ===\n\n";
	Tree_234 b;
	for(int k=1;k<=20;k++) b.insert(k);
	visualise_234(b.root);
	validate(b);

	// 4. Duplicates and find
	b.insert(7); b.insert(7);
	vector<int> keys;
	collectKeys(b.root,keys);
	assert(keys.size()==20);
	assert(b.find(7)!=nullptr && b.find(100)==nullptr);
	cout<<"duplicates and find: OK\n";

	// 5. Random stress test against std::set
	mt19937 rng(42);
	for(int trial=0;trial<200;trial++)
	{
		Tree_234 t;
		set<int> ref;
		int n=rng()%2000;
		for(int i=0;i<n;i++)
		{
			int k=rng()%5000;
			t.insert(k);
			ref.insert(k);
			validate(t);
		}
		vector<int> got;
		collectKeys(t.root,got);
		assert(got==vector<int>(ref.begin(),ref.end()));
	}
	cout<<"random stress test (200 trees): OK\n";

	// 6. Deletion: the worked example (every repair case appears)
	cout<<"\n=== Deleting 45 5 40 65 35 30 ===\n\n";
	Tree_234 d;
	for(int k:{5,30,40,70,35,45,60,65}) d.insert(k);
	visualise_234(d.root);
	for(int k:{45,5,40,65,35,30})
	{
		d.remove(k);
		cout<<"after deleting "<<k<<":\n";
		visualise_234(d.root);
		validate(d);
	}

	// 7. Random inserts AND deletes against std::set
	for(int trial=0;trial<200;trial++)
	{
		Tree_234 t;
		set<int> ref;
		int n=rng()%3000;
		for(int i=0;i<n;i++)
		{
			int k=rng()%500;                  // small range: lots of hits
			if(rng()%2) { t.insert(k); ref.insert(k); }
			else        { t.remove(k); ref.erase(k); }
			validate(t);
			assert((t.find(k)!=nullptr)==(ref.count(k)>0));
		}
		vector<int> got;
		collectKeys(t.root,got);
		assert(got==vector<int>(ref.begin(),ref.end()));
		for(int k:vector<int>(ref.begin(),ref.end()))   // empty it completely
		{
			t.remove(k);
			validate(t);
		}
		assert(t.root==nullptr);
	}
	cout<<"random insert/delete stress test (200 trees): OK\n";

	// 8. Height: every node has >= 2 children, so h <= log2(n+1)
	Tree_234 big;
	int N=1000000;
	for(int k=0;k<N;k++) big.insert(k);
	int h=validate(big);
	cout<<"1,000,000 sorted inserts: height = "<<h
	    <<"  (bound floor(log2(n+1)) = "<<floor(log2(N+1.0))<<")\n";
	assert(h<=floor(log2(N+1.0)));

	cout<<"\nall tests passed\n";
}