#include "generalDifferentiator.h"

typedef treeNode_t* subTreeRoot;

subTreeRoot differentiatorCtor(expression_t* expression);
subTreeRoot differentiatorDtor(expression_t* expression);

void differentiatorReadData(expression_t* expression);

expression_t differentiate(expression_t* expression);
treeNode_t* differentiateNode(treeNode_t* node);

bool optimizeDerivative(treeNode_t* subTreeRoot);
