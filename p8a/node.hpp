#ifndef NODE_H
#define NODE_H

struct node { 
    char character; 
    struct node *left; 
    struct node *right; 
    //left is smaller value, right is larger value

    node(char val=0, node* left=nullptr, node* right=nullptr):character(val),left(left),right(right){}
    
}; 


#endif