#include "mathHandlers.h"

#include <math.h>
#include <assert.h>
#include <stdio.h>

int addH(int* params, size_t amountParam){
    assert(params);

    return params[0] + params[1];
}

int subH(int* params, size_t amountParam){
    assert(params);

    return params[0] - params[1];
}

int mulH(int* params, size_t amountParam){
    assert(params);

    return params[0] * params[1];
} 

int divH(int* params, size_t amountParam){
    assert(params);

    if(params[1] == 0){
        printf("Деление на 0 невозможно!\n");
    }

    return params[0] / params[1];
} 

int sinH(int* params, size_t amountParam){
    assert(params);

    return sin(params[0]);
} 

int cosH(int* params, size_t amountParam){
    assert(params);

    return cos(params[0]);
} 