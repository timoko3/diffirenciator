#include "tree.h"
#include "expressionTree.h"

#define DEBUG

#include "general/strFunc.h"
#include "general/debug.h"
#include "general/poison.h"

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

    copyExpressionNode(copy, toCopy);

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

void freeNode(treeNode_t* node, bool withoutRoot = false){
    assert(node);
    
    static int depth = 1;

    if(node->left){
        depth++;
        freeNode(node->left, withoutRoot);
        depth--;
    }
    
    if(node->right){
        depth++;
        freeNode(node->right, withoutRoot);
        depth--;
    }

    if(!freeExpressionNode(node, withoutRoot, depth)) return;

    LPRINTF("end freeing memory");
}

void freeLeftSubtree(treeNode_t* node, bool withoutRoot){
    assert(node);

    LPRINTF("start freeing leftSubtree %p", node);

    freeNode(node->left, withoutRoot);

    LPRINTF("end of freeLeftSubtree func");
}

void freeRightSubtree(treeNode_t* node, bool withoutRoot){
    assert(node);

    LPRINTF("start freeing rightSubtree %p", node);

    freeNode(node->right, withoutRoot);

    LPRINTF("end of freeRightSubtree func");
}
