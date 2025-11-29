#ifndef TABLE_DERIVATIVE_H
#define TABLE_DERIVATIVE_H

#include "generalDifferentiator.h"

treeNode_t* numDiff();
treeNode_t* varDiff();
treeNode_t* operDiff(treeNode_t* node);

treeNode_t* addDiff(treeNode_t** params, size_t amountParam);
treeNode_t* subDiff(treeNode_t** params, size_t amountParam);
treeNode_t* mulDiff(treeNode_t** params, size_t amountParam);
treeNode_t* divDiff(treeNode_t** params, size_t amountParam);
treeNode_t* sinDiff(treeNode_t** params, size_t amountParam);
treeNode_t* cosDiff(treeNode_t** params, size_t amountParam);

#endif /* TABLE_DERIVATIVE_H */