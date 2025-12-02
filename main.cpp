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

    const char* expressionFIleName = NULL;
    if(argc == 2){
        expressionFIleName = argv[1];
    }
    else{
        expressionFIleName = DIFFERENTIATOR_DATA_FILE_NAME;
    }

    tree_t expression;
    treeCtor(&expression);
    
    treeRead(&expression, expressionFIleName);
    assert(expression.root);

    texDumpTree(&expression);

    logTree(&expression, "after reading");
    tree_t derivative = differentiate(&expression);
    logTree(&derivative, "after differentiation");

    logTree(&expression, "after differentiation expresion");

    texDumpTree(&derivative);

    optimizeExpression(&derivative, derivative.root);
    LPRINTF("Ended optimization");
    logTree(&derivative, "after optimization");

    // texDumpTree(&derivative);
    
    tree_t tailor = tailorExpansion(&expression);
    logTree(&tailor, "tailor tree dump");
    
    startTexDumpTailor();
    texDumpTree(&tailor, NULL, true);

    optimizeExpression(&tailor, tailor.root);

    texDumpTree(&tailor, NULL, true);
    endTexFile(&derivative);

    LPRINTF("конец");

    treeDtor(&tailor);
    treeDtor(&derivative);
    treeDtor(&expression);
}