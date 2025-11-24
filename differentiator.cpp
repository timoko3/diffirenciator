#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"

#define DEBUG

#include "general/poison.h"
#include "general/debug.h"
#include "general/file.h" 
#include "general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>

const char* DIFFERENTIATOR_DATA_FILE_NAME = "differentiatorData.txt";

static void freeNode(treeNode_t* node);
static curAnchorNode readNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos);

static curAnchorNode readCreateNode(differentiator_t* differentiator, char* buffer, size_t* cuBufferPose);
static bool isNilNode(char* buffer, size_t* curBufferPos);
static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos);
static treeNode_t* differentiateNode(treeNode_t* toDifferentiate);

static treeNode_t* createNewNodeNumber(int value);
static treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right);
static treeNode_t* copyNode(treeNode_t* toCopy);

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

static curAnchorNode readCreateNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPose){
    assert(differentiator);
    assert(buffer);
    
    treeNode_t* curNode;

    static size_t countNodes = 1;
    if(countNodes == 1){
        curNode = differentiator->root;
        (differentiator->size)--;
    }
    else{
        curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
        assert(curNode);
        LPRINTF("Выделил память");
    }
    countNodes++;

    (*curBufferPose)++;
    
    LPRINTF("записал указатель в data нового node");
    size_t lenName = 0;

    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPose]);

    sscanf(&buffer[*curBufferPose], "\"%[^\"]\"%n", curNodeData, &lenName);
    LPRINTF("Получил размер строки %lu и саму строку %s", lenName, curNodeData);

    if(curNodeData[0] >= (int) '0' && curNodeData[0] <= (int) '9'){
        printf("зашел в NUMBER\n");
        curNode->nodeType = NUMBER;

        int dataNum = 0;
        sscanf(curNodeData, "%d", &dataNum);
        printf("Число dataNum: %d\n", dataNum);
        curNode->data.num = dataNum;
    }
    else if(curNodeData[0] == '*' || curNodeData[0] == '+'){
        curNode->nodeType = OPERATOR;

        curNode->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));

        strcpy(curNode->data.operatorVar, curNodeData);
        *curBufferPose += lenName;
        LPRINTF("\nбуфер после чтением: %s", &buffer[*curBufferPose]);
    }
    else{
        curNode->nodeType = VARIABLE;

        curNode->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));

        strcpy(curNode->data.operatorVar, curNodeData);
        *curBufferPose += lenName;
        LPRINTF("\nбуфер после чтением: %s", &buffer[*curBufferPose]);
    }

    free(curNodeData);
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


static treeNode_t* differentiateNode(treeNode_t* toDifferentiate){
    assert(toDifferentiate);


    treeNode_t* createdNode = NULL;

    switch(toDifferentiate->nodeType){
        case NUMBER:   createdNode = createNewNodeNumber(0);  break;
        case VARIABLE: createdNode = createNewNodeNumber(1); break;
        case OPERATOR:
            switch(toDifferentiate->data.operatorVar[0]){
                case '+':
                    createdNode = _ADD(_DIF(_L), _DIF(_R));
                    break;
                case '*': 
                    createdNode = _ADD(_MUL(_DIF(_L), _C(_R)), _MUL(_C(_L), _DIF(_R)));
                    // createdNode = createNewNodeOperator("+", createNewNodeOperator("*", differentiateNode(toDifferentiate->left), copyNode(toDifferentiate->right)), createNewNodeOperator("*", copyNode(toDifferentiate->left), differentiateNode(toDifferentiate->right)));
                    break;
                default: break;
            }     
        default: break;
    }


    return createdNode;
}

static treeNode_t* createNewNodeNumber(int value){
    treeNode_t* curNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(curNode);
    LPRINTF("Выделил память");

    curNode->nodeType  = NUMBER;
    curNode->data.num  = value;

    return curNode;
}

static treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(newNode);
    LPRINTF("Выделил память");

    newNode->nodeType = OPERATOR;
    newNode->left  = left;
    newNode->right = right;

    newNode->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(newNode->data.operatorVar);

    strcpy(newNode->data.operatorVar, name);

    return newNode;
}

static treeNode_t* copyNode(treeNode_t* toCopy){
    assert(toCopy);

    treeNode_t* copy = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    assert(copy);

    copy->nodeType = toCopy->nodeType;
    if(toCopy->nodeType == NUMBER){
        copy->data.num = toCopy->data.num;
    }
    else{
        copy->data.operatorVar = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
        assert(copy->data.operatorVar);

        strcpy(copy->data.operatorVar, toCopy->data.operatorVar);
    }

    if(toCopy->left){
        copy->left  = copyNode(toCopy->left);
    }
    if(toCopy->right){
        copy->right = copyNode(toCopy->right);
    }

    return copy;
}
