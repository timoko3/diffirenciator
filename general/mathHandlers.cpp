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

int tanH(int* params){
    assert(params);

    return (int) tan(params[0]);
} 

int cotH(int* params){
    assert(params);

    return (int) (1 / tan(params[0]));
} 

int lnH (int* params){
    assert(params);

    return (int) log(params[0]);
}

int shH (int* params){
    assert(params);

    return (int) sinh(params[0]);
}

int chH (int* params){
    assert(params);

    return (int) cosh(params[0]);
}

int arcsinH (int* params){
    assert(params);

    return (int) asin(params[0]);
}

int arccosH (int* params){
    assert(params);

    return (int) acos(params[0]);
}

int arctanH (int* params){
    assert(params);

    return (int) atan(params[0]);
}

int sqrtH (int* params){
    assert(params);

    return (int) sqrt(params[0]);
}

int factorial(int n){

    int factorial = 1;

    for(int curNum = 1; curNum <= n; curNum++){
        factorial *= curNum;
    }

    return factorial;
}