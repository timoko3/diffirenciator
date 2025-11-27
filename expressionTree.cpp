#include "expressionTree.h"
#include "tree.h"

#define DEBUG

#include "general/strFunc.h"
#include "general/debug.h"
#include "general/poison.h"

#include <assert.h>
#include <malloc.h>

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
    newNode->data.var = myStrDup(name);
    
    setParent(newNode);
    
    return newNode;
}

treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = OPERATOR;
    newNode->data.op = myStrDup(name);

    setParent(newNode);

    return newNode;
}

void copyExpressionNode(treeNode_t* copy, treeNode_t* toCopy){
    assert(copy);
    assert(toCopy);

    copy->type = toCopy->type;
    if(toCopy->type == NUMBER){
        copy->data.num = toCopy->data.num;
    }
    else if(toCopy->type == VARIABLE){
        copy->data.var = myStrDup(toCopy->data.var);
    }
    else if(toCopy->type == OPERATOR){
        copy->data.op = myStrDup(toCopy->data.op);
    }
}

bool freeExpressionNode(treeNode_t* node, bool withoutRoot, int depth){
    assert(node);
    
    LPRINTF("entered freeExpressionNode func with node %p, withoutRoot = %d, depth = %d", node, withoutRoot, depth);

    if(!withoutRoot || (withoutRoot && depth != 1)){
        if(node->type == VARIABLE){
            free(node->data.var);
        }
        if(node->type == OPERATOR){
            free(node->data.op);
        }
        LPRINTF("free: %p", node);
        poisonMemory(node, sizeof(*node));
        free(node);
        node = NULL;
        return true;
    }

    return false;
}