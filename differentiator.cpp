#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"
#include "tableDerivative.h"
#include "operations.h"
#include "mathHandlers.h"

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

static void freeNode(treeNode_t* node, bool withoutRoot);
static void freeLeftSubtree(treeNode_t* node);
static void freeRightSubtree(treeNode_t* node);

static curAnchorNode readNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos);
static curAnchorNode readCreateNode(differentiator_t* differentiator, char* buffer, size_t* cuBufferPose);
static treeNode_t* readAllocateNodeIfNeed(differentiator_t* differentiator, size_t countReadNodes);
static bool isNilNode(char* buffer, size_t* curBufferPos);
static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos);
static bool isSupportedOperation(char readSym);

static bool checkNoVariables(treeNode_t* curNode);
static bool collapseConstant(treeNode_t* subTreeRoot);
static int calculateSubTree(treeNode_t* subTreeRoot);
static treeNode_t* createNumNode(int value, treeNode_t* parent);
static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot);

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

    freeNode(differentiator->root, false);

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

static void freeNode(treeNode_t* node, bool withoutRoot = false){
    assert(node);

    static int depth = 1;
    

    if(node->left){
        depth++;
        freeNode(node->left);
        depth--;
    }
    

    if(node->right){
        depth++;
        freeNode(node->right);
        depth--;
    }
    
    
    if(node->nodeType != NUMBER){
        free(node->data.operatorVar);
    }

    if(!withoutRoot || (withoutRoot && depth != 1)){
        LPRINTF("free: %p", node);
        LPRINTF("depth: %d", depth);
        poisonMemory(node, sizeof(*node));
        free(node);
        node = NULL;
        return;
    }

    LPRINTF("end freeing memory");
}

static curAnchorNode readNode(differentiator_t* differentiator, char* buffer, size_t* curBufferPos){
    assert(differentiator);
    assert(buffer);

    // log(differentiator, "startReadNode with curBuffer: %s", &buffer[*curBufferPos]);
    static bool continueReading = true;

    if(!continueReading){
        LPRINTF("Ошибка! Возвращаю NULL");
        return NULL;
    }

    skipSpaceAndCloseBracket(buffer, curBufferPos);

    treeNode_t* createdNode = NULL;
    
    LPRINTF("buffer[*curBufferPos] = %c\n", buffer[*curBufferPos]);
    if((buffer[*curBufferPos] == '(') ){
        createdNode = readCreateNode(differentiator, buffer, curBufferPos);
    }
    else if(isNilNode(buffer, curBufferPos)){
        return NULL;
    }
    else{
        LPRINTF("readNode: неожиданный символ, не '(' и не nil. Позиция: %zu\n", *curBufferPos);
        continueReading = false;
        return NULL; // <- важно: не продолжать с createdNode == NULL
    }
    LPRINTF("адрес текущей созданной ноды: %p", createdNode);

    skipSpaceAndCloseBracket(buffer, curBufferPos);

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
    
    
    static size_t countReadNodes = 1;
    treeNode_t* curNode = readAllocateNodeIfNeed(differentiator, countReadNodes);
    countReadNodes++;

    (*curBufferPos)++;
    
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
    else if(isSupportedOperation(curNodeData[0])){
        LPRINTF("%p - addr curNode", curNode);
        curNode->nodeType = OPERATOR;
        curNode->data.operatorVar = myStrDup(curNodeData);
        LPRINTF("curNode->data.operatorVar = %s", curNode->data.operatorVar);
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

static bool isSupportedOperation(char readSym){
    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(readSym == operations[curOper].symbol[0]){
            LPRINTF("Операция доступна");
            return true;
        }
    }

    return false;
}

static treeNode_t* readAllocateNodeIfNeed(differentiator_t* differentiator, size_t countReadNodes){
    assert(differentiator);
    assert(curNode);

    treeNode_t* curNode;

    if(countReadNodes == 1){
        curNode = differentiator->root;
        (differentiator->size)--;
        LPRINTF("Получил addr: %p", curNode);
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
    LPRINTF("differentiateNode start");
    treeNode_t* createdNode = NULL;

    LPRINTF("toDifferentiate->nodeType = %d", toDifferentiate->nodeType);
    switch(toDifferentiate->nodeType){
        case NUMBER:   createdNode = numDiff();                 break;
        case VARIABLE: createdNode = varDiff();                 break;
        case OPERATOR: createdNode = operDiff(toDifferentiate); break;
        default: break;
    }

    return createdNode;
}

bool optimizeDerivative(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    LPRINTF("\n\nstart optimization");
    if(checkNoVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        collapseConstant(subTreeRoot);
    }
    subTreeRoot = removeNeutralElements(subTreeRoot);

    if(subTreeRoot->left){
        optimizeDerivative(subTreeRoot->left);
    }
    if(subTreeRoot->right){
        optimizeDerivative(subTreeRoot->right);
    }

    return true;
}

static bool checkNoVariables(treeNode_t* curNode){
    assert(curNode);

    if(curNode->nodeType == VARIABLE){
        return false;
    }

    if(curNode->left){
        if(!checkNoVariables(curNode->left)) return false;
    }
    if(curNode->right){
        if(!checkNoVariables(curNode->right)) return false;
    }

    return true;
}

static bool collapseConstant(treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    int calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    freeNode(subTreeRoot, true);

    subTreeRoot = createNumNode(calculatedVal, subTreeRoot);
    
    return true;
}

static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    LPRINTF("start removal neutral");
    if(subTreeRoot->nodeType == OPERATOR){
        if(subTreeRoot->data.operatorVar[0] == '*' && subTreeRoot->left->data.num == 0){
            LPRINTF("zero division case");

            freeNode(subTreeRoot, true);
            subTreeRoot = createNumNode(0, subTreeRoot);
            return subTreeRoot;
        }
        if((subTreeRoot->data.operatorVar[0] == '*' && subTreeRoot->left->data.num == 1) || 
            (subTreeRoot->data.operatorVar[0] == '+' && subTreeRoot->left->data.num == 0)){
                LPRINTF("keep only right subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->right;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->right;
                }

                treeNode_t* result = subTreeRoot->right;

                freeLeftSubtree(subTreeRoot);

                return result;
        }
        if((subTreeRoot->data.operatorVar[0] == '*' && subTreeRoot->right->data.num == 1) || 
            (subTreeRoot->data.operatorVar[0] == '+' && subTreeRoot->right->data.num == 0)){
                LPRINTF("keep only left subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->left;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->left;
                }
                treeNode_t* result = subTreeRoot->left;
                
                freeRightSubtree(subTreeRoot);

                return result;
        }
    }

    return subTreeRoot;
}

static void freeLeftSubtree(treeNode_t* node){
    assert(node);

    LPRINTF("start freeing leftSubtree %p", node);

    static int depth = 1;

    if(node->left){
        depth++;
        freeLeftSubtree(node->left);
        depth--;
    }
    

    if(node->right && depth != 1){
        depth++;
        freeLeftSubtree(node->right);
        depth--;
    }
    
    
    if(node->nodeType != NUMBER){
        free(node->data.operatorVar);
    }

    poisonMemory(node, sizeof(*node));
    free(node);
    node = NULL;
}

static void freeRightSubtree(treeNode_t* node){
    assert(node);

    static int depth = 1;
    

    if(node->left && depth != 1){
        depth++;
        freeRightSubtree(node->left);
        depth--;
    }
    

    if(node->right){
        depth++;
        freeRightSubtree(node->right);
        depth--;
    }
    
    
    if(node->nodeType != NUMBER){
        free(node->data.operatorVar);
    }

    poisonMemory(node, sizeof(*node));
    free(node);
    node = NULL;
}

static treeNode_t* createNumNode(int value, treeNode_t* curNode){

    LPRINTF("start creating calculated node");

    curNode->nodeType = NUMBER;
    curNode->data.num = value;

    curNode->left   = NULL;
    curNode->right  = NULL;

    return curNode;
}

static int calculateSubTree(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    if(subTreeRoot->nodeType == OPERATOR){
        for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
            if(operations[curOper].symbol[0] == subTreeRoot->data.operatorVar[0]){
                return operations[curOper].calcHandler(calculateSubTree(subTreeRoot->left), calculateSubTree(subTreeRoot->right));
            }
        }
    }
    else{
        return subTreeRoot->data.num;
    }
}