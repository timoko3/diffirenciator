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

static treeNode_t* readCreateNode(tree_t* expression, char* buffer, size_t* cuBufferPose);
static treeNode_t* readAllocateNodeIfNeed(tree_t* expression, size_t countReadNodes);
static bool isNilNode(char* buffer, size_t* curBufferPos);
static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos);
static bool isSupportedOperation(char readSym);

static bool checkHaveVariables(treeNode_t* curNode);
static treeNode_t* collapseConstant(treeNode_t* subTreeRoot);
static int calculateSubTree(treeNode_t* subTreeRoot);
static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot);

tree_t differentiate(tree_t* expression){
    assert(expression);

    LPRINTF("\n\n differentiation start\n");

    tree_t derivativeTree;

    treeCtor(&derivativeTree);  //
    free(derivativeTree.root); //

    derivativeTree.root = differentiateNode(expression->root);

    return derivativeTree;
}

// rename
static treeNode_t* readCreateNode(tree_t* expression, char* buffer, size_t* curBufferPos){
    assert(expression);
    assert(buffer);
    
    static size_t countReadNodes = 1;
    treeNode_t* curNode = readAllocateNodeIfNeed(expression, countReadNodes);
    countReadNodes++;

    (*curBufferPos)++;
    
    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPos]);
    
    size_t lenName = 0;

    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    sscanf(&buffer[*curBufferPos], "\"%[^\"]\"%n", curNodeData, &lenName);
    
    LPRINTF("Получил размер строки %lu и саму строку %s", lenName, curNodeData);

    if(isdigit(curNodeData[0])){
        curNode->type = NUMBER;
        sscanf(curNodeData, "%d", &curNode->data.num);
    }
    else if(isSupportedOperation(curNodeData[0])){
        LPRINTF("%p - addr curNode", curNode);
        curNode->type = OPERATOR;
        curNode->data.op = myStrDup(curNodeData);
        LPRINTF("curNode->data.op = %s", curNode->data.op);
    }
    else{
        curNode->type = VARIABLE;
        curNode->data.var = myStrDup(curNodeData); 
    }
    *curBufferPos += lenName;
    LPRINTF("\nбуфер после чтением: %s", &buffer[*curBufferPos]);

    free(curNodeData);
    return curNode;
}

static bool isSupportedOperation(char readSym){
    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(readSym == operations[curOper].symbol[0]){
            LPRINTF("Операция доступна");
            return true;
        }
    }

    return false;
}

// rename
static treeNode_t* readAllocateNodeIfNeed(tree_t* expression, size_t countReadNodes){
    assert(expression);

    treeNode_t* curNode;

    if(countReadNodes == 1){
        curNode = expression->root;
        (expression->amountNodes)--;
        LPRINTF("Получил addr: %p", curNode);
    }
    else{
        curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
        assert(curNode);
        LPRINTF("Выделил память");
    }

    return curNode;
}

static bool isNilNode(char* buffer, size_t* curBufferPos){
    assert(buffer);
    assert(curBufferPos);

    // char curNodeData[MAX_ANSWER_SIZE] = {};
    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPos]);
    size_t lenName = 0;
    sscanf(&buffer[*curBufferPos], "%s%n", curNodeData, &lenName);
    LPRINTF("Прочиталось внутри случая nil: %s", curNodeData);
    if(isEqualStrings(curNodeData, "nil")){
        LPRINTF("Зашел в nil");
        *curBufferPos += lenName;
        LPRINTF("буфер после чтения: %s\n", &buffer[*curBufferPos]);
        free(curNodeData);
        return true;
    }

    free(curNodeData);

    return false;
}

static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos){
    assert(buffer);
    assert(curBufferPos);

    while(true){
        if((buffer[*curBufferPos] == ')') || (buffer[*curBufferPos] == ' ')){
            (*curBufferPos)++;
        }
        else{
            break;
        }
    }
}   

// open(file)
// openFile(toOpen)

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
            if(operations[curOper].symbol[0] == subTreeRoot->data.op[0]){
                return operations[curOper].calcHandler(calculateSubTree(subTreeRoot->left), calculateSubTree(subTreeRoot->right));
            }
        }
    }
    else{
        return subTreeRoot->data.num;
    }
}