#include "differentiator.h"
#include "generalDifferentiator.h"
#include "protectionDifferentiator.h"
#include "DSL.h"
#include "tableDerivative.h"
#include "operations.h"
#include "mathHandlers.h"
#include "expressionTree.h"
#include "tree.h"

// # tree.cpp
// NodeData ReadData (const char* data);

// ...
// ...

// # treeFunc.cpp // rename
// NodeData ReadData (const char* data);
// NodeData ReadData (const char* data) {

// }

#define DEBUG

#include "general/poison.h"
#include "general/debug.h"
#include "general/file.h" 
#include "general/strFunc.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

const char* DIFFERENTIATOR_DATA_FILE_NAME = "expression.txt"; //

static subTreeRoot readNode(expression_t* expression, char* buffer, size_t* curBufferPos);
static subTreeRoot readCreateNode(expression_t* expression, char* buffer, size_t* cuBufferPose);
static treeNode_t* readAllocateNodeIfNeed(expression_t* expression, size_t countReadNodes);
static bool isNilNode(char* buffer, size_t* curBufferPos);
static void skipSpaceAndCloseBracket(char* buffer, size_t* curBufferPos);
static bool isSupportedOperation(char readSym);

static bool checkNoVariables(treeNode_t* curNode);
static treeNode_t* collapseConstant(treeNode_t* subTreeRoot);
static int calculateSubTree(treeNode_t* subTreeRoot);
static treeNode_t* createNumNode(int value, treeNode_t* parent);
static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot);

subTreeRoot differentiatorCtor(expression_t* expression){
    assert(expression);

    expression->amountNodes = 1;

    expression->root = (treeNode_t*) calloc(1, sizeof(treeNode_t)); // 
    assert(expression->root);

    expression->root->left = NULL;
    expression->root->right = NULL;
    expression->root->parent = NULL;

    return expression->root;
}

subTreeRoot differentiatorDtor(expression_t* expression){
    assert(expression);

    freeNode(expression->root, false);

    poisonMemory(&expression->amountNodes, sizeof(expression->amountNodes));

    return NULL;
}

void differentiatorReadData(expression_t* expression){
    assert(expression);

    log(expression, "started reading");

    data_t expressionData;
    parseStringsFile(&expressionData, DIFFERENTIATOR_DATA_FILE_NAME);

    LPRINTF("akinator buffer: %s\n", expressionData.buffer);

    static size_t curPose = 0; // pos
    readNode(expression, expressionData.buffer, &curPose);
    
    free(expressionData.buffer);
    free(expressionData.strings);

    log(expression, "ended reading");
}

expression_t differentiate(expression_t* expression){
    assert(expression);

    LPRINTF("\n\n differentiation start\n");

    expression_t derivativeTree;

    differentiatorCtor(&derivativeTree);  //
    free(derivativeTree.root); //

    derivativeTree.root = differentiateNode(expression->root);

    return derivativeTree;
}

static subTreeRoot readNode(expression_t* expression, char* buffer, size_t* curBufferPos){
    assert(expression);
    assert(buffer);

    // log(expression, "startReadNode with curBuffer: %s", &buffer[*curBufferPos]);
    static bool continueReading = true;
    if(!continueReading){
        LPRINTF("Ошибка! Возвращаю NULL");
        return NULL;
    }

    skipSpaceAndCloseBracket(buffer, curBufferPos);

    treeNode_t* createdNode = NULL;
    
    LPRINTF("buffer[*curBufferPos] = %c\n", buffer[*curBufferPos]);
    if((buffer[*curBufferPos] == '(') ){
        createdNode = readCreateNode(expression, buffer, curBufferPos);
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
    createdNode->left = readNode(expression, buffer, curBufferPos);
    if(createdNode->left){
        createdNode->left->parent = createdNode;
    }

    createdNode->right = readNode(expression, buffer, curBufferPos);
    if(createdNode->right){
        createdNode->right->parent = createdNode;
    }
    
    (expression->amountNodes)++;

    // log(expression, "endReadNode with curBuffer: %s", &buffer[*curBufferPos]);

    return createdNode;
}

// rename
static subTreeRoot readCreateNode(expression_t* expression, char* buffer, size_t* curBufferPos){
    assert(expression);
    assert(buffer);
    
    static size_t countReadNodes = 1;
    treeNode_t* curNode = readAllocateNodeIfNeed(expression, countReadNodes);
    countReadNodes++;

    (*curBufferPos)++;
    
    LPRINTF("буфер перед чтением: %s", &buffer[*curBufferPos]);
    
    size_t lenName = 0;

    char* curNodeData = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    assert(curNodeData);

    sscanf(&buffer[*curBufferPos], "\"%[^\"]\"%n", curNodeData, &lenName);
    
    LPRINTF("Получил размер строки %lu и саму строку %s", lenName, curNodeData);

    if(isdigit(curNodeData[0])){
        curNode->type = NUMBER;
        sscanf(curNodeData, "%d", &curNode->data.num);
    }
    else if(isSupportedOperation(curNodeData[0])){
        LPRINTF("%p - addr curNode", curNode);
        curNode->type = OPERATOR;
        curNode->data.op = myStrDup(curNodeData);
        LPRINTF("curNode->data.op = %s", curNode->data.op);
    }
    else{
        curNode->type = VARIABLE;
        curNode->data.var = myStrDup(curNodeData); 
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

// rename
static treeNode_t* readAllocateNodeIfNeed(expression_t* expression, size_t countReadNodes){
    assert(expression);

    treeNode_t* curNode;

    if(countReadNodes == 1){
        curNode = expression->root;
        (expression->amountNodes)--;
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

    // char curNodeData[MAX_ANSWER_SIZE] = {};
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

// open(file)
// openFile(toOpen)

treeNode_t* differentiateNode(treeNode_t* node){ // to - сущ. // node
    assert(node);
    LPRINTF("differentiateNode start");
    treeNode_t* createdNode = NULL;

    LPRINTF("node->type = %d", node->type);
    switch(node->type){
        case NUMBER:   createdNode = numDiff();      break;
        case VARIABLE: createdNode = varDiff();      break;
        case OPERATOR: createdNode = operDiff(node); break;
        default: break;
    }

    return createdNode;
}

// (5+3) - (7+1)

//       -
//   +       +
// 5   3   7   1

//       -
//   8       +
//         7   1

//       -
//   8       8
  
//       0
  

bool optimizeDerivative(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    LPRINTF("\n\nstart optimization");
    if(checkNoVariables(subTreeRoot)){
        LPRINTF("subTreeRoot addr: %p", subTreeRoot);
        subTreeRoot = collapseConstant(subTreeRoot);
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

static bool checkNoVariables(treeNode_t* curNode){ // have has
    assert(curNode);

    if(curNode->type == VARIABLE){
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

static treeNode_t* collapseConstant(treeNode_t* subTreeRoot){
    assert(subTreeRoot);

    treeNode_t* result = NULL;

    int calculatedVal = calculateSubTree(subTreeRoot);
    LPRINTF("calculatedVal: %d", calculatedVal);

    freeNode(subTreeRoot, true);
    
    LPRINTF("freeed collapsingConst with no problems");

    if(subTreeRoot->parent->left == subTreeRoot){
        subTreeRoot->parent->left = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    else{
        subTreeRoot->parent->right = createNewNodeNumber(calculatedVal, NULL, NULL);
        result = subTreeRoot->parent->right;
    }
    setParent(subTreeRoot->parent);

    freeExpressionNode(subTreeRoot, false, 1);

    return result;
}

static treeNode_t* removeNeutralElements(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    LPRINTF("start removal neutral");
    if(subTreeRoot->type == OPERATOR){
        if(subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 0){
            LPRINTF("zero division case");

            freeNode(subTreeRoot, true);
            subTreeRoot = createNumNode(0, subTreeRoot);
            return subTreeRoot;
        }
        // copypaste
        if((subTreeRoot->data.op[0] == '*' && subTreeRoot->left->data.num == 1) || 
            (subTreeRoot->data.op[0] == '+' && subTreeRoot->left->data.num == 0)){
                LPRINTF("keep only right subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->right;
                    subTreeRoot->right->parent = subTreeRoot->parent;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->right;
                }

                treeNode_t* result = subTreeRoot->right;
                LPRINTF("during optimization freeing leftSubtree");
                freeLeftSubtree(subTreeRoot, false);
                freeExpressionNode(subTreeRoot, false, 1);

                return result;
        }
        if((subTreeRoot->data.op[0] == '*' && subTreeRoot->right->data.num == 1) || 
            (subTreeRoot->data.op[0] == '+' && subTreeRoot->right->data.num == 0)){
                LPRINTF("keep only left subTree of %p", subTreeRoot);
                if(subTreeRoot->parent->left == subTreeRoot){
                    subTreeRoot->parent->left = subTreeRoot->left;
                }
                else{
                    subTreeRoot->parent->right = subTreeRoot->left;
                }
                treeNode_t* result = subTreeRoot->left;
                
                LPRINTF("during optimization freeing rightSubtree");
                freeRightSubtree(subTreeRoot, false);
                freeExpressionNode(subTreeRoot, false, 1);

                return result;
        }
    }

    return subTreeRoot;
}

static treeNode_t* createNumNode(int value, treeNode_t* curNode){

    LPRINTF("start creating calculated node");

    curNode->type = NUMBER;
    curNode->data.num = value;

    curNode->left   = NULL;
    curNode->right  = NULL;

    return curNode;
}

static int calculateSubTree(treeNode_t* subTreeRoot){
    assert(subTreeRoot);
    
    if(subTreeRoot->type == OPERATOR){
        for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
            if(operations[curOper].symbol[0] == subTreeRoot->data.op[0]){
                return operations[curOper].calcHandler(calculateSubTree(subTreeRoot->left), calculateSubTree(subTreeRoot->right));
            }
        }
    }
    else{
        return subTreeRoot->data.num;
    }
}