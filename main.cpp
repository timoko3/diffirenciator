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
    treeGraphDump(&differentiator);
    differentiatorDtor(&differentiator);
}