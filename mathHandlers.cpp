#include "mathHandlers.h"

#include <math.h>
#include <assert.h>

int addH(int* params, size_t amountParam){
    assert(params);

    return params[0] + params[1];
}

int mulH(int* params, size_t amountParam){
    assert(params);

    return params[0] * params[1];
} 

int sinH(int* params, size_t amountParam){
    assert(params);

    return sin(params[0]);
} 