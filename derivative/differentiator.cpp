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

// #define DEBUG

#include "../general/poison.h"
#include "../general/debug.h"
#include "../general/file.h" 
#include "../general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

static treeNode_t* collapseConstant         (tree_t* derivative, treeNode_t* subTreeRoot, bool needDump);

static treeNode_t* removeNeutralElements    (tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t** addrToAsignNewSubTree, bool needDump);
static treeNode_t* removeNeutralSubtree     (tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t* remainSubTreeRoot);
static treeNode_t* removeLeftNeutralSubtree (tree_t* derivative, treeNode_t* subTreeRoot);
static treeNode_t* removeRightNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot);

// static treeNode_t* replaceDivisionWithPow(treeNode_t* subTreeRoot);
static void replaceVariableWithNumber(treeNode_t* node, int valToReplace, const char* variableToDiff);

tree_t differentiate(tree_t* expression, const char* variableToDiff){
    assert(expression);

    LPRINTF("\n\n differentiation start\n");

    tree_t derivativeTree;

    treeCtor(&derivativeTree);

    derivativeTree.root = differentiateNode(expression->root, variableToDiff);

    return derivativeTree;
}

treeNode_t* differentiateNode(treeNode_t* node, const char* variableToDiff){
    assert(node);
    LPRINTF("differentiateNode start");
    treeNode_t* createdNode = NULL;

    LPRINTF("node->type = %d", node->type);
    switch(node->type){
        case NUMBER:   createdNode = numDiff ();                       break;
        case VARIABLE: createdNode = varDiff (node, variableToDiff);   break;
        case OPERATOR: createdNode = operDiff(node, variableToDiff);   break;
        case NO_TYPE:  LPRINTF("node: %p has no type", node);          break;
        default: break;
    }

    return createdNode;
}

bool optimizeExpression(tree_t* derivative, treeNode_t* subTreeRoot, bool needDump){
    assert(derivative);
    assert(subTreeRoot);

    // if(needDump){
    //     logTree(derivative, "optimization");
    // }

    LPRINTF("\n\nstart optimization");

    if(subTreeRoot->left){
        optimizeExpression(derivative, subTreeRoot->left, needDump);
    }
    if(subTreeRoot->right){
        optimizeExpression(derivative, subTreeRoot->right, needDump);
    }

    if(checkNotHaveVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        subTreeRoot = collapseConstant(derivative, subTreeRoot, needDump);

        if(subTreeRoot == derivative->root){
            return true;
        }
    }

    LPRINTF("subTreeRoot = %p", subTreeRoot);
    LPRINTF("subTreeRoot->parent = %p", subTreeRoot->parent);

    treeNode_t** addrToAsignNewSubTree = NULL;
    if(subTreeRoot->parent){
        LPRINTF("addrToAsignNewSubTree init with %p", &derivative->root);
        if(subTreeRoot->parent->left == subTreeRoot){
            addrToAsignNewSubTree = &subTreeRoot->parent->left;
        }
        else if(subTreeRoot->parent->right == subTreeRoot){
            addrToAsignNewSubTree = &subTreeRoot->parent->right;
        }
    }
    else{
        LPRINTF("addrToAsignNewSubTree init with %p", &derivative->root);
        addrToAsignNewSubTree = &derivative->root;
    }
    assert(addrToAsignNewSubTree);

    subTreeRoot = removeNeutralElements(derivative, subTreeRoot, addrToAsignNewSubTree, needDump);

    // if(subTreeRoot->type == OPERATOR){
    //     LPRINTF("check case with pow subTreeRoot->left->data.var = %d");
    //     if(isEqualStrings(subTreeRoot->data.op, "/") && subTreeRoot->right->type == VARIABLE){
    //         LPRINTF("case replace with pow aproved");
    
    //         treeNode_t* curSubTreeRoot = subTreeRoot;
    //         treeNode_t* newNode = replaceDivisionWithPow(subTreeRoot);
    //         LPRINTF("newNode = %p", newNode);

    //         if(subTreeRoot->parent->left == subTreeRoot){
    //             subTreeRoot->parent->left = newNode;
    //         }
    //         else{
    //             subTreeRoot->parent->right = subTreeRoot->parent->left;
    //         }
    //         newNode->parent = subTreeRoot->parent;

    //         freeNode(curSubTreeRoot, false);

    //         subTreeRoot = newNode;

    //         LPRINTF("subTreeRoot.right.type = %d", subTreeRoot->right->type);
    //         LPRINTF("subTreeRoot.left.type = %d", subTreeRoot->left->type);

    //         LPRINTF("subTreeRoot = %p, data = %s, subTreeRoot.left = %p, subTreeRoot.right = %p, subTreeRoot.left.type = %d ,subTreeRoot.right.type = %d", subTreeRoot, subTreeRoot->data.op, subTreeRoot->left, subTreeRoot->right, subTreeRoot->left->type, subTreeRoot->right->type);
            

    //         LPRINTF("subTreeRoot.left.data.num = %d, subTreeRoot.right.data.op = %s", subTreeRoot->left->data.num, subTreeRoot->right->data.op);
            
    //         LPRINTF("subTreeRoot->parent->left = %p", subTreeRoot->parent->left);
            
    //         logTree(derivative, "replace div with pow");
    //     }
    // }

    return true;
}

tree_t tangentExpression(tree_t* expression, tree_t* derivative, tree_t* tailorX0, const char* variableToDiff){
    assert(expression);
    assert(derivative);
    assert(tailorX0);
    assert(variableToDiff);

    tree_t tangentTree;
    treeCtor(&tangentTree);

    tree_t derivativeInPoint;
    treeCtor(&derivativeInPoint);
    derivativeInPoint.root = _C(derivative->root);
    replaceVariableWithNumber(derivativeInPoint.root, tailorX0->root->data.num, variableToDiff);

    tree_t expressionInPoint;
    treeCtor(&expressionInPoint);
    expressionInPoint.root = _C(expression->root);
    replaceVariableWithNumber(expressionInPoint.root, tailorX0->root->data.num, variableToDiff);
    
    tangentTree.root = _ADD(expressionInPoint.root, _MUL(derivativeInPoint.root, _SUB(_V("x"),  _N(tailorX0->root->data.num))));
    
    // freeNode(derivativeInPoint.root, false);
    // freeNode(expressionInPoint.root, false);

    return tangentTree;
}

tree_t tailorExpansion(tree_t* expression, const char* variableToDiff, tree_t* tailorX0, tree_t* tailorOrder){
    assert(expression);
    assert(variableToDiff);
    assert(tailorX0);
    assert(tailorOrder);

    tree_t tailorTree;
    treeCtor(&tailorTree);
    
    tree_t curTermDerivative;
    treeCtor(&curTermDerivative);
    curTermDerivative.root = _C(expression->root);

    int x0val          = calculateSubTree(tailorX0->root);
    int tailorOrderVal = calculateSubTree(tailorOrder->root);

    for(size_t curTerm = 0; curTerm <= (size_t) tailorOrderVal; curTerm++){
        tree_t curTermDerivativeInPoint;
        treeCtor(&curTermDerivativeInPoint);
        curTermDerivativeInPoint.root = _C(curTermDerivative.root);
        assert(curTermDerivativeInPoint.root);

        // logTree(&curTermDerivativeInPoint, "tailor derivative copied to replace variable with num");

        replaceVariableWithNumber(curTermDerivativeInPoint.root, tailorX0->root->data.num, variableToDiff);

        if(tailorTree.root){
            tailorTree.root = _ADD(tailorTree.root, _MUL(_DIV(curTermDerivativeInPoint.root, _N(factorial((int) curTerm))), _POW(_SUB(_V("x"), _N(x0val)), _N((int) curTerm))));
        }
        else{
            tailorTree.root = _MUL(_DIV(curTermDerivativeInPoint.root, _N(factorial((int) curTerm))), _POW(_SUB(_V("x"), _N(x0val)), _N((int) curTerm)));
        }
        
        // logTree(&tailorTree, "%lu  tailorTree", curTerm);
        tree_t tmp = curTermDerivative;
        curTermDerivative = differentiate(&curTermDerivative, variableToDiff);
        freeNode(tmp.root, false);
    }
    freeNode(curTermDerivative.root, false);
    
    setParent(tailorTree.root);

    return tailorTree;
}

static treeNode_t* collapseConstant(tree_t* derivative, treeNode_t* subTreeRoot, bool needDump){
    assert(subTreeRoot);

    static size_t collapseConstantCount = 1;
    // logTree(derivative, "collapseConst number %lu. CurSubTreeRoot = %p", collapseConstantCount, subTreeRoot);
    collapseConstantCount++;

    LPRINTF("start collapsing constant");

    treeNode_t* result = NULL;

    if(subTreeRoot->type == OPERATOR && isEqualStrings(subTreeRoot->data.op, "/")){
        return subTreeRoot;
    }

    int calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    if(!subTreeRoot->left && !subTreeRoot->right){
        LPRINTF("collapse constant no need");

        return subTreeRoot; 
    }

    freeNode(subTreeRoot, true);
    
    LPRINTF("freed collapsingConst with no problems");


    LPRINTF("subTreeRoot = %p\n", subTreeRoot);
    if(subTreeRoot == derivative->root){
        LPRINTF("subTreeRoot == derivative->root");
        derivative->root = createNewNodeNumber(calculatedVal, NULL, NULL);
        
        freeExpressionNodeData(subTreeRoot, false, 1);
        free(subTreeRoot);

        return derivative->root;
    }

    if(subTreeRoot->parent->left == subTreeRoot){
        LPRINTF("case subTreeRoot parent's left");
        subTreeRoot->parent->left = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->left;
    }
    else{
        LPRINTF("case subTreeRoot parent's left");
        subTreeRoot->parent->right = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    LPRINTF("MEOW");
    setParent(subTreeRoot->parent);

    LPRINTF("setParent succeed in collapseConstant func");

    freeExpressionNodeData(subTreeRoot, false, 1);
    free(subTreeRoot);

    if(needDump){
        texDumpTree(derivative);
    }

    LPRINTF("ended collapsing constant and free root");
    return result;
}

static treeNode_t* removeNeutralElements(tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t** addrToAsignNewSubTree, bool needDump){
    assert(subTreeRoot);
    assert(derivative);
    assert(addrToAsignNewSubTree);

    static size_t removeNeutralCount = 1;
    // logTree(derivative, "removeNeutral number %lu. CurSubTreeRoot = %p", removeNeutralCount, subTreeRoot);
    removeNeutralCount++;

    LPRINTF("check if can removal neutral subTreeRoot = %p", subTreeRoot);
    if(subTreeRoot->type == OPERATOR){
        if(subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 0){
            LPRINTF("zero multiply case removal neutral");

            freeNode(subTreeRoot, true);

            assert(subTreeRoot->parent);

            treeNode_t* newNode = NULL;
            if(subTreeRoot->parent->left == subTreeRoot){
                subTreeRoot->parent->left = createNewNodeNumber(0, NULL, NULL);
                newNode = subTreeRoot->parent->left;
            }
            else{
                subTreeRoot->parent->right = createNewNodeNumber(0, NULL, NULL);
                newNode = subTreeRoot->parent->right;
            }
            newNode->parent = subTreeRoot->parent;

            freeExpressionNodeData(subTreeRoot, false, 1);
            free(subTreeRoot);

            LPRINTF("after removing zero multiply subTreeRoot = %p", newNode);

            static size_t removeNeutralCaseZeroMulCount = 1;
            // logTree(derivative, "removeNeutral case multiply zero number %lu. CurSubTreeRoot = %p", removeNeutralCaseZeroMulCount, newNode);
            removeNeutralCaseZeroMulCount++;

            // logTree(derivative, "during removal of neutral mul 0");
            // texDumpTree(derivative);

            return newNode;
        }

        LPRINTF("subTreeRoot->left = %p, subTreeRoot->right = %p", subTreeRoot->left, subTreeRoot->right);
        treeNode_t* newRootLeft = removeLeftNeutralSubtree(derivative, subTreeRoot);
        if(newRootLeft){
            LPRINTF("*addrToAsignNewSubTree = %p", *addrToAsignNewSubTree);
            *addrToAsignNewSubTree = newRootLeft;
            subTreeRoot = newRootLeft;

            if(needDump){
                texDumpTree(derivative);
            }
        }

        treeNode_t* newRootRight = removeRightNeutralSubtree(derivative, subTreeRoot);
        if(newRootRight){
            LPRINTF("*addrToAsignNewSubTree = %p", *addrToAsignNewSubTree);
            *addrToAsignNewSubTree = newRootRight;
            subTreeRoot = newRootRight;

            if(needDump){
                texDumpTree(derivative);
            }
        }
    }

    return subTreeRoot;
}

static treeNode_t* removeNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t* remainSubTreeRoot){
    assert(derivative);
    assert(subTreeRoot);
    assert(remainSubTreeRoot);

    if(!subTreeRoot->parent){
        return NULL;
    }

    if(subTreeRoot->type != NUMBER){
        return NULL;
    }

    // LPRINTF("subTreeRoot = %p, subTreeRoot->parent->data.op = %s", subTreeRoot, subTreeRoot->parent->data.num);
    // LPRINTF("subTreeRoot->parent = %p, subTreeRoot->parent->data.op = %s", subTreeRoot->parent->data.op);

    // LPRINTF("Проверка на то, есть ли умножение на 1 или плюсование 0. subTreeRoot->data.num = %d, subTreeRoot->parent->data.op[0] == '%c', subTreeRoot = %p, subTreeRoot->parent->right = %p", subTreeRoot->data.num, subTreeRoot->parent->data.op[0], subTreeRoot, subTreeRoot->parent->right);

    if((subTreeRoot->parent->data.op[0] == '*' && subTreeRoot->data.num == 1)  || 
        ((subTreeRoot->parent->data.op[0] == '+' || subTreeRoot->parent->data.op[0] == '-') && subTreeRoot->data.num == 0) ||
        (subTreeRoot->parent->data.op[0] == '/' && subTreeRoot->data.num == 1 && subTreeRoot->parent->right == subTreeRoot)){
        LPRINTF("удаляем поддерево с корнем %p", subTreeRoot);

        if(subTreeRoot->parent->parent){
            if(subTreeRoot->parent->parent->left == subTreeRoot->parent){
                subTreeRoot->parent->parent->left = remainSubTreeRoot;
                remainSubTreeRoot->parent = subTreeRoot->parent->parent;
            }
            else{
                subTreeRoot->parent->parent->right = remainSubTreeRoot;
                remainSubTreeRoot->parent = subTreeRoot->parent->parent;
            }
        }
        else{
            derivative->root = remainSubTreeRoot;
            remainSubTreeRoot->parent = NULL;
        }

        return remainSubTreeRoot;
    }   
    
    return NULL;
}

static treeNode_t* removeLeftNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    if(subTreeRoot->left && subTreeRoot->right && subTreeRoot->type == OPERATOR){
        LPRINTF("subTreeRoot->left = %p, subTreeRoot->right = %p", subTreeRoot->left, subTreeRoot->right);
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

    if(subTreeRoot->left && subTreeRoot->right && subTreeRoot->type == OPERATOR){
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

// static treeNode_t* replaceDivisionWithPow(treeNode_t* subTreeRoot){
//     assert(subTreeRoot);

//     LPRINTF("FIX -pow");

//     treeNode_t* newNode = _MUL(_C(subTreeRoot->left), _POW(_C(subTreeRoot->right), _N(-1)));

//     return newNode;
// }


int calculateSubTree(treeNode_t* subTreeRoot){
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
                // LPRINTF("params[0] = %d, params[1] = %d", params[0], params[1]);
                
                int result = operations[curOper].calcHandler(params);
                LPRINTF("result = %d", result);

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

static void replaceVariableWithNumber(treeNode_t* node, int valToReplace, const char* variableToDiff){
    assert(node);
    assert(variableToDiff);

    if(node->type == VARIABLE && isEqualStrings(node->data.var, variableToDiff)){
        free(node->data.var);
        node->type = NUMBER;
        node->data.num = valToReplace;

    }

    if(node->left){
        replaceVariableWithNumber(node->left, valToReplace, variableToDiff);
    }

    if(node->right){
        replaceVariableWithNumber(node->right, valToReplace, variableToDiff);
    }
}