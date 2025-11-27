#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "differentiator.h"
#include "tree.h"
// #include "texDump.h"

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

    log(&expression, "after reading");
    tree_t derivative = differentiate(&expression);
    log(&derivative, "after differentiation");

    optimizeDerivative(derivative.root);
    LPRINTF("Ended optimization");
    log(&derivative, "after optimization");

    // texDumpTree(&derivative);
    
    treeDtor(&expression);
    treeDtor(&derivative);
}