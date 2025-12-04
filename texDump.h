#include "generalDifferentiator.h"

#include <stdio.h>

void texDumpTree(tree_t* expression, FILE* externalFilePtr = NULL, bool isTailorTree = false);
void startTexDumpTailor();

void endTexFile(tree_t* expression, tree_t* derivative, tree_t* scaleGraphic);