#ifndef TABLE_DERIVATIVE_H
#define TABLE_DERIVATIVE_H

#include "generalDifferentiator.h"

treeNode_t* numDiff();
treeNode_t* varDiff    (treeNode_t* node, const char* variableToDiff);
treeNode_t* operDiff   (treeNode_t* node, const char* variableToDiff);

treeNode_t* addDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* subDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* mulDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* divDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* powDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* sinDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* cosDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* tanDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* cotDiff    (treeNode_t** params, const char* variableToDiff);
treeNode_t* lnDiff     (treeNode_t** params, const char* variableToDiff);
treeNode_t* shDiff     (treeNode_t** params, const char* variableToDiff);
treeNode_t* chDiff     (treeNode_t** params, const char* variableToDiff);
treeNode_t* arcsinDiff (treeNode_t** params, const char* variableToDiff);
treeNode_t* arccosDiff (treeNode_t** params, const char* variableToDiff);
treeNode_t* arctanDiff (treeNode_t** params, const char* variableToDiff);
treeNode_t* sqrtDiff   (treeNode_t** params, const char* variableToDiff);
treeNode_t* expFuncDiff(treeNode_t** params, const char* variableToDiff);


#endif /* TABLE_DERIVATIVE_H */