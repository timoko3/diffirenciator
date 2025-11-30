#include "mathHandlers.h"

#include <math.h>
#include <assert.h>
#include <stdio.h>

int addH(int* params){
    assert(params);

    return params[0] + params[1];
}

int subH(int* params){
    assert(params);

    return params[0] - params[1];
}

int mulH(int* params){
    assert(params);

    return params[0] * params[1];
} 

int divH(int* params){
    assert(params);

    if(params[1] == 0){
        printf("Деление на 0 невозможно!\n");
    }

    return params[0] / params[1];
} 

int powH(int* params){
    assert(params);

    return (int) pow((int) params[0], (int) params[1]);
}

int sinH(int* params){
    assert(params);

    return (int) sin(params[0]);
} 

int cosH(int* params){
    assert(params);

    return (int) cos(params[0]);
} 

int lnH (int* params){
    assert(params);

    return (int) log(params[0]);
}