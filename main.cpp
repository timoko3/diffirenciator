#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "differentiator.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

int main(void){

    differentiator_t differentiator;
    differentiatorCtor(&differentiator);

    differentiatorReadData(&differentiator);

    log(&differentiator, "after reading");
    differentiator_t derivative = differentiate(&differentiator);
    log(&derivative, "after differentiation");


    differentiatorDtor(&differentiator);
    differentiatorDtor(&derivative);
}