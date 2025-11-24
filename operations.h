#include "tableDerivative.h"


typedef treeNode_t* (*handler_t)(treeNode_t* leftNode, treeNode_t* rightNode);

enum operationNames{
    ADD,
    MULTIPLY
};

struct operation_t{
    char*          symbol;
    operationNames name;
    size_t         derivativeSize;
    handler_t      handler;
    size_t         paramCount;
};

operation_t operations[]{
    {"+", ADD,      3, addDiff, 2},
    {"*", MULTIPLY, 7, mulDiff, 2}
};