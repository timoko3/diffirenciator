#include "tableDerivative.h"
#include "mathHandlers.h"

typedef treeNode_t* (*diffHandler_t)(treeNode_t** params);
typedef int (*calcHandler_t)(int* params);

enum operationNames{
    ADD,
    SUB,
    MULTIPLY,
    DIVIDE,
    SIN,
    COS
};

struct operation_t{
    const char*    nameString; 
    operationNames type;
    size_t         derivativeSize; // no used
    calcHandler_t  calcHandler;
    diffHandler_t  diffHandler; 
    size_t         paramCount;
    int            priorityRank;
    const char*    texCode;
    bool           texExists;
};

static operation_t operations[]{
    {"+",   ADD,      3,  addH, addDiff, 2, 2, "\\plus" , false},
    {"-",   SUB,      3,  subH, subDiff, 2, 2, "\\minus", false},
    {"*",   MULTIPLY, 7,  mulH, mulDiff, 2, 1, "\\mul"  , false},
    {"/",   DIVIDE,   11, divH, divDiff, 2, 1, "\\frac" , true},
    {"sin", SIN,      1,  sinH, sinDiff, 1, 1, "\\sin"  , true},
    {"cos", COS,      1,  cosH, cosDiff, 1, 1, "\\cos"  , true}
};

operation_t* getCurrentOperation(char* curOpStringName);