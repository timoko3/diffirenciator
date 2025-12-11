#include "generalDifferentiator.h"

tree_t differentiate(tree_t* expression, const char* variableToDiff);
treeNode_t* differentiateNode(treeNode_t* node, const char* variableToDiff);

bool optimizeExpression(tree_t* derivative, treeNode_t* subTreeRoot, bool needDump = false);

tree_t tangentExpression(tree_t* expression, tree_t* derivative, tree_t* tailorX0, const char* variableToDiff);

tree_t tailorExpansion(tree_t* expression, const char* variableToDiff, tree_t* tailorX0, tree_t* tailorOrder);
double calculateSubTree (treeNode_t* subTreeRoot);