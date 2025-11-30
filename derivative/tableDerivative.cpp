#include "tableDerivative.h"
#include "DSL.h"
#include "differentiator.h"
#include "operations.h"
#include "expressionTree.h"

#define DEBUG 

#include "../general/tree.h"
#include "../general/debug.h"
#include "../general/strFunc.h"

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

            treeNode_t** params = (treeNode_t**) calloc(operations[curOper].paramCount, sizeof(treeNode_t*));
            switch(operations[curOper].paramCount){
                case 1: params[0] = _L; break;
                case 2: params[0] = _L; params[1] = _R; break;
                default: break;
            }

            treeNode_t* result = operations[curOper].diffHandler(params);

            free(params);
            return result;
        }
    }
    LPRINTF("Не нашел нужную ноду");
    return NULL;
}

treeNode_t* addDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная суммы");
    return _ADD(_DIF(params[0]), _DIF(params[1]));
}

treeNode_t* subDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная суммы");
    return _SUB(_DIF(params[0]), _DIF(params[1]));
}

treeNode_t* mulDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная произведения");
    return _ADD(_MUL(_DIF(params[0]), _C(params[1])), _MUL(_C(params[0]), _DIF(params[1])));
}

treeNode_t* divDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная частного");
    return _DIV(_SUB(_MUL(_DIF(params[0]), _C(params[1])), _MUL(_C(params[0]), _DIF(params[1]))), _MUL(_C(params[1]), _C(params[1])));
}

treeNode_t* powDiff(treeNode_t** params){
    assert(params);
    
    LPRINTF("Производная степени");

    return _MUL(_POW(_C(params[0]), _C(params[1])), _ADD(_MUL(_DIF(params[1]), _LN(_C(params[0]), NULL)), _MUL(_DIV(_DIF(params[0]), _C(params[0])), _C(params[1]))));
}

// _ADD(_MUL(_DIF(params[1]), _LN(params[0])), _MUL(_DIV(_DIF(params[0]), _C(params[1])), _C(params[1])))

treeNode_t* sinDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная синуса");
    return _MUL(_COS(_C(params[0]), NULL), _DIF(params[0]));
}

treeNode_t* cosDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная косинуса");
    return _MUL(_MUL(_SIN(_C(params[0]), NULL), _N(-1)), _DIF(params[0]));
}

treeNode_t* lnDiff(treeNode_t** params){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_DIV(_N(1), _C(params[0])), _DIF(params[0]));
}
