#include "generalDifferentiator.h"

#include <assert.h>


treeVal_t* curData(differentiator_t* akinator){
    assert(akinator);

    return &akinator->curState.Node->data;
}

treeNode_t** curNode(differentiator_t* akinator){
    assert(akinator);

    return &akinator->curState.Node;
}