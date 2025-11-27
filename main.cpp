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

// #include "superTree.h"

// void ReadData() {
//     ...
// }

int main(void){
    expression_t expression;
    differentiatorCtor(&expression);

    differentiatorReadData(&expression);

    log(&expression, "after reading");
    expression_t derivative = differentiate(&expression);
    log(&derivative, "after differentiation");

    optimizeDerivative(derivative.root);
    LPRINTF("Ended optimization");
    log(&derivative, "after optimization");

    // texDumpTree(&derivative);
    
    differentiatorDtor(&expression);
    differentiatorDtor(&derivative);
}