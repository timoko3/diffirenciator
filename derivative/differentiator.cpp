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

const size_t MAX_AMOUNT_TEXED_EXP = 110;

static treeNode_t* collapseConstant         (tree_t* derivative, treeNode_t* subTreeRoot, bool needDump);

static treeNode_t* removeNeutralElements    (tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t** addrToAsignNewSubTree, bool needDump);
static treeNode_t* removeNeutralSubtree     (tree_t* derivative, treeNode_t* subTreeRoot, treeNode_t* remainSubTreeRoot);
static treeNode_t* removeLeftNeutralSubtree (tree_t* derivative, treeNode_t* subTreeRoot);
static treeNode_t* removeRightNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot);

// static treeNode_t* replaceDivisionWithPow(treeNode_t* subTreeRoot);
static void replaceVariableWithNumber(treeNode_t* node, double valToReplace, const char* variableToDiff);

tree_t differentiate(tree_t* expression, const char* variableToDiff){
    assert(expression);

    LPRINTF("\n\n differentiation start\n");

    tree_t derivativeTree;

    treeCtor(&derivativeTree);

    derivativeTree.root = differentiateNode(expression->root, variableToDiff);
    derivativeTree.amountNodes = (size_t) countNodesTree(derivativeTree.root);

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

    static int optimizeCalls = 0;
    optimizeCalls++;
    LPRINTF("optimizeCalls = %d", optimizeCalls);

    LPRINTF("\n\nstart optimization");

    if(_L(subTreeRoot)){
        optimizeExpression(derivative, _L(subTreeRoot), needDump);
    }
    if(_R(subTreeRoot)){
        optimizeExpression(derivative, _R(subTreeRoot), needDump);
    }

    if(checkNotHaveVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        subTreeRoot = collapseConstant(derivative, subTreeRoot, needDump);

        if(subTreeRoot == derivative->root){
            return true;
        }
    }

    LPRINTF("subTreeRoot = %p", subTreeRoot);
    LPRINTF("subTreeRoot->parent = %p", _PAR(subTreeRoot));

    treeNode_t** addrToAsignNewSubTree = NULL;
    if(_PAR(subTreeRoot)){
        LPRINTF("addrToAsignNewSubTree init with %p", &derivative->root);
        if(_L(_PAR(subTreeRoot)) == subTreeRoot){
            addrToAsignNewSubTree = &_L(_PAR(subTreeRoot));
        }
        else if(_R(_PAR(subTreeRoot)) == subTreeRoot){
            addrToAsignNewSubTree = &_R(_PAR(subTreeRoot));
        }
    }
    else{
        LPRINTF("addrToAsignNewSubTree init with %p", &derivative->root);
        addrToAsignNewSubTree = &derivative->root;
    }
    assert(addrToAsignNewSubTree);

    subTreeRoot = removeNeutralElements(derivative, subTreeRoot, addrToAsignNewSubTree, needDump);

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
    replaceVariableWithNumber(derivativeInPoint.root, _DATA_NUM(tailorX0->root), variableToDiff);

    tree_t expressionInPoint;
    treeCtor(&expressionInPoint);
    expressionInPoint.root = _C(expression->root);
    replaceVariableWithNumber(expressionInPoint.root, _DATA_NUM(tailorX0->root), variableToDiff);
    
    tangentTree.root = _ADD(expressionInPoint.root, _MUL(derivativeInPoint.root, _SUB(_V("x"),  _N(_DATA_NUM(tailorX0->root)))));
    
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

    double x0val          = calculateSubTree(tailorX0->root);
    double tailorOrderVal = calculateSubTree(tailorOrder->root);

    for(size_t curTerm = 0; curTerm <= (size_t) tailorOrderVal; curTerm++){
        tree_t curTermDerivativeInPoint;
        treeCtor(&curTermDerivativeInPoint);
        curTermDerivativeInPoint.root = _C(curTermDerivative.root);
        assert(curTermDerivativeInPoint.root);

        // logTree(&curTermDerivativeInPoint, "tailor derivative copied to replace variable with num");

        replaceVariableWithNumber(curTermDerivativeInPoint.root, _DATA_NUM(tailorX0->root), variableToDiff);

        if(tailorTree.root){
            tailorTree.root = _ADD(tailorTree.root, _MUL(_DIV(curTermDerivativeInPoint.root, _N(factorial((double) curTerm))), _POW(_SUB(_V("x"), _N(x0val)), _N((double) curTerm))));
        }
        else{
            tailorTree.root = _MUL(_DIV(curTermDerivativeInPoint.root, _N(factorial((double) curTerm))), _POW(_SUB(_V("x"), _N(x0val)), _N((double) curTerm)));
        }
        
        // logTree(&tailorTree, "%lu  tailorTree", curTerm);
        tree_t tmp = curTermDerivative;
        curTermDerivative = differentiate(&curTermDerivative, variableToDiff);
        freeNode(tmp.root, false);

        optimizeExpression(&curTermDerivative, curTermDerivative.root);
        optimizeExpression(&tailorTree, tailorTree.root);
    }
    freeNode(curTermDerivative.root, false);
    
    setParent(tailorTree.root);

    tailorTree.amountNodes = (size_t) countNodesTree(tailorTree.root);

    return tailorTree;
}

static treeNode_t* collapseConstant(tree_t* derivative, treeNode_t* subTreeRoot, bool needDump){
    assert(subTreeRoot);

    static size_t collapseConstantCount = 1;
    // logTree(derivative, "collapseConst number %lu. CurSubTreeRoot = %p", collapseConstantCount, subTreeRoot);
    collapseConstantCount++;

    LPRINTF("start collapsing constant");

    treeNode_t* result = NULL;

    if(subTreeRoot->type == OPERATOR && isEqualStrings(_DATA_OP(subTreeRoot), "/")){
        return subTreeRoot;
    }

    double calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    if(!_L(subTreeRoot) && !_R(subTreeRoot)){
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

    if(_L(_PAR(subTreeRoot)) == subTreeRoot){
        LPRINTF("case subTreeRoot parent's left");
        _L(_PAR(subTreeRoot)) = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = _L(_PAR(subTreeRoot));
    }
    else{
        LPRINTF("case subTreeRoot parent's left");
        _R(_PAR(subTreeRoot)) = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = _R(_PAR(subTreeRoot));
    }
    LPRINTF("MEOW");
    setParent(_PAR(subTreeRoot));

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
        if(_DATA_OP(subTreeRoot)[0] == '*' && isEqualDouble(_DATA_NUM(_L(subTreeRoot)), 0)){
            LPRINTF("zero multiply case removal neutral");

            freeNode(subTreeRoot, true);

            assert(_PAR(subTreeRoot));

            treeNode_t* newNode = NULL;
            if(_L(_PAR(subTreeRoot)) == subTreeRoot){
                _L(_PAR(subTreeRoot)) = createNewNodeNumber(0, NULL, NULL);
                newNode = _L(_PAR(subTreeRoot));
            }
            else{
                _R(_PAR(subTreeRoot)) = createNewNodeNumber(0, NULL, NULL);
                newNode = _R(_PAR(subTreeRoot));
            }
            _PAR(newNode) = _PAR(subTreeRoot);

            freeExpressionNodeData(subTreeRoot, false, 1);
            free(subTreeRoot);

            LPRINTF("after removing zero multiply subTreeRoot = %p", newNode);

            static size_t removeNeutralCaseZeroMulCount = 1;
            // logTree(derivative, "removeNeutral case multiply zero number %lu. CurSubTreeRoot = %p", removeNeutralCaseZeroMulCount, newNode);
            removeNeutralCaseZeroMulCount++;

            // logTree(derivative, "during removal of neutral mul 0");

            if(needDump){
                texDumpTree(derivative);
            }

            return newNode;
        }

        LPRINTF("subTreeRoot->left = %p, subTreeRoot->right = %p", _L(subTreeRoot), _R(subTreeRoot));
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

    if(!_PAR(subTreeRoot)){
        return NULL;
    }

    if(subTreeRoot->type != NUMBER){
        return NULL;
    }

    // LPRINTF("subTreeRoot = %p, subTreeRoot->parent->data.op = %s", subTreeRoot, subTreeRoot->parent->data.num);
    // LPRINTF("subTreeRoot->parent = %p, subTreeRoot->parent->data.op = %s", subTreeRoot->parent->data.op);

    // LPRINTF("Проверка на то, есть ли умножение на 1 или плюсование 0. subTreeRoot->data.num = %d, subTreeRoot->parent->data.op[0] == '%c', subTreeRoot = %p, subTreeRoot->parent->right = %p", subTreeRoot->data.num, subTreeRoot->parent->data.op[0], subTreeRoot, subTreeRoot->parent->right);
    if((_DATA_OP(_PAR(subTreeRoot))[0] == '*' && isEqualDouble(_DATA_NUM(_PAR(subTreeRoot)), 1))  || 
        ((_DATA_OP(_PAR(subTreeRoot))[0] == '+' || _DATA_OP(_PAR(subTreeRoot))[0] == '-') && isEqualDouble(_DATA_NUM(subTreeRoot), 0)) ||
        (_DATA_OP(_PAR(subTreeRoot))[0] == '/' && isEqualDouble(_DATA_NUM(subTreeRoot), 1) && _R(_PAR(subTreeRoot)) == subTreeRoot)){
        LPRINTF("удаляем поддерево с корнем %p", subTreeRoot);

        if(_PAR(_PAR(subTreeRoot))){
            if(_L(_PAR(_PAR(subTreeRoot))) == _PAR(subTreeRoot)){
                _L(_PAR(_PAR(subTreeRoot))) = remainSubTreeRoot;
                _PAR(remainSubTreeRoot) = _PAR(_PAR(subTreeRoot));
            }
            else{
                _R(_PAR(_PAR(subTreeRoot))) = remainSubTreeRoot;
                _PAR(remainSubTreeRoot) = _PAR(_PAR(subTreeRoot));
            }
        }
        else{
            derivative->root = remainSubTreeRoot;
            _PAR(remainSubTreeRoot) = NULL;
        }

        return remainSubTreeRoot;
    }   
    
    return NULL;
}

static treeNode_t* removeLeftNeutralSubtree(tree_t* derivative, treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    if(_L(subTreeRoot) && _R(subTreeRoot) && subTreeRoot->type == OPERATOR){
        LPRINTF("subTreeRoot->left = %p, subTreeRoot->right = %p", _L(subTreeRoot), _R(subTreeRoot));
        if(removeNeutralSubtree(derivative, _L(subTreeRoot), _R(subTreeRoot))){
            treeNode_t* result = _R(subTreeRoot);
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

    if(_L(subTreeRoot) && _R(subTreeRoot) && subTreeRoot->type == OPERATOR){
        if(removeNeutralSubtree(derivative, _R(subTreeRoot), _L(subTreeRoot))){
            treeNode_t* result = _L(subTreeRoot);
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


double calculateSubTree(treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    if(subTreeRoot->type == OPERATOR){
        for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
            if(isEqualStrings(operations[curOper].nameString, _DATA_OP(subTreeRoot))){
                double* params = (double*) calloc(operations[curOper].paramCount, sizeof(double));
                assert(params);

                switch(operations[curOper].paramCount){
                    case 1: params[0] = calculateSubTree(_L(subTreeRoot)); break;
                    case 2: params[0] = calculateSubTree(_L(subTreeRoot)); params[1] = calculateSubTree(_R(subTreeRoot)); break;
                    default: break;
                }
                // LPRINTF("params[0] = %d, params[1] = %d", params[0], params[1]);
                
                double result = operations[curOper].calcHandler(params);
                LPRINTF("result = %d", result);

                free(params);
                return result;
            }
        }
    }
    else{
        return _DATA_NUM(subTreeRoot);
    }

    LPRINTF("calculation failure");
    return 0;
}

static void replaceVariableWithNumber(treeNode_t* node, double valToReplace, const char* variableToDiff){
    assert(node);
    assert(variableToDiff);

    if(node->type == VARIABLE && isEqualStrings(_DATA_VAR(node), variableToDiff)){
        free(_DATA_VAR(node));
        node->type = NUMBER;
        _DATA_NUM(node) = valToReplace;

    }

    if(_L(node)){
        replaceVariableWithNumber(_L(node), valToReplace, variableToDiff);
    }

    if(_R(node)){
        replaceVariableWithNumber(_R(node), valToReplace, variableToDiff);
    }
}