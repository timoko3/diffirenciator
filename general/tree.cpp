#include "tree.h"
#include "expressionTree.h"
#include "differentiator.h"
#include "protectionDifferentiator.h"
#include "../derivative/differentiatorConfig.h"

#define DEBUG

#include "strFunc.h"
#include "debug.h"
#include "poison.h"
#include "file.h"

#include <assert.h>
#include <malloc.h>


treeNode_t* treeCtor(tree_t* expression){
    assert(expression);

    expression->amountNodes = 1;
    expression->root        = NULL;

    return expression->root;
}

treeNode_t* treeDtor(tree_t* expression){
    assert(expression);

    freeNode(expression->root, false);

    poisonMemory(&expression->amountNodes, sizeof(expression->amountNodes));

    return NULL;
}

void treeRead(const char* expressionFile){
    assert(expressionFile);

    data_t treeData;
    parseStringsFile(&treeData, expressionFile);

    LPRINTF("expression buffer: %s\n", treeData.buffer);

    for(size_t curStringInd = 0; curStringInd < treeData.nStrings; curStringInd++){
        LPRINTF("treeRead main cycle iteration: %lu", curStringInd + 1);
        differentiatorReadConfigParam(treeData.strings[curStringInd]);
    }
    
    free(treeData.buffer);
    free(treeData.strings);

}

treeNode_t* createNewNode(treeNode_t* left, treeNode_t* right){

    treeNode_t* newNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(newNode);
    LPRINTF("Выделил память");

    newNode->left  = left;
    newNode->right = right;

    return newNode;
}

treeNode_t* copyNode(treeNode_t* node){
    if(!node) return NULL;

    treeNode_t* copy = createNewNode(NULL, NULL);

    copyExpressionNode(copy, node);

    if(node->left){
        copy->left  = copyNode(node->left);
    }
    if(node->right){
        copy->right = copyNode(node->right); 
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
    if(!node){
        LPRINTF("передана нулева нода");
        return;
    }
    
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

    if(!freeExpressionNodeData(node, withoutRoot, depth)) return;

    LPRINTF("free: %p", node);
    poisonMemory(node, sizeof(*node));
    free(node);
    node = NULL;

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
