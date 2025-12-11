#include "generalDifferentiator.h"
#include "strFunc.h"

#include <stddef.h>

enum differentiatorConfigParams{
    TAILOR_ORDER,
    TAILOR_X0,
    FORMULA,
    GRAPHIC_X_SCALE,
    GRAPHIC_Y_SCALE
};


struct configParam{
    const char* name;
    differentiatorConfigParams paramType;
    tree_t treeParam;
};

extern configParam config[];

void differentiatorReadConfigParam(string curString);
tree_t* findConfigParameter(differentiatorConfigParams type);
void configDtor();