#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "differentiator.h"
#include "tree.h"
#include "texDump.h"
#include "differentiatorConfig.h"

#define DEBUG

#include "general/debug.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

const char* DIFFERENTIATOR_DATA_FILE_NAME = "realExpression.txt"; 

int main(int argc, char* argv[]){

    const char* expressionFIleName = "";

    const char* variableToDiff = "";
    if(argc == 3){
        expressionFIleName = argv[1];
        variableToDiff     = argv[2];
    }
    else if(argc == 2){
        expressionFIleName = argv[1];
    }
    else{
        expressionFIleName = DIFFERENTIATOR_DATA_FILE_NAME;
    }
    
    treeRead(expressionFIleName);

    LPRINTF("exit reading function");

    tree_t* tailorOrder = findConfigParameter(TAILOR_ORDER);
    LPRINTF("tailorOrder.type = %d", tailorOrder->root->type);
    logTree(tailorOrder, "tailor order tree");

    tree_t* tailorX0 = findConfigParameter(TAILOR_X0);
    LPRINTF("tailorOrder.type = %d", tailorOrder->root->type);
    logTree(tailorX0, "tailorX0 tree");

    tree_t* formula = findConfigParameter(FORMULA);
    LPRINTF("tailorOrder.type = %d", tailorOrder->root->type);
    logTree(formula, "formula tree");

    tree_t* graphicXScale = findConfigParameter(GRAPHIC_X_SCALE);
    LPRINTF("tailorOrder.type = %d", tailorOrder->root->type);
    logTree(graphicXScale, "graphicXScale tree");

    tree_t* graphicYScale = findConfigParameter(GRAPHIC_Y_SCALE);
    LPRINTF("tailorOrder.type = %d", tailorOrder->root->type);
    logTree(graphicYScale, "graphicYScale tree");

    tree_t derivative = differentiate(formula, variableToDiff);
    logTree(&derivative, "after differentiation");

    texDumpTree(formula);

    optimizeExpression(&derivative, derivative.root);
    LPRINTF("Ended optimization");
    logTree(&derivative, "after optimization");
    
    tree_t tailor = tailorExpansion(formula, variableToDiff);
    
    startTexDumpTailor();
    texDumpTree(&tailor, NULL, true);

    optimizeExpression(&tailor, tailor.root);

    texDumpTree(&tailor, NULL, true);
    endTexFile(formula, &derivative, graphicXScale);

    configDtor();
    
    LPRINTF("конец");
}