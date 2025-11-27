#ifndef TREE_PROTECTION_H
#define TREE_PROTECTION_H

#include "generalDifferentiator.h"

#include <stdio.h>

void htmlLog(expression_t* expression, const char* callFileName, const char* callFuncName, int callLine,
                           const char* dumpDescription, ...);
void treeGraphDump(expression_t* expression);
// treeStatus verifyTree(expression_t* tree, const char* function, const char* file, const int line);

#endif /* TREE_PROTECTION_H */