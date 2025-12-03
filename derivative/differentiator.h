#include "generalDifferentiator.h"

tree_t differentiate(tree_t* expression, const char* variableToDiff);
treeNode_t* differentiateNode(treeNode_t* node, const char* variableToDiff);

bool optimizeExpression(tree_t* derivative, treeNode_t* subTreeRoot);

tree_t tailorExpansion(tree_t* expression, const char* variableToDiff);