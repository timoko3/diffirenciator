#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"
#include "tableDerivative.h"
#include "operations.h"
#include "mathHandlers.h"
#include "expressionTree.h"
#include "tree.h"
#include "../texDump.h"

#define DEBUG

#include "../general/poison.h"
#include "../general/debug.h"
#include "../general/file.h" 
#include "../general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

const size_t PRECISION_TERM_TAILOR = 3;

static bool        checkHaveVariables(treeNode_t* curNode);
static treeNode_t* collapseConstant(tree_t* derivative, treeNode_t* subTreeRoot);
static int         calculateSubTree(treeNode_t* subTreeRoot);
static treeNode_t* removeNeutralElements    (tree_t* derivative, treeNode_t* subTreeRoot);
static treeNode_t* removeNeutralSubtree     (tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t* remainSubTreeRoot);
static treeNode_t* removeLeftNeutralSubtree (tree_t* derivative, treeNode_t* subTreeRoot);
static treeNode_t* removeRightNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot);

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
        case NUMBER:   createdNode = numDiff();                break;
        case VARIABLE: createdNode = varDiff();                break;
        case OPERATOR: createdNode = operDiff(node);           break;
        case NO_TYPE:  LPRINTF("node: %p has no type", node);  break;
        default: break;
    }

    return createdNode;
}

bool optimizeDerivative(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(derivative);
    assert(subTreeRoot);

    LPRINTF("\n\nstart optimization");

    if(subTreeRoot->left){
        optimizeDerivative(derivative, subTreeRoot->left);
    }
    if(subTreeRoot->right){
        optimizeDerivative(derivative, subTreeRoot->right);
    }

    if(checkHaveVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        subTreeRoot = collapseConstant(derivative, subTreeRoot);

        if(subTreeRoot == derivative->root){
            return true;
        }
    }
    subTreeRoot = removeNeutralElements(derivative, subTreeRoot);

    return true;
}

tree_t tailorExpansion(tree_t* expression){
    assert(expression);

    tree_t tailorTree;

    treeCtor(&tailorTree);
    
    tree_t curTermDerivative;
    treeCtor(&curTermDerivative);
    curTermDerivative.root = _C(expression->root);

    for(size_t curTerm = 0; curTerm < PRECISION_TERM_TAILOR; curTerm++){
        
        logTree(&curTermDerivative, "%lu производная tailor", curTerm);
                
        tailorTree.root = _ADD(tailorTree.root, _MUL(_DIV(curTermDerivative.root, _N(factorial((int) curTerm))), _POW(_V("x"), _N((int) curTerm))));
        
        curTermDerivative = differentiate(&curTermDerivative);

    }
    freeNode(curTermDerivative.root,     false);

    return tailorTree;
}

static bool checkHaveVariables(treeNode_t* curNode){
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

static treeNode_t* collapseConstant(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    static size_t collapseConstantCount = 1;
    // logTree(derivative, "collapseConst number %lu. CurSubTreeRoot = %p", collapseConstantCount, subTreeRoot);
    collapseConstantCount++;

    LPRINTF("start collapsing constant");

    treeNode_t* result = NULL;

    int calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    freeNode(subTreeRoot, true);
    
    LPRINTF("freed collapsingConst with no problems");

    if(subTreeRoot == derivative->root){
        LPRINTF("subTreeRoot == derivative->root");
        derivative->root = createNewNodeNumber(calculatedVal, NULL, NULL);
        
        freeExpressionNodeData(subTreeRoot, false, 1);
        free(subTreeRoot);

        return derivative->root;
    }

    if(subTreeRoot->parent->left == subTreeRoot){
        subTreeRoot->parent->left = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    else{
        subTreeRoot->parent->right = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    setParent(subTreeRoot->parent);

    LPRINTF("setParent succeed in collapseConstant func");

    freeExpressionNodeData(subTreeRoot, false, 1);
    free(subTreeRoot);

    texDumpTree(derivative);

    LPRINTF("ended collapsing constant and free root");
    return result;
}

static treeNode_t* removeNeutralElements(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    assert(derivative);

    static size_t removeNeutralCount = 1;
    // logTree(derivative, "removeNeutral number %lu. CurSubTreeRoot = %p", removeNeutralCount, subTreeRoot);
    removeNeutralCount++;

    LPRINTF("start removal neutral");
    if(subTreeRoot->type == OPERATOR){
        if(subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 0){
            LPRINTF("zero multiply case");

            freeNode(subTreeRoot, true);

            treeNode_t* newNode = NULL;
            if(subTreeRoot->parent->left == subTreeRoot){
                subTreeRoot->parent->left = createNewNodeNumber(0, NULL, NULL);
                newNode = subTreeRoot->parent->left;
            }
            else{
                subTreeRoot->parent->right = createNewNodeNumber(0, NULL, NULL);
                newNode = subTreeRoot->parent->right;
            }

            freeExpressionNodeData(subTreeRoot, false, 1);
            free(subTreeRoot);

            LPRINTF("after removing zero multiply subTreeRoot = %p", newNode);

            static size_t removeNeutralCaseZeroMulCount = 1;
            // logTree(derivative, "removeNeutral case multiply zero number %lu. CurSubTreeRoot = %p", removeNeutralCaseZeroMulCount, newNode);
            removeNeutralCaseZeroMulCount++;

            texDumpTree(derivative);

            return newNode;
        }
        removeLeftNeutralSubtree (derivative, subTreeRoot);
        removeRightNeutralSubtree(derivative, subTreeRoot);
    }

    texDumpTree(derivative);

    return subTreeRoot;
}

static treeNode_t* removeNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t* remainSubTreeRoot){
    assert(derivative);
    assert(subTreeRoot);
    assert(remainSubTreeRoot);

    if(subTreeRoot->type == OPERATOR){
        if((subTreeRoot->data.op[0] == '*' && subTreeRoot->data.num == 1) || 
            (subTreeRoot->data.op[0] == '+' && subTreeRoot->data.num == 0)){
            LPRINTF("keep only right subTree of %p", subTreeRoot);
            // logTree(derivative, "keep only right subTree of %p", subTreeRoot);

            if(subTreeRoot->parent->parent->left == subTreeRoot->parent){
                subTreeRoot->parent->parent->left = remainSubTreeRoot;
                remainSubTreeRoot->parent = subTreeRoot->parent->parent;
            }
            else{
                subTreeRoot->parent->parent->right = remainSubTreeRoot;
            }

            return remainSubTreeRoot;
        }   
    }
    
    return NULL;
}

static treeNode_t* removeLeftNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    if(subTreeRoot->left && subTreeRoot->right){
        if(removeNeutralSubtree(derivative, subTreeRoot->left, subTreeRoot->right)){
            treeNode_t* result = subTreeRoot->right;
            LPRINTF("during optimization freeing leftSubtree");
            // logTree(derivative, "during optimization freeing leftSubtree of %p", subTreeRoot);

            freeLeftSubtree(subTreeRoot, false);
            freeExpressionNodeData(subTreeRoot, false, 1);
            free(subTreeRoot);

            return result;
        }
    }
    return NULL;
}

static treeNode_t* removeRightNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    if(subTreeRoot->left && subTreeRoot->right){
        if(removeNeutralSubtree(derivative, subTreeRoot->right, subTreeRoot->left)){
            treeNode_t* result = subTreeRoot->left;
            LPRINTF("during optimization freeing RightSubtree of %p", subTreeRoot);
            // logTree(derivative, "during optimization freeing RightSubtree of %p", subTreeRoot);

            freeRightSubtree(subTreeRoot, false);
            freeExpressionNodeData(subTreeRoot, false, 1);
            free(subTreeRoot);

            return result;
        }
    }

    return NULL;
}


static int calculateSubTree(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    if(subTreeRoot->type == OPERATOR){
        for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
            if(isEqualStrings(operations[curOper].nameString, subTreeRoot->data.op)){
                int* params = (int*) calloc(operations[curOper].paramCount, sizeof(int));
                assert(params);

                switch(operations[curOper].paramCount){
                    case 1: params[0] = calculateSubTree(subTreeRoot->left); break;
                    case 2: params[0] = calculateSubTree(subTreeRoot->left); params[1] = calculateSubTree(subTreeRoot->right); break;
                    default: break;
                }

                int result = operations[curOper].calcHandler(params);

                free(params);
                return result;
            }
        }
    }
    else{
        return subTreeRoot->data.num;
    }

    LPRINTF("calculation failure");
    return 0;
}