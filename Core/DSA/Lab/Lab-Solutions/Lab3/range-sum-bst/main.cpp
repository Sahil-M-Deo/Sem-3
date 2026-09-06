#include "range-sum-bst.h"

Node* buildTree(const std::string& line){
    if(line.empty() || line == "null") return nullptr;
    
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> nodes;
    
    while(ss >> item){
        nodes.push_back(item);
    }
    
    if(nodes.empty() || nodes[0] == "null") return nullptr;
    Node *root = new Node(std::stoi(nodes[0]));
    std::queue<Node*> q;
    q.push(root);
    
    int i =1;
    while(!q.empty() && i<(int)nodes.size()){
        Node* curr = q.front();
        q.pop();
        if(i<(int)nodes.size() && nodes[i] != "null"){
            curr->left = new Node(std::stoi(nodes[i]));
            q.push(curr->left);
        }
        i++;
        if(i<(int)nodes.size()&&nodes[i]!="null"){
            curr->right=new Node(std::stoi(nodes[i]));
            q.push(curr->right);
        }
        i++;
    }
    return root;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    std::string treeInput;
    int low, high;
    
    if(!std::getline(std::cin, treeInput)){
        return 0;
    }
    
    if(!(std::cin >> low >> high)){
        return 0;
    }
    
    Node* root = buildTree(treeInput);
    Solution sol;
    int result = sol.rangeSumBST(root, low, high);
    std::cout << result <<std::endl;
    
    return 0;

}
