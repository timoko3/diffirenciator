#ifndef TREE_PROTECTION_H
#define TREE_PROTECTION_H

#include "generalDifferentiator.h"

#include <stdio.h>

void htmlLog(differentiator_t* differentiator, const char* callFileName, const char* callFuncName, int callLine,
                           const char* dumpDescription, ...);
void treeGraphDump(differentiator_t* differentiator);
// treeStatus verifyTree(differentiator_t* tree, const char* function, const char* file, const int line);

#endif /* TREE_PROTECTION_H */