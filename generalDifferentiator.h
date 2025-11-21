#ifndef GENERAL_AKINATOR_H
#define GENERAL_AKINATOR_H

#include <stddef.h>
#include <limits.h>

#define log(differentiator, text, ...) htmlLog(differentiator, __FILE__, __FUNCTION__, __LINE__, text, ##__VA_ARGS__)


union treeVal_t {
    char* operatorVar;
    int num;
};

enum infoType{
    OPERATOR,
    VARIABLE,
    NUMBER
};

struct treeNode_t{
    infoType nodeType;
    treeVal_t   data;
    treeNode_t* right;
    treeNode_t* left;
    treeNode_t* parent;
};

struct akinatorState_t{
    treeNode_t* Node;
};

struct differentiator_t{
    treeNode_t*   root;
    size_t        size;
    akinatorState_t curState;
    // treeStatusDescription status;
};

typedef char* (*handler_t)(treeNode_t* leftNode, treeNode_t* rightNode);

enum operationNames{
    ADD,
    MULTIPLY
};

struct operation_t{
    char* symbol;
    operationNames name;
    handler_t handler;
};

static operation_t operations[]{
    {"+", ADD,      addDiff},
    {"*", MULTIPLY, mulDiff}
};

treeVal_t* curData(differentiator_t* akinator);
treeNode_t** curNode(differentiator_t* akinator);

#endif /* GENERAL_AKINATOR_H */ 