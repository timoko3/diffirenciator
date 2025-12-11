#ifndef MATH_HANDLERS_H
#define MATH_HANDLERS_H

#include "generalDifferentiator.h" 

double addH(double* params);
double subH(double* params);
double mulH(double* params);
double divH(double* params);
double powH(double* params);
double sinH(double* params);
double cosH(double* params);
double tanH(double* params);
double cotH(double* params);
double lnH (double* params);
double shH (double* params);
double chH (double* params);
double arcsinH (double* params);
double arccosH (double* params);
double arctanH (double* params);

double sqrtH (double* params);

double factorial(double n);

#endif /* MATH_HANDLERS_H */