#include "tableDerivative.h"
#include "mathHandlers.h"


typedef treeNode_t* (*diffHandler_t)(treeNode_t* leftNode, treeNode_t* rightNode);
typedef int (*calcHandler_t)(int leftNode, int rightNode);

enum operationNames{
    ADD,
    MULTIPLY
};

struct operation_t{
    char*          symbol;
    operationNames name;
    size_t         derivativeSize;
    diffHandler_t  diffHandler;
    size_t         paramCount;
    calcHandler_t  calcHandler;
};

static operation_t operations[]{
    {"+", ADD,      3, addDiff, 2, add},
    {"*", MULTIPLY, 7, mulDiff, 2, mul}
};