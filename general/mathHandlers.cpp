#include "mathHandlers.h"

#define DEBUG

#include "debug.h"

#include <math.h>
#include <assert.h>
#include <stdio.h>

double addH(double* params){
    assert(params);

    return params[0] + params[1];
}

double subH(double* params){
    assert(params);

    return params[0] - params[1];
}

double mulH(double* params){
    assert(params);

    return params[0] * params[1];
} 

double divH(double* params){
    assert(params);

    if((params[1] - 0) < 0.0001){
        printf("Деление на 0 невозможно!\n");
    }

    return params[0] / params[1];
} 

double powH(double* params){
    assert(params);

    return pow(params[0], params[1]);
}

double sinH(double* params){
    assert(params);

    LPRINTF("Сейчас операция синус c параметром %lf\n", params[0]);

    return sin(params[0]);
} 

double cosH(double* params){
    assert(params);

    return cos(params[0]);
} 

double tanH(double* params){
    assert(params);

    return  tan(params[0]);
} 

double cotH(double* params){
    assert(params);

    return (1 / tan(params[0]));
} 

double lnH (double* params){
    assert(params);

    return log(params[0]);
}

double shH (double* params){
    assert(params);

    return sinh(params[0]);
}

double chH (double* params){
    assert(params);

    return cosh(params[0]);
}

double arcsinH (double* params){
    assert(params);

    return asin(params[0]);
}

double arccosH (double* params){
    assert(params);

    return acos(params[0]);
}

double arctanH (double* params){
    assert(params);

    return atan(params[0]);
}

double sqrtH (double* params){
    assert(params);

    return sqrt(params[0]);
}

double factorial(double n){

    double factorial = 1;

    for(double curNum = 1; curNum <= n; curNum++){
        factorial *= curNum;
    }

    return factorial;
}

bool isEqualDouble(double num1, double num2){
    if(fabs(num2 - num1) < 0.00001){
        // LPRINTF("%lf и %lf равны");
        return true;
    }

    return false; 
}