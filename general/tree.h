#include "generalDifferentiator.h"

treeNode_t* treeCtor(tree_t* expression);
treeNode_t* treeDtor(tree_t* expression);

tree_t treeRead(tree_t* expression, const char* expressionFileName);

treeNode_t* createNewNode(treeNode_t* left, treeNode_t* right);
treeNode_t* copyNode(treeNode_t* node);
bool setParent(treeNode_t* curNode);

void freeNode(treeNode_t* node, bool withoutRoot);
void freeLeftSubtree(treeNode_t* node,  bool withoutRoot);
void freeRightSubtree(treeNode_t* node, bool withoutRoot);