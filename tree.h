#include "generalDifferentiator.h"

treeNode_t* createNewNode(treeNode_t* left, treeNode_t* right);
treeNode_t* copyNode(treeNode_t* toCopy);
bool setParent(treeNode_t* curNode);

treeNode_t* createNewNodeNumber  (int value,  treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right);
treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right);

