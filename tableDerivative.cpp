#include "tableDerivative.h"
#include "DSL.h"
#include "differentiator.h"
#include "operations.h"
#include "tree.h"
#include "expressionTree.h"

#define DEBUG 

#include "general/debug.h"
#include "general/strFunc.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

treeNode_t* numDiff(){
    LPRINTF("Создаю ноду численную производной");
    return createNewNodeNumber(0, NULL, NULL);
}

treeNode_t* varDiff(){
    LPRINTF("Создаю ноду переменной производной");
    return createNewNodeNumber(1, NULL, NULL);
}

treeNode_t* operDiff(treeNode_t* node){
    assert(node);

    LPRINTF("Создаю ноду операции производной");
    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(node->data.op[0] == operations[curOper].nameString[0]){
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

