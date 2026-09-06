#include "range-sum-bst.h"
/*
 *struct Node{
 *    int val;
 *    Node *left;
 *    Node *right;
 *  
 *    Node() : val(0), left(nullptr), right(nullptr) {}
 *    Node(int x) : val(x), left(nullptr), right(nullptr) {}
 *    Node(int x, Node *left, Node *right): val(x), left(left), right(right) {}
 *  
 *};
 */
int Solution::rangeSumBST(Node* root, int low, int high){
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if(!root) return 0;
    
    if(root->val < low){
        return rangeSumBST(root->right, low, high);
    }else if(root->val > high){
        return rangeSumBST(root->left, low, high);
    }
    return root->val + rangeSumBST(root->left, low, high) + rangeSumBST(root->right, low, high);
       
    
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
