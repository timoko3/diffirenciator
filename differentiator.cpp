#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"
#include "tableDerivative.h"
#include "operations.h"
#include "mathHandlers.h"
#include "expressionTree.h"
#include "tree.h"

// # tree.cpp
// NodeData ReadData (const char* data);

// ...
// ...

// # treeFunc.cpp // rename
// NodeData ReadData (const char* data);
// NodeData ReadData (const char* data) {

// }

#define DEBUG

#include "general/poison.h"
#include "general/debug.h"
#include "general/file.h" 
#include "general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

static bool checkHaveVariables(treeNode_t* curNode);
static treeNode_t* collapseConstant(treeNode_t* subTreeRoot);
static int calculateSubTree(treeNode_t* subTreeRoot);
static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot);

tree_t differentiate(tree_t* expression){
    assert(expression);

    LPRINTF("\n\n differentiation start\n");

    tree_t derivativeTree;

    treeCtor(&derivativeTree);

    derivativeTree.root = differentiateNode(expression->root);

    return derivativeTree;
}



treeNode_t* differentiateNode(treeNode_t* node){
    assert(node);
    LPRINTF("differentiateNode start");
    treeNode_t* createdNode = NULL;

    LPRINTF("node->type = %d", node->type);
    switch(node->type){
        case NUMBER:   createdNode = numDiff();      break;
        case VARIABLE: createdNode = varDiff();      break;
        case OPERATOR: createdNode = operDiff(node); break;
        default: break;
    }

    return createdNode;
}

// (5+3) - (7+1)

//       -
//   +       +
// 5   3   7   1

//       -
//   8       +
//         7   1

//       -
//   8       8
  
//       0
  

bool optimizeDerivative(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    LPRINTF("\n\nstart optimization");
    if(checkHaveVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        subTreeRoot = collapseConstant(subTreeRoot);
    }
    subTreeRoot = removeNeutralElements(subTreeRoot);

    if(subTreeRoot->left){
        optimizeDerivative(subTreeRoot->left);
    }
    if(subTreeRoot->right){
        optimizeDerivative(subTreeRoot->right);
    }

    return true;
}

static bool checkHaveVariables(treeNode_t* curNode){ // have has
    assert(curNode);

    if(curNode->type == VARIABLE){
        return false;
    }

    if(curNode->left){
        if(!checkHaveVariables(curNode->left)) return false;
    }
    if(curNode->right){
        if(!checkHaveVariables(curNode->right)) return false;
    }

    return true;
}

static treeNode_t* collapseConstant(treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    treeNode_t* result = NULL;

    int calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    freeNode(subTreeRoot, true);
    
    LPRINTF("freeed collapsingConst with no problems");

    if(subTreeRoot->parent->left == subTreeRoot){
        subTreeRoot->parent->left = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    else{
        subTreeRoot->parent->right = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    setParent(subTreeRoot->parent);

    freeExpressionNodeData(subTreeRoot, false, 1);
    free(subTreeRoot);

    return result;
}

static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    LPRINTF("start removal neutral");
    if(subTreeRoot->type == OPERATOR){
        if(subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 0){
            LPRINTF("zero division case");

            freeNode(subTreeRoot, true);
            subTreeRoot = createNewNodeNumber(0, NULL, NULL);
            return subTreeRoot;
        }
        // copypaste
        if((subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 1) || 
            (subTreeRoot->data.op[0] == '+' && subTreeRoot->left->data.num == 0)){
                LPRINTF("keep only right subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->right;
                    subTreeRoot->right->parent = subTreeRoot->parent;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->right;
                }

                treeNode_t* result = subTreeRoot->right;
                LPRINTF("during optimization freeing leftSubtree");
                freeLeftSubtree(subTreeRoot, false);
                freeExpressionNodeData(subTreeRoot, false, 1);
                free(subTreeRoot);

                return result;
        }
        if((subTreeRoot->data.op[0] == '*' && subTreeRoot->right->data.num == 1) || 
            (subTreeRoot->data.op[0] == '+' && subTreeRoot->right->data.num == 0)){
                LPRINTF("keep only left subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->left;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->left;
                }
                treeNode_t* result = subTreeRoot->left;
                
                LPRINTF("during optimization freeing rightSubtree");
                freeRightSubtree(subTreeRoot, false);
                freeExpressionNodeData(subTreeRoot, false, 1);
                free(subTreeRoot);

                return result;
        }
    }

    return subTreeRoot;
}

static int calculateSubTree(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    if(subTreeRoot->type == OPERATOR){
        for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
            if(operations[curOper].nameString[0] == subTreeRoot->data.op[0]){
                return operations[curOper].calcHandler(calculateSubTree(subTreeRoot->left), calculateSubTree(subTreeRoot->right));
            }
        }
    }
    else{
        return subTreeRoot->data.num;
    }
}