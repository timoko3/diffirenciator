#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

int main(void){

    treeNode_t* node1 = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(node1);
    node1->nodeType = VARIABLE;
    node1->data.operatorVar = "x";

    treeNode_t* node2 = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(node2);

    node2->nodeType = OPERATOR;
    node2->data.operatorVar = "ADD";

    treeNode_t* node3 = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(node3);

    node3->nodeType = NUMBER;
    node3->data.num = 30;

    node1->left  = node2;
    node1->right = node3;


    differentiator_t differentiator;
    differentiator.root = node1;
    differentiator.size = 3;

    treeGraphDump(&differentiator);

    free(node1);
    free(node2);
    free(node3);
}