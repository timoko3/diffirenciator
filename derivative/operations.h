#include "tableDerivative.h"
#include "mathHandlers.h"

typedef treeNode_t* (*diffHandler_t)(treeNode_t** params);
typedef int (*calcHandler_t)(int* params);

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
    size_t         derivativeSize;
    calcHandler_t  calcHandler;
    diffHandler_t  diffHandler; 
    size_t         paramCount;
    int            priorityRank;
    const char*    texCode;
    bool           texExists;
    bool           texNeedBrackets;
};

static operation_t operations[]{
    {"+",       ADD,      3,  addH,     addDiff,     2, 2, "\\plus"    , false , false },
    {"-",       SUB,      3,  subH,     subDiff,     2, 2, "\\minus"   , false , false },
    {"*",       MULTIPLY, 7,  mulH,     mulDiff,     2, 1, "\\mul"     , false , true  },
    {"/",       DIVIDE,   11, divH,     divDiff,     2, 1, "\\frac"    , true  , false },
    {"^",       POW,      24, powH,     powDiff,     2, 0, "\\pow"     , false , false },
    {"sin",     SIN,      1,  sinH,     sinDiff,     1, 1, "\\sin"     , true  , true  },
    {"cos",     COS,      1,  cosH,     cosDiff,     1, 1, "\\cos"     , true  , true  },
    {"tan",     TAN,      5,  tanH,     tanDiff,     1, 1, "\\tan"     , true  , true  },
    {"cot",     TAN,      7,  cotH,     cotDiff,     1, 1, "\\cot"     , true  , true  },
    {"ln",      LN,       3,  lnH,      lnDiff,      1, 1, "\\ln"      , true  , true  },
    {"sinh",    SH,       5,  shH,      shDiff,      1, 1, "\\sinh"    , true  , true  },
    {"cosh",    CH,       5,  chH,      chDiff,      1, 1, "\\cosh"    , true  , true  },
    {"arcsin",  ARCSIN,   23, arcsinH,  arcsinDiff,  1, 1, "\\arcsin"  , true  , true  },
    {"arccos",  ARCCOS,   23, arccosH,  arccosDiff,  1, 1, "\\arccos"  , true  , true  },
    {"arctan",  ARCTAN,   23, arctanH,  arctanDiff,  1, 1, "\\arctan"  , true  , true  },
    {"sqrt",    SQRT,     7,  sqrtH,    sqrtDiff,    1, 1, "\\sqrt"    , true  , true  }
};

const size_t PRECISION_TERM_TAILOR = 2;

operation_t* getCurrentOperation(char* curOpStringName);