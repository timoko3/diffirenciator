#include "tableDerivative.h"
#include "mathHandlers.h"


typedef treeNode_t* (*diffHandler_t)(treeNode_t* leftNode, treeNode_t* rightNode);
typedef int (*calcHandler_t)(int leftNode, int rightNode);

enum operationNames{
    ADD,
    MULTIPLY
};

struct operation_t{
    char*          nameString; 
    operationNames type;
    size_t         derivativeSize; // no used
    calcHandler_t  calcHandler;
    diffHandler_t  diffHandler; 
    size_t         paramCount;
    int            priorityRank;
};

static operation_t operations[]{
    {"+", ADD,      3, add, addDiff, 2, 2},
    {"*", MULTIPLY, 7, mul, mulDiff, 2, 1}
};