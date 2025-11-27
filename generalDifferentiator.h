#ifndef GENERAL_AKINATOR_H
#define GENERAL_AKINATOR_H

#include <stddef.h>
#include <limits.h>

#define log(expression, text, ...) htmlLog(expression, __FILE__, __FUNCTION__, __LINE__, text, ##__VA_ARGS__)

// var
// op
// num

union treeVal_t{
    char* op;
    char* var;
    int num;
};

enum nodeType{
    NO_TYPE,
    OPERATOR,
    VARIABLE,
    NUMBER
};

// data left right
// data = {value + type}

struct treeNode_t{
    nodeType    type;
    treeVal_t   data;
    treeNode_t* right;
    treeNode_t* left;
    treeNode_t* parent;
};

struct expression_t{ 
    treeNode_t*   root;
    size_t        amountNodes;
};

#endif /* GENERAL_AKINATOR_H */ 