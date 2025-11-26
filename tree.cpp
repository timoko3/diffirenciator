#include "tree.h"

#define DEBUG

#include "general/strFunc.h"
#include "general/debug.h"

#include <assert.h>
#include <malloc.h>

treeNode_t* createNewNode(){

    treeNode_t* curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(curNode);
    LPRINTF("Выделил память");

    return curNode;
}

treeNode_t* createNewNodeNumber(int value){

    treeNode_t* newNode = createNewNode();

    newNode->nodeType = NUMBER;

    newNode->data.num = value;

    return newNode;
}

treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode();

    newNode->nodeType = VARIABLE;

    newNode->data.operatorVar = myStrDup(name);

    newNode->left         = left;
    newNode->left->parent = newNode;

    newNode->right = right;
    newNode->right->parent = newNode;

    return newNode;
}

treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode();

    newNode->nodeType = OPERATOR;
    newNode->data.operatorVar = myStrDup(name);

    newNode->left         = left;
    newNode->left->parent = newNode;

    newNode->right = right;
    newNode->right->parent = newNode;



    return newNode;
}
