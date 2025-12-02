#ifndef MATH_HANDLERS_H
#define MATH_HANDLERS_H

#include "generalDifferentiator.h" 

int addH(int* params);
int subH(int* params);
int mulH(int* params);
int divH(int* params);
int powH(int* params);
int sinH(int* params);
int cosH(int* params);
int tanH(int* params);
int cotH(int* params);
int lnH (int* params);
int shH (int* params);
int chH (int* params);
int arcsinH (int* params);
int arccosH (int* params);
int arctanH (int* params);

int sqrtH (int* params);

int factorial(int n);

#endif /* MATH_HANDLERS_H */