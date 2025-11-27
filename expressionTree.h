#include "generalDifferentiator.h"

treeNode_t* createNewNodeNumber  (int value,  treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right);

void copyExpressionNode(treeNode_t* copy, treeNode_t* toCopy);
bool freeExpressionNodeData(treeNode_t* node, bool withoutRoot, int depth);

