#include "tableDerivative.h"
#include "mathHandlers.h"


typedef treeNode_t* (*diffHandler_t)(treeNode_t** params, size_t amountParam);
typedef int (*calcHandler_t)(int* params, size_t amountParam);

enum operationNames{
    ADD,
    SUB,
    MULTIPLY,
    DIVIDE,
    SIN,
    COS
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
    {"+",   ADD,      3,  addH, addDiff, 2, 2},
    {"-",   SUB,      3,  subH, subDiff, 2, 2},
    {"*",   MULTIPLY, 7,  mulH, mulDiff, 2, 1},
    {"/",   DIVIDE,   11, divH, divDiff, 2, 1},
    {"sin", SIN,      1,  sinH, sinDiff, 1, 1},
    {"cos", COS,      1,  cosH, cosDiff, 1, 1}
};