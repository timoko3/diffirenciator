#include "generalDifferentiator.h"

treeNode_t* treeCtor(tree_t* expression);
treeNode_t* treeDtor(tree_t* expression);

void treeRead(tree_t* expression);

treeNode_t* createNewNode(treeNode_t* left, treeNode_t* right);
treeNode_t* copyNode(treeNode_t* toCopy);
bool setParent(treeNode_t* curNode);

void freeNode(treeNode_t* node, bool withoutRoot);
void freeLeftSubtree(treeNode_t* node,  bool withoutRoot);
void freeRightSubtree(treeNode_t* node, bool withoutRoot);