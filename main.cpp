#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "differentiator.h"
#include "tree.h"
#include "texDump.h"

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



    tree_t expression;
    treeCtor(&expression);
    
    tree_t scaleGraphic = treeRead(&expression, expressionFIleName);
    
    assert(expression.root);
    texDumpTree(&expression);

    
    logTree(&expression, "after reading");
    tree_t derivative = differentiate(&expression, variableToDiff);
    logTree(&derivative, "after differentiation");

    logTree(&expression, "after differentiation expresion");

    texDumpTree(&derivative);

    optimizeExpression(&derivative, derivative.root);
    LPRINTF("Ended optimization");
    // logTree(&derivative, "after optimization");
    
    tree_t tailor = tailorExpansion(&expression, variableToDiff);
    // logTree(&tailor, "tailor tree dump");
    
    startTexDumpTailor();
    texDumpTree(&tailor, NULL, true);

    optimizeExpression(&tailor, tailor.root);

    texDumpTree(&tailor, NULL, true);
    endTexFile(&derivative, &scaleGraphic);

    LPRINTF("конец");

    treeDtor(&tailor);
    treeDtor(&derivative);
    treeDtor(&expression);
    treeDtor(&scaleGraphic);
}