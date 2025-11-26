#include "tableDerivative.h"
#include "DSL.h"
#include "differentiator.h"
#include "operations.h"
#include "tree.h"

#define DEBUG 

#include "general/debug.h"
#include "general/strFunc.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

static treeNode_t* copyNode(treeNode_t* toCopy);
static bool setParent(treeNode_t* curNode);

treeNode_t* numDiff(){
    LPRINTF("Создаю ноду численную производной");
    return createNewNodeNumber(0);
}

treeNode_t* varDiff(){
    LPRINTF("Создаю ноду переменной производной");
    return createNewNodeNumber(1);
}

treeNode_t* operDiff(treeNode_t* toDifferentiate){
    assert(toDifferentiate);

    LPRINTF("Создаю ноду операции производной");
    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(toDifferentiate->data.operatorVar[0] == operations[curOper].symbol[0]){
            LPRINTF("Нашел нужную операцию");
            return operations[curOper].diffHandler(_L, _R);
        }
    }
    LPRINTF("Не нашел нужную ноду");
    return NULL;
}

treeNode_t* addDiff(treeNode_t* left, treeNode_t* right){
    assert(right);
    assert(left);

    LPRINTF("Производная суммы");
    return _ADD(_DIF(left), _DIF(right));
}

treeNode_t* mulDiff(treeNode_t* left, treeNode_t* right){
    assert(right);
    assert(left);

    LPRINTF("Производная произведения");
    return _ADD(_MUL(_DIF(left), _C(right)), _MUL(_C(left), _DIF(right)));
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
        copy->data.operatorVar = myStrDup(toCopy->data.operatorVar);
    }

    if(toCopy->left){
        copy->left  = copyNode(toCopy->left);
    }
    if(toCopy->right){
        copy->right = copyNode(toCopy->right); 
    }
    
    setParent(copy);

    return copy;
}


static bool setParent(treeNode_t* curNode){
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