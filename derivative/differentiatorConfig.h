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
    char* name;
    differentiatorConfigParams paramType;
    tree_t treeParam;
};

static configParam config[]{
    {"tailor_order" , TAILOR_ORDER,     },
    {"tailor_x0"    , TAILOR_X0,        },
    {"formula"      , FORMULA,          },
    {"x_scale"      , GRAPHIC_X_SCALE,  },
    {"y_scale"      , GRAPHIC_Y_SCALE,  }
};

void differentiatorReadConfigParam(string curString);
tree_t* findConfigParameter(differentiatorConfigParams type);
void configDtor();