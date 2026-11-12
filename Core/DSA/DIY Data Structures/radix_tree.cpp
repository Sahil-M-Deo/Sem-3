#include <bits/stdc++.h>
#include "helper.h"
using namespace std;

struct node{
    vector<pair<node*,string>>children;
    bool is_end;

	node()
	{
		is_end=false;
	}

	void isEnd(bool is_end=true)
	{
		this->is_end=is_end;
	}

	void addChild(node* child, string edge_string)
	{
		children.push_back({child,edge_string});
	}

	void removeChild(node* child, string edge_string)
	{
		erase(children, make_pair(child, edge_string));
	}
};

struct Tree
{
	node* root;
	Tree()
	{
		root=new node();
	}

	private:

	int longest_common_prefix(const string &a, const string &b, int p1) //finds len of longest common prefix between a[p1...] and b
	{
		int len=min(a.size()-p1,b.size());
		for(int i=0;i<len;i++)
		{
			if(a[p1+i]!=b[i])
				return i;
		}
		return len;
	}


	public:
	void insert(const string& word) 
	{
	    if(search(word))
	        return;

	    int p1=0;
	    node* curr=root;
	    while(1)
	    {
			if(p1==word.size())
			{
				curr->isEnd();
				return;
			}
			node* next=nullptr;
	        for(auto &[x,y]:curr->children)
	        {
				int lcp=longest_common_prefix(word,y,p1);
	            if(y.size()==lcp)
	            {
	                p1+=y.size();
	                next=x;
	                break;
	            }
	            else if(word.size()-p1==lcp)
	            {
					node* niu=new node();
					niu->addChild(x,y.substr(word.size()-p1));
					niu->isEnd(true);
					x=niu;
					y=y.substr(0,word.size()-p1);
					return;
	            }
				else if(lcp!=0)
				{
					
					node* niuniu=new node(); //end node for word ending
					niuniu->isEnd(true);

					node* niu=new node(); //intermediate node
					niu->addChild(x,y.substr(lcp)); //og node
					niu->addChild(niuniu,word.substr(p1+lcp)); //actual new node

					x=niu;
					y=y.substr(0,lcp);
					return;
				}
	        }
			if(!next)
			{
				node* niu=new node();
				niu->isEnd(true);
				curr->addChild(niu,word.substr(p1));
				return;
			}
	        curr=next;
	    }
	}
	node* search(const string& word) 
	{
	    int p1=0;
	    node* curr=root;
	    while(curr)
	    {
			if(p1==word.size())
			{
				if(curr->is_end)
					return curr;
				else
					return nullptr;
			}

			node* next=nullptr;
	        for(auto &[x,y]:curr->children)
	        {
	            if(longest_common_prefix(word,y,p1)==y.size())
	            {
	                p1+=y.size();
	                next=x;
	                break;
	            }
	        }
			curr=next;
	    }
		return nullptr;
	}

	bool startsWith(const string& prefix) 
	{
	    int p1=0;
	    node* curr=root;
	    while(curr)
	    {
			if(p1==prefix.size())
	            return true;
			node* next=nullptr;
	        for(auto &[x,y]:curr->children)
	        {
				int lcp=longest_common_prefix(prefix,y,p1);
	            if(lcp==y.size())
	            {
	                p1+=y.size();
	                next=x;
	                break;
	            }
	            if(prefix.size()-p1==lcp)
	                return true;
	        }
			curr=next;
	    }
		return false;	    
	}

	bool remove(const string& word)
	{
		return _remove(root,word,0);
	}

	private:
	bool _remove(node* curr, const string& word, int p1) //returns true if word was found and removed
	{
		if(p1==word.size())
		{
			if(!curr->is_end)
				return false;
			curr->isEnd(false);
			return true;
		}

		for(int i=0;i<curr->children.size();i++)
		{
	        auto &[x,y]=curr->children[i];
	        if(longest_common_prefix(word,y,p1)==y.size())
	        {
	            if(!_remove(x,word,p1+y.size()))
	                return false;

	            if(!x->is_end)
	            {
	                if(x->children.size()==0)
	                {
	                    node* dead=x;
	                    curr->children.erase(curr->children.begin()+i);
	                    delete dead;
	                }
	                else if(x->children.size()==1)
	                {
	                    node* og=x;
	                    auto [gson_ptr,suffix]=x->children[0];
	                    x=gson_ptr;
	                    y=y+suffix;
	                    delete og;
	                }
	            }
	            return true;
	        }
		}
		return false;
	}

};

// --- Test harness ---
// Each scenario prints what it does, draws the tree when its shape changes,
// and marks every check with ✓ or ✗. A summary is printed at the end.

static int passed = 0, failed = 0;

static void section(const string& title)
{
    cout << "\n==================== " << title << " ====================\n";
}

static void show(Tree& t, const string& caption)
{
    cout << "\n  " << caption << "\n\n";
    visualise_radix_topdown(t.root);
}

static void check(bool got, bool expected, const string& what)
{
    bool ok = (got == expected);
    (ok ? passed : failed)++;
    cout << "  " << (ok ? "\033[32m✓\033[0m " : "\033[31m✗\033[0m ") << left << setw(40) << what << " "
         << boolalpha << got;
    if (!ok) cout << "   (expected " << expected << ")";
    cout << "\n";
}

static void insertAll(Tree& t, const vector<string>& words)
{
    cout << "  insert:";
    for (const string& w : words) { cout << " \"" << w << "\""; t.insert(w); }
    cout << "\n";
}

int main()
{
    // ------------------------------------------------------------------
    section("1. Build a tree");
    Tree t;
    insertAll(t, {"test", "team", "te", "toast", "testing"});
    show(t, "\"te\" is a split point, \"st\" -> \"ing\" extends test into testing:");

    // ------------------------------------------------------------------
    section("2. search (exact words only)");
    for (string w : {"test", "team", "te", "toast", "testing"})
        check(t.search(w), true, "search(\"" + w + "\")");
    check(t.search(string("tea")),   false, "search(\"tea\")      ends mid-edge");
    check(t.search(string("t")),     false, "search(\"t\")        node, not a word");
    check(t.search(string("tests")), false, "search(\"tests\")    runs past a leaf");
    check(t.search(string("xyz")),   false, "search(\"xyz\")      no such path");

    // ------------------------------------------------------------------
    section("3. startsWith (any word with this prefix)");
    check(t.startsWith(string("t")),        true,  "startsWith(\"t\")");
    check(t.startsWith(string("tea")),      true,  "startsWith(\"tea\")      mid-edge");
    check(t.startsWith(string("testi")),    true,  "startsWith(\"testi\")");
    check(t.startsWith(string("toa")),      true,  "startsWith(\"toa\")");
    check(t.startsWith(string("tx")),       false, "startsWith(\"tx\")");
    check(t.startsWith(string("testings")), false, "startsWith(\"testings\")");

    // ------------------------------------------------------------------
    section("4. remove \"test\": node with one child gets merged");
    string test = "test", testing = "testing";
    check(t.remove(test), true, "remove(\"test\")");
    show(t, "\"st\" and \"ing\" are now one edge \"sting\":");
    check(t.search(test),    false, "search(\"test\")");
    check(t.search(testing), true,  "search(\"testing\")   still there");
    check(t.remove(test),    false, "remove(\"test\")      again");

    // ------------------------------------------------------------------
    section("5. remove words that aren't stored");
    string nope = "nope", tes = "tes";
    check(t.remove(nope), false, "remove(\"nope\")      no such path");
    check(t.remove(tes),  false, "remove(\"tes\")       ends mid-edge");
    show(t, "Tree unchanged:");

    // ------------------------------------------------------------------
    section("6. remove \"te\": branching node just gets unmarked");
    string te = "te", team = "team";
    check(t.remove(te), true, "remove(\"te\")");
    show(t, "\"e\" node is now ○ but stays, since it still has two children:");
    check(t.search(te),             false, "search(\"te\")");
    check(t.search(team),           true,  "search(\"team\")");
    check(t.startsWith(string("te")), true, "startsWith(\"te\")    team, testing");

    // ------------------------------------------------------------------
    section("7. remove the only word: no dead path left behind");
    Tree u;
    string dog = "dog", d = "do";
    insertAll(u, {"dog", "dog"});
    show(u, "Inserting \"dog\" twice stores it once:");
    check(u.search(dog), true, "search(\"dog\")");
    check(u.remove(dog), true, "remove(\"dog\")");
    show(u, "After remove(\"dog\") the edge and leaf are gone:");
    check(u.search(dog),   false, "search(\"dog\")");
    check(u.remove(dog),   false, "remove(\"dog\")       again");
    check(u.startsWith(d), false, "startsWith(\"do\")");

    // ------------------------------------------------------------------
    section("8. remove a leaf: its parent gets merged upward");
    Tree v;
    string a = "test", b = "team", c = "te";
    insertAll(v, {"test", "team"});
    show(v, "\"te\" splits into \"am\" and \"st\":");
    check(v.remove(c), false, "remove(\"te\")        only a path");
    check(v.remove(a), true,  "remove(\"test\")");
    show(v, "\"st\" leaf deleted, then \"te\" + \"am\" merged into \"team\":");
    check(v.search(b), true, "search(\"team\")");
    check(v.remove(b), true, "remove(\"team\")");
    show(v, "Last word removed:");
    check(v.startsWith(c), false, "startsWith(\"te\")");

    // ------------------------------------------------------------------
    section("Summary");
    cout << "  " << passed << " passed, " << failed << " failed\n";
    return failed != 0;
}