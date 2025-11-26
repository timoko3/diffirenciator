#include "generalDifferentiator.h"

treeNode_t* createNewNode();

treeNode_t* createNewNodeNumber  (int value);
treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right);