#include "tree.h"

#define DEBUG

#include "general/strFunc.h"
#include "general/debug.h"

#include <assert.h>
#include <malloc.h>

treeNode_t* createNewNode(treeNode_t* left, treeNode_t* right){

    treeNode_t* newNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(newNode);
    LPRINTF("Выделил память");

    newNode->left  = left;
    newNode->right = right;

    return newNode;
}

treeNode_t* copyNode(treeNode_t* toCopy){
    assert(toCopy);

    treeNode_t* copy = createNewNode(NULL, NULL);

    copy->type = toCopy->type;
    if(toCopy->type == NUMBER){
        copy->data.num = toCopy->data.num;
    }
    else{
        copy->data.operatorVar = myStrDup(toCopy->data.operatorVar);
    }

    if(toCopy->left){
        copy->left  = copyNode(toCopy->left);
    }
    if(toCopy->right){
        copy->right = copyNode(toCopy->right); 
    }
    
    setParent(copy);

    return copy;
}


bool setParent(treeNode_t* curNode){
    assert(curNode);
    
    if(curNode->left){
        curNode->left->parent = curNode;
    }
    if(curNode->right){
        curNode->right->parent = curNode;
    }

    if(curNode->left){
        setParent(curNode->left);
    }
    
    if(curNode->right){
        setParent(curNode->right);
    }

    return true;
}

treeNode_t* createNewNodeNumber(int value, treeNode_t* left, treeNode_t* right){
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = NUMBER;

    newNode->data.num = value;

    return newNode;
}

treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = VARIABLE;
    newNode->data.operatorVar = myStrDup(name);
    
    setParent(newNode);
    
    return newNode;
}

treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = OPERATOR;
    newNode->data.operatorVar = myStrDup(name);

    setParent(newNode);

    return newNode;
}

