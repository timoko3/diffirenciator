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

int main(void){
    tree_t expression;
    treeCtor(&expression);
    
    treeRead(&expression);
    assert(expression.root);

    logTree(&expression, "after reading");
    tree_t derivative = differentiate(&expression);
    logTree(&derivative, "after differentiation");

    optimizeDerivative(&derivative, derivative.root);
    LPRINTF("Ended optimization");
    logTree(&derivative, "after optimization");

    texDumpTree(&derivative);
    
    treeDtor(&expression);
    treeDtor(&derivative);
}