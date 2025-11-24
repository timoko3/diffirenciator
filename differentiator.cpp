#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"
#include "tableDerivative.h"

#define DEBUG

#include "general/poison.h"
#include "general/debug.h"
#include "general/file.h" 
#include "general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

const char* DIFFERENTIATOR_DATA_FILE_NAME = "differentiatorData.txt";

static void freeNode(treeNode_t* node);

static curAnchorNode readNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos);
static curAnchorNode readCreateNode(differentiator_t* differentiator, char* buffer, size_t* cuBufferPose);
static treeNode_t* readAllocateNodeIfNeed(differentiator_t* differentiator, treeNode_t* curNode, size_t countReadNodes);
static bool isNilNode(char* buffer, size_t* curBufferPos);
static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos);

static treeNode_t* differentiateNode(treeNode_t* toDifferentiate);

curAnchorNode differentiatorCtor(differentiator_t* differentiator){
    assert(differentiator);

    differentiator->size = 1;

    differentiator->root = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(differentiator->root);

    differentiator->root->left = NULL;
    differentiator->root->right = NULL;
    differentiator->root->parent = NULL;

    differentiator->curState.Node = differentiator->root;

    return differentiator->root;
}

curAnchorNode differentiatorDtor(differentiator_t* differentiator){
    assert(differentiator);

    freeNode(differentiator->root);

    poisonMemory(&differentiator->size, sizeof(differentiator->size));

    return NULL;
}

void differentiatorReadData(differentiator_t* differentiator){
    assert(differentiator);

    log(differentiator, "started reading");

    data_t differentiatorData;
    parseStringsFile(&differentiatorData, DIFFERENTIATOR_DATA_FILE_NAME);

    LPRINTF("akinator buffer: %s\n", differentiatorData.buffer);

    static size_t curPose = 0;
    readNode(differentiator, differentiatorData.buffer, &curPose);
    
    free(differentiatorData.buffer);
    free(differentiatorData.strings);

    log(differentiator, "ended reading");
}

differentiator_t differentiate(differentiator_t* differentiator){
    assert(differentiator);

    LPRINTF("\n\n differentiation start\n");

    differentiator_t derivativeTree;

    differentiatorCtor(&derivativeTree);
    free(derivativeTree.root);
    
    derivativeTree.root = differentiateNode(differentiator->root);

    return derivativeTree;
}

static void freeNode(treeNode_t* node){
    assert(node);

    static int i = 1;
    LPRINTF("%d) MEOW\n", i++);
    LPRINTF("nodeAddr: %p\n", node);
    
    if(node->left){
        freeNode(node->left);
    }

    if(node->right){
        freeNode(node->right);
    }

    
    if(node->nodeType != NUMBER){
        free(node->data.operatorVar);
    }

    poisonMemory(node, sizeof(*node));
    free(node);
    node = NULL;
}

static curAnchorNode readNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos){
    assert(differentiator);
    assert(buffer);

    // log(differentiator, "startReadNode with curBuffer: %s", &buffer[*curBufferPos]);

    skipSpaceAndCloseBracket(buffer, curBufferPos);

    treeNode_t* createdNode = NULL;
    
    if((buffer[*curBufferPos] == '(') ){
        LPRINTF("скобочка\n");
        createdNode = readCreateNode(differentiator, buffer, curBufferPos);
    }
    else{
        if(isNilNode(buffer, curBufferPos)){
            return NULL;
        }

        LPRINTF("readNode: неожиданный символ, не '(' и не nil. Позиция: %zu\n", *curBufferPos);
        return NULL; // <- важно: не продолжать с createdNode == NULL
    }

    LPRINTF("адрес текущей созданной ноды: %p", createdNode);
    (*curBufferPos)++;


    LPRINTF("\n\nрекурсивный запуск");
    createdNode->left = readNode(differentiator, buffer, curBufferPos);
    if(createdNode->left){
        createdNode->left->parent = createdNode;
    }

    createdNode->right = readNode(differentiator, buffer, curBufferPos);
    if(createdNode->right){
        createdNode->right->parent = createdNode;
    }
    
    (differentiator->size)++;

    // log(differentiator, "endReadNode with curBuffer: %s", &buffer[*curBufferPos]);

    return createdNode;
}

static curAnchorNode readCreateNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos){
    assert(differentiator);
    assert(buffer);
    
    treeNode_t* curNode;
    static size_t countReadNodes = 1;
    readAllocateNodeIfNeed(differentiator, curNode, countReadNodes);
    countReadNodes++;

    (*curBufferPos)++;///
    
    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPos]);
    
    size_t lenName = 0;

    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    sscanf(&buffer[*curBufferPos], "\"%[^\"]\"%n", curNodeData, &lenName);
    
    LPRINTF("Получил размер строки %lu и саму строку %s", lenName, curNodeData);

    if(isdigit(curNodeData[0])){
        curNode->nodeType = NUMBER;
        sscanf(curNodeData, "%d", &curNode->data.num);
    }
    else if(curNodeData[0] == '*' || curNodeData[0] == '+'){
        curNode->nodeType = OPERATOR;
        curNode->data.operatorVar = myStrDup(curNodeData);
    }
    else{
        curNode->nodeType = VARIABLE;
        curNode->data.operatorVar = myStrDup(curNodeData); 
    }
    *curBufferPos += lenName;
    LPRINTF("\nбуфер после чтением: %s", &buffer[*curBufferPos]);

    free(curNodeData);
    return curNode;
}

bool isSupportedOperation(char readSym){
    
}

static treeNode_t* readAllocateNodeIfNeed(differentiator_t* differentiator, treeNode_t* curNode, size_t countReadNodes){
    assert(differentiator);
    assert(curNode);

    if(countReadNodes == 1){
        curNode = differentiator->root;
        (differentiator->size)--;
    }
    else{
        curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
        assert(curNode);
        LPRINTF("Выделил память");
    }

    return curNode;
}

static bool isNilNode(char* buffer, size_t* curBufferPos){
    assert(buffer);
    assert(curBufferPos);

    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPos]);
    size_t lenName = 0;
    sscanf(&buffer[*curBufferPos], "%s%n", curNodeData, &lenName);
    LPRINTF("Прочиталось внутри случая nil: %s", curNodeData);
    if(isEqualStrings(curNodeData, "nil")){
        LPRINTF("Зашел в nil");
        *curBufferPos += lenName;
        LPRINTF("буфер после чтения: %s\n", &buffer[*curBufferPos]);
        free(curNodeData);
        return true;
    }

    free(curNodeData);

    return false;
}

static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos){
    assert(buffer);
    assert(curBufferPos);

    while(true){
        if((buffer[*curBufferPos] == ')') || (buffer[*curBufferPos] == ' ')){
            (*curBufferPos)++;
        }
        else{
            break;
        }
    }
}   


treeNode_t* differentiateNode(treeNode_t* toDifferentiate){
    assert(toDifferentiate);
 
    treeNode_t* createdNode = NULL;

    switch(toDifferentiate->nodeType){
        case NUMBER:   createdNode = numDiff();                 break;
        case VARIABLE: createdNode = varDiff();                 break;
        case OPERATOR: createdNode = operDiff(toDifferentiate); break;
        default: break;
    }

    return createdNode;
}

