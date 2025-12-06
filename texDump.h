#include "generalDifferentiator.h"

#include <stdio.h>

void texDumpTree(tree_t* expression, FILE* externalFilePtr = NULL, bool isTailorTree = false, tree_t* tailorX0 = NULL, tree_t* tailorOrder = NULL);
void startTexDumpTailor(tree_t* tailorX0, tree_t* tailorOrder);

FILE* generateGraphic(tree_t* expression, tree_t* scaleGraphicX, tree_t* scaleGraphicY);
void endGraphicDump();

void pythonGenGraphic(tree_t* expression, tree_t* derivative, tree_t* tailor, tree_t* tailorOrder, tree_t* tailorX0, tree_t* scaleGraphicX, tree_t* scaleGraphicY);

void endTexFile(tree_t* expression, tree_t* derivative);