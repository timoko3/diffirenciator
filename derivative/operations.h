#include "tableDerivative.h"
#include "mathHandlers.h"

typedef treeNode_t* (*diffHandler_t)(treeNode_t** params, const char* variableToDiff);
typedef double (*calcHandler_t)(double* params);

enum operationNames{
    ADD,
    SUB,
    MULTIPLY,
    DIVIDE,
    POW,
    SIN,
    COS,
    TAN,
    COT,
    LN,
    SH,
    CH,
    ARCSIN,
    ARCCOS,
    ARCTAN,
    SQRT
};

struct operation_t{
    const char*    nameString; 
    operationNames type;
    calcHandler_t  calcHandler;
    diffHandler_t  diffHandler; 
    size_t         paramCount;
    int            priorityRank;
    const char*    texCode;
    const char*    pyCode;
    bool           texExists;
    bool           texNeedBrackets;
};

static operation_t operations[]{
    {"+",       ADD,      addH,     addDiff,     2, 2, "\\plus"  , "+"         , false , false},
    {"-",       SUB,      subH,     subDiff,     2, 2, "\\minus" , "-"         , false , false},
    {"*",       MULTIPLY, mulH,     mulDiff,     2, 1, "\\mul"   , "*"         , false , false},
    {"/",       DIVIDE,   divH,     divDiff,     2, 1, "\\frac"  , "/"         , true  , false},
    {"^",       POW,      powH,     powDiff,     2, 0, "\\pow"   , "**"        , false , false},
    {"sin",     SIN,      sinH,     sinDiff,     1, 1, "\\sin"   , "np.sin"    , true  , true },
    {"cos",     COS,      cosH,     cosDiff,     1, 1, "\\cos"   , "np.cos"    , true  , true },
    {"tan",     TAN,      tanH,     tanDiff,     1, 1, "\\tan"   , "np.tan"    , true  , true },
    {"cot",     TAN,      cotH,     cotDiff,     1, 1, "\\cot"   , "1 / np.tan", true  , true },
    {"ln",      LN,       lnH,      lnDiff,      1, 1, "\\ln"    , "np.log"    , true  , true },
    {"sinh",    SH,       shH,      shDiff,      1, 1, "\\sinh"  , "np.sinh"   , true  , true },
    {"cosh",    CH,       chH,      chDiff,      1, 1, "\\cosh"  , "np.cosh"   , true  , true },
    {"arcsin",  ARCSIN,   arcsinH,  arcsinDiff,  1, 1, "\\arcsin", "np.asin"   , true  , true },
    {"arccos",  ARCCOS,   arccosH,  arccosDiff,  1, 1, "\\arccos", "np.acos"   , true  , true },
    {"arctan",  ARCTAN,   arctanH,  arctanDiff,  1, 1, "\\arctan", "np.atan"   , true  , true },
    {"sqrt",    SQRT,     sqrtH,    sqrtDiff,    1, 1, "\\sqrt"  , "np.sqrt"   , true  , true }
};

operation_t* getCurrentOperation(char* curOpStringName);