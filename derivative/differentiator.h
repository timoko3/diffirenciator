#include "generalDifferentiator.h"

tree_t differentiate(tree_t* expression);
treeNode_t* differentiateNode(treeNode_t* node);

bool optimizeDerivative(tree_t* derivative, treeNode_t* subTreeRoot);
