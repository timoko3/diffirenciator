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

treeNode_t* varDiff(treeNode_t* node, const char* variableToDiff){
    LPRINTF("Создаю ноду переменной производной");

    if(isEqualStrings(node->data.var, variableToDiff)) return _N(1);
    else return _C(node);
}

treeNode_t* operDiff(treeNode_t* node, const char* variableToDiff){
    assert(node);
    assert(variableToDiff);

    LPRINTF("Создаю ноду операции производной");
    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(isEqualStrings(node->data.op, operations[curOper].nameString)){
            LPRINTF("Нашел нужную операцию");

            treeNode_t** params = (treeNode_t**) calloc(operations[curOper].paramCount, sizeof(treeNode_t*));
            switch(operations[curOper].paramCount){
                case 1: params[0] = _L; break;
                case 2: params[0] = _L; params[1] = _R; break;
                default: break;
            }

            treeNode_t* result = operations[curOper].diffHandler(params, variableToDiff);

            free(params);
            return result;
        }
    }
    LPRINTF("Не нашел нужную ноду");
    return NULL;
}

treeNode_t* addDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная суммы");
    return _ADD(_DIF(params[0], variableToDiff), _DIF(params[1], variableToDiff));
}

treeNode_t* subDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная суммы");
    return _SUB(_DIF(params[0], variableToDiff), _DIF(params[1], variableToDiff));
}

treeNode_t* mulDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная произведения");
    return _ADD(_MUL(_DIF(params[0], variableToDiff), _C(params[1])), _MUL(_C(params[0]), _DIF(params[1], variableToDiff)));
}

treeNode_t* divDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная частного");
    return _DIV(_SUB(_MUL(_DIF(params[0], variableToDiff), _C(params[1])), _MUL(_C(params[0]), _DIF(params[1], variableToDiff))), _MUL(_C(params[1]), _C(params[1])));
}

// treeNode_t* powDiff(treeNode_t** params, const char* variableToDiff){
//     assert(params);
    
//     LPRINTF("Производная степени");

//     return _MUL(_POW(_C(params[0]), _C(params[1])), _ADD(_MUL(_DIF(params[1], variableToDiff), _LN(_C(params[0]), NULL)), _MUL(_DIV(_DIF(params[0], variableToDiff), _C(params[0])), _C(params[1]))));
// }

treeNode_t* powDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("nodeAddr.left = %p, nodeAddr.right = %p", params[0], params[1]);
    if(!checkNotHaveVariables(params[1])){
        LPRINTF("HAVE VARIABLES POW");
        return expFuncDiff(params, variableToDiff);
    }
    LPRINTF("NO VARIABLES POW");

    return _MUL(_DIF(params[0], variableToDiff), _MUL(_C(params[1]), _POW(_C(params[0]), _SUB(_C(params[1]), _N(1)))));
}

treeNode_t* expFuncDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    return _MUL(_POW(_C(params[0]), _C(params[1])), _ADD(_MUL(_DIF(params[1], variableToDiff), _LN(_C(params[0]), NULL)), _MUL(_DIV(_DIF(params[0], variableToDiff), _C(params[0])), _C(params[1]))));
}

treeNode_t* sinDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная синуса");
    return _MUL(_COS(_C(params[0]), NULL), _DIF(params[0], variableToDiff));
}

treeNode_t* cosDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная косинуса");
    return _MUL(_MUL(_SIN(_C(params[0]), NULL), _N(-1)), _DIF(params[0], variableToDiff));
}

treeNode_t* tanDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная тангенса");
    return _DIV(_DIF(params[0], variableToDiff), _MUL(_COS(_C(params[0]), NULL), _COS(_C(params[0]), NULL)));
}

treeNode_t* cotDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная котангенса");
    return _DIV(_MUL(_DIF(params[0], variableToDiff), _N(-1)), _MUL(_SIN(_C(params[0]), NULL), _SIN(_C(params[0]), NULL)));
}

treeNode_t* lnDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_DIV(_N(1), _C(params[0])), _DIF(params[0], variableToDiff));
}

treeNode_t* shDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_CH(_C(params[0]), NULL), _DIF(params[0], variableToDiff));
}

treeNode_t* chDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_SH(_C(params[0]), NULL), _DIF(params[0], variableToDiff));
}

treeNode_t* arcsinDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_DIV(_N(1), _SQRT(_SUB(_N(1), _POW(_C(params[0]), _N(2))), NULL)), _DIF(params[0], variableToDiff));
}

treeNode_t* arccosDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_MUL(_DIV(_N(1), _SQRT(_SUB(_N(1), _POW(_C(params[0]), _N(2))), NULL)), _DIF(params[0], variableToDiff)), _N(-1));
}

treeNode_t* arctanDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_DIV(_N(1), _ADD(_N(1), _POW(_C(params[0]), _N(2)))), _DIF(params[0], variableToDiff));
}

treeNode_t* sqrtDiff(treeNode_t** params, const char* variableToDiff){
    assert(params);

    LPRINTF("Производная натурального логарифма");
    return _MUL(_DIV(_N(1), _MUL(_N(2), _SQRT(_C(params[0]), NULL))), _DIF(params[0], variableToDiff));
}
