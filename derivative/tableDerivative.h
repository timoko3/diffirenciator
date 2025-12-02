#ifndef TABLE_DERIVATIVE_H
#define TABLE_DERIVATIVE_H

#include "generalDifferentiator.h"

treeNode_t* numDiff();
treeNode_t* varDiff();
treeNode_t* operDiff(treeNode_t* node);

treeNode_t* addDiff    (treeNode_t** params);
treeNode_t* subDiff    (treeNode_t** params);
treeNode_t* mulDiff    (treeNode_t** params);
treeNode_t* divDiff    (treeNode_t** params);
treeNode_t* powDiff    (treeNode_t** params);
treeNode_t* sinDiff    (treeNode_t** params);
treeNode_t* cosDiff    (treeNode_t** params);
treeNode_t* tanDiff    (treeNode_t** params);
treeNode_t* cotDiff    (treeNode_t** params);
treeNode_t* lnDiff     (treeNode_t** params);
treeNode_t* shDiff     (treeNode_t** params);
treeNode_t* chDiff     (treeNode_t** params);
treeNode_t* arcsinDiff (treeNode_t** params);
treeNode_t* arccosDiff (treeNode_t** params);
treeNode_t* arctanDiff (treeNode_t** params);
treeNode_t* sqrtDiff   (treeNode_t** params);

#endif /* TABLE_DERIVATIVE_H */