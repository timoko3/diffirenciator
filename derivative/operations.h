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
    COS,
    LN,
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
    bool           texNeedBrackets;
};

static operation_t operations[]{
    {"+",   ADD,      3,  addH, addDiff, 2, 2, "\\plus" , false, false},
    {"-",   SUB,      3,  subH, subDiff, 2, 2, "\\minus", false, false},
    {"*",   MULTIPLY, 7,  mulH, mulDiff, 2, 1, "\\mul"  , false, true},
    {"/",   DIVIDE,   11, divH, divDiff, 2, 1, "\\frac" , true , true},
    {"sin", SIN,      1,  sinH, sinDiff, 1, 1, "\\sin"  , true , true},
    {"cos", COS,      1,  cosH, cosDiff, 1, 1, "\\cos"  , true , true},
    {"ln",  LN,       3,  lnH,  lnDiff,  1, 1, "\\ln"   , true , true}
};

operation_t* getCurrentOperation(char* curOpStringName);