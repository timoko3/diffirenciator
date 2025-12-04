#include "generalDifferentiator.h"

treeNode_t* readExpression(tree_t* expression, char* buffer, size_t* curBufferPos);

treeNode_t* createNewNodeNumber  (int value,  treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeVariable(const char* type, treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeOperator(const char* type, treeNode_t* left, treeNode_t* right);

void copyExpressionNode(treeNode_t* copy, treeNode_t* node);
bool freeExpressionNodeData(treeNode_t* node, bool withoutRoot, int depth);

bool checkNotHaveVariables(treeNode_t* curNode);
