#ifndef TABLE_DERIVATIVE_H
#define TABLE_DERIVATIVE_H

#include "generalDifferentiator.h"

treeNode_t* numDiff();
treeNode_t* varDiff();
treeNode_t* operDiff(treeNode_t* toDifferentiate); // node

treeNode_t* addDiff(treeNode_t* left, treeNode_t* right);
treeNode_t* mulDiff(treeNode_t* left, treeNode_t* right);

#endif /* TABLE_DERIVATIVE_H */