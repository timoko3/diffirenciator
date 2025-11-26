#ifndef GENERAL_AKINATOR_H
#define GENERAL_AKINATOR_H

#include <stddef.h>
#include <limits.h>

#define log(differentiator, text, ...) htmlLog(differentiator, __FILE__, __FUNCTION__, __LINE__, text, ##__VA_ARGS__)

// var
// op
// num

union treeVal_t{
    char* operatorVar; 
    int num;
};

enum infoType{ // node
    NO_TYPE,
    OPERATOR,
    VARIABLE,
    NUMBER
};

// data left right
// data = {value + type}

struct treeNode_t{
    infoType nodeType;
    treeVal_t   data;
    treeNode_t* right;
    treeNode_t* left;
    treeNode_t* parent;
};

struct akinatorState_t{ //
    treeNode_t* Node;
};

struct differentiator_t{ // expression ?
    treeNode_t*   root;
    size_t        size; // of ???
    akinatorState_t curState;
    // treeStatusDescription status;
};

treeVal_t* curData(differentiator_t* akinator);
treeNode_t** curNode(differentiator_t* akinator);

#endif /* GENERAL_AKINATOR_H */ 