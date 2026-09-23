#include <bits/stdc++.h>
using namespace std;
struct node
{
	int key;
	node* parent;
	node* left;
	node* right;

	node(int key, node* parent=nullptr, node* left=nullptr, node* right=nullptr)
	{
		this->key=key;
		this->parent=parent;
		this->left=left;
		this->right=right;
	}
};

enum from_t{
	LEFT, RIGHT, PARENT
};

struct state{
	node* curr;
	from_t from=PARENT;
	void step_forward()
	{
		if(from==PARENT)
		{
			if(curr->left)
				curr=curr->left;
			else
				from=LEFT;
		}
		else if(from==LEFT)
		{
			if(curr->right)
				curr=curr->right, from=PARENT;
			else
				from=RIGHT;
		}
		else //from==RIGHT
		{
			if(curr->parent)
				from=(curr->parent->left==curr)?LEFT:RIGHT;
			curr=curr->parent;
		}
	}

};

struct tree{
	node* root=nullptr;
	void insert(int x) //inserts duplicates in right subtree
	{
		if(root==nullptr)
		{
			root=new node(x);
			return;
		}

		node* curr=root;
		node* prev=nullptr;
		while(curr)
		{
			if(x>=curr->key)
				prev=curr,curr=curr->right;
			else //x<curr->key
				prev=curr,curr=curr->left;
		}
		if(x>=prev->key)
			prev->right=new node(x,prev);
		else //x<curr->key
			prev->left=new node(x,prev);
	}
	
	node* find(int x) //returns first occurrence if duplicate
	{
		node* curr=root;
		while(curr)
		{
			if(curr->key==x)
				return curr;
			if(curr->key<x)
				curr=curr->right;
			else //curr->key>x
				curr=curr->left;
		}
		return nullptr;
	}

	bool remove(int x)
	{
		node* tmp=find(x);
		if(tmp)
		{
			erase(tmp);
			return true;
		}
		return false;
	}

	node* minimum(node* p)
	{
		while(p->left)
			p=p->left;
		return p;
	}

	node* maximum(node* p)
	{
		while(p->right)
			p=p->right;
		return p;
	}

	node* next_inorder(node* p) //without using state and enum
	{
		if(p->right)
			return minimum(p->right);
		else
		{
			while(p->parent && (p->parent->right==p))
				p=p->parent;
			return p->parent;
		}
	}

	node* prev_inorder(node* p)  //without using state and enum
	{
		if(p->left)
			return maximum(p->left);
		else
		{
			while(p->parent && (p->parent->left==p))
				p=p->parent;
			return p->parent;
		}
	}

	node* next(node* p)  //using state and enum
	{
		auto initial=p;
		for(state current={p,LEFT}; current.curr!=nullptr ;current.step_forward())
		{
			if(current.curr!=p && current.from==LEFT)
				return current.curr;
		}
		return nullptr;
	}

	node* &where(node* p)
	{
		if(p==root)
			return root;
		
		if(p->parent->left==p)
			return p->parent->left;
		else
			return p->parent->right;
	}

	node* detach(node* p) //detaches node from BST while maintaining the property
	{
		cout<<"Detaching " << p->key<<" whose left kid is "<<((p->left)?to_string(p->left->key):"null")<<" and right kid is "<<((p->right)?to_string(p->right->key):"null")<<endl;
		if((p->left==nullptr)&&(p->right==nullptr)) //no kids exist
			where(p)=nullptr;
		else if((p->left!=nullptr)&&(p->right!=nullptr)) //both kids exist
		{
			node* replacement=p->right;
			while(replacement->left)
				replacement=replacement->left;
			detach(replacement);
			
			replacement->parent=p->parent;
			replacement->left=p->left;
			replacement->right=p->right;
			where(p)=replacement;
			if(replacement->left) 
				replacement->left->parent = replacement;
            if(replacement->right) 
				replacement->right->parent = replacement;
		}
		else //exactly one of the kids exist
		{
			node* child=(p->left)?(p->left):(p->right);  //the one who exists
			where(p)=child; 
			child->parent=p->parent;
		}
		return p;
	}

	void erase(node* p)
	{
		if(p==nullptr)
			return;
		detach(p);
		delete p;
	}

	vector<int>inorder(node* p)
	{
		if(!p)
			return {};

		vector<int>ans;

		for(state current={p,PARENT}; current.curr!=nullptr ;current.step_forward())
		{
			if(current.from==LEFT)
				ans.push_back(current.curr->key);
		}

		return ans;
	}

	vector<int>preorder(node* p)
	{
		if(!p)
			return {};

		vector<int>ans;

		for(state current={p,PARENT}; current.curr!=nullptr ;current.step_forward())
		{
			if(current.from==PARENT)
				ans.push_back(current.curr->key);
		}

		return ans;
	}

	vector<int>postorder(node* p)
	{
		if(!p)
			return {};

		vector<int>ans;

		for(state current={p,PARENT}; current.curr!=nullptr ;current.step_forward())
		{
			if(current.from==RIGHT)
				ans.push_back(current.curr->key);
		}

		return ans;
	}

	vector<int>preorder()
	{
		return preorder(root);
	}

	vector<int>postorder()
	{
		return postorder(root);
	}

	vector<int> inorder()
	{
		return inorder(root);
	}
};

int main()
{
    tree t;

    // 1. Test Insertions
    cout << "Inserting elements: 50, 30, 20, 40, 70, 60, 80, 50 (duplicate)\n";
    vector<int> elements = {50, 30, 20, 40, 70, 60, 80, 50};
    for(int x:elements) {
        t.insert(x);
    }

    auto print_vec=[](const string &name, const vector<int> &v){
        cout<<name<<": ";
        for(int x:v) 
			cout<<x<<" ";
        cout<<"\n";
    };

    // 2. Test Traversals
    cout << "\n--- Testing Traversals ---\n";
    print_vec("Inorder  ", t.inorder(t.root));   // Should be sorted
    print_vec("Preorder ", t.preorder(t.root));
    print_vec("Postorder", t.postorder(t.root));

    // 3. Test Find, Minimum, Maximum
    cout << "\n--- Testing Find, Min, Max ---\n";
    node* root_node = t.find(50);
    if (root_node) {
        cout << "Found 50 in the tree.\n";
        cout << "Minimum in subtree of 50: " << t.minimum(root_node)->key << "\n";
        cout << "Maximum in subtree of 50: " << t.maximum(root_node)->key << "\n";
    }

    // 4. Test Predecessor and Successor (Inorder)
    cout << "\n--- Testing Predecessor and Successor ---\n";
    node* node_40 = t.find(40);
    if (node_40) {
        node* next = t.next_inorder(node_40);
        if (next) cout << "Next inorder after 40 is: " << next->key << "\n";
    }

    node* node_60 = t.find(60);
    if (node_60) {
        node* prev = t.prev_inorder(node_60);
        if (prev) cout << "Prev inorder before 60 is: " << prev->key << "\n";
    }

    // Test the state-based next() function
    node* state_next = t.next(node_40);
    if (state_next) cout << "State-based next() after 40 is: " << state_next->key << "\n";

    // 5. Test Removals
    cout << "\n--- Testing Removals ---\n";
    
    // Remove a leaf node
    cout << "Removing 20 (Leaf node)...\n";
    t.remove(20);
    print_vec("Inorder after removing 20", t.inorder(t.root));

    // Remove a node with one child
    cout << "\nRemoving 30 (Node with one child)...\n";
    t.remove(30);
    print_vec("Inorder after removing 30", t.inorder(t.root));

    // Remove a node with two children (the root)
    cout << "\nRemoving 70 (Node with two children)...\n";
    t.remove(70);
    print_vec("Inorder after removing 70", t.inorder(t.root));

    // Remove the root (which now has duplicate 50 as well)
    cout << "\nRemoving 50 (Root node)...\n";
    t.remove(50);
    print_vec("Inorder after removing 50", t.inorder(t.root));
    print_vec("Preorder after removing 50", t.preorder(t.root));
    return 0;
}