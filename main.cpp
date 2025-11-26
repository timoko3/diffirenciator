#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "differentiator.h"
#include "texDump.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

// #include "superTree.h"

// void ReadData() {
//     ...
// }

int main(void){

    differentiator_t differentiator;
    differentiatorCtor(&differentiator);

    differentiatorReadData(&differentiator);

    log(&differentiator, "after reading");
    differentiator_t derivative = differentiate(&differentiator);
    log(&derivative, "after differentiation");

    optimizeDerivative(derivative.root);
    log(&derivative, "after optimization");

    texDumpTree(&derivative);

    differentiatorDtor(&differentiator);
    differentiatorDtor(&derivative);
}