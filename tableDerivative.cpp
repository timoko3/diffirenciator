#include "tableDerivative.h"
#include "DSL.h"
#include "differentiator.h"
#include "operations.h"

#include "general/debug.h"
#include "general/strFunc.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

static treeNode_t* createNewNodeNumber(int value);
static treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right);
static treeNode_t* copyNode(treeNode_t* toCopy);

treeNode_t* numDiff(){
    return createNewNodeNumber(0);
}

treeNode_t* varDiff(){
    return createNewNodeNumber(1);
}

treeNode_t* operDiff(treeNode_t* toDifferentiate){
    assert(toDifferentiate);

    for(size_t curOper = 0; curOper < sizeof(operations) - sizeof(operation_t); curOper++){
        if(toDifferentiate->data.operatorVar == operations[curOper].symbol){
            return operations[curOper].handler(_L, _R);
        }
    }

    return NULL;
}

treeNode_t* addDiff(treeNode_t* left, treeNode_t* right){
    assert(right);
    assert(left);

    return _ADD(_DIF(left), _DIF(right));
}

treeNode_t* mulDiff(treeNode_t* left, treeNode_t* right){
    assert(right);
    assert(left);

    return _ADD(_MUL(_DIF(left), _C(right)), _MUL(_C(left), _DIF(right)));
}

static treeNode_t* createNewNodeNumber(int value){
    treeNode_t* curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(curNode);
    LPRINTF("Выделил память");

    curNode->nodeType  = NUMBER;
    curNode->data.num  = value;

    return curNode;
}

static treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(newNode);
    LPRINTF("Выделил память");

    newNode->nodeType = OPERATOR;
    newNode->left  = left;
    newNode->right = right;

    newNode->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(newNode->data.operatorVar);

    strcpy(newNode->data.operatorVar, name);

    return newNode;
}

static treeNode_t* copyNode(treeNode_t* toCopy){
    assert(toCopy);

    treeNode_t* copy = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(copy);

    copy->nodeType = toCopy->nodeType;
    if(toCopy->nodeType == NUMBER){
        copy->data.num = toCopy->data.num;
    }
    else{
        copy->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
        assert(copy->data.operatorVar);

        strcpy(copy->data.operatorVar, toCopy->data.operatorVar);
    }

    if(toCopy->left){
        copy->left  = copyNode(toCopy->left);
    }
    if(toCopy->right){
        copy->right = copyNode(toCopy->right);
    }

    return copy;
}
