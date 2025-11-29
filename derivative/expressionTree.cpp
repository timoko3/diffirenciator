#include "expressionTree.h"
#include "operations.h"

#define DEBUG

#include "../general/tree.h"
#include "../general/strFunc.h"
#include "../general/debug.h"
#include "../general/poison.h"

#include <assert.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

const size_t MAX_VARIABLE_SIZE = 64;

static treeNode_t* getG(treeNode_t* node, char* buffer, char* curBufferPos);
static treeNode_t* getE(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getT(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getP(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getN(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getV(treeNode_t* node, char* buffer, char** curBufferPos);

static bool isSupportedOperation(char* readOpName);
static bool tryParseLongNameOp(char* opName, char** curBufferPos);
static bool stringNameOpHaveBracket(char* opName, char nextChar);

static void SyntaxError();

treeNode_t* readExpression(tree_t* expression, char* buffer, size_t* curBufferPos){
    LPRINTF("begin reading root: %p", expression->root);

    expression->root = getG(expression->root, buffer, &buffer[*curBufferPos]);

    return expression->root;
}

treeNode_t* createNewNodeNumber(int value, treeNode_t* left, treeNode_t* right){
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = NUMBER;

    newNode->data.num = value;

    return newNode;
}

treeNode_t* createNewNodeVariable(char* type, treeNode_t* left, treeNode_t* right){
    assert(type);
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = VARIABLE;
    newNode->data.var = myStrDup(type);
    
    setParent(newNode);
    
    return newNode;
}

treeNode_t* createNewNodeOperator(char* type, treeNode_t* left, treeNode_t* right){
    assert(type);
    
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = OPERATOR;
    newNode->data.op = myStrDup(type);

    setParent(newNode);

    return newNode;
}

void copyExpressionNode(treeNode_t* copy, treeNode_t* toCopy){
    assert(copy);
    assert(toCopy);

    copy->type = toCopy->type;
    if(toCopy->type == NUMBER){
        copy->data.num = toCopy->data.num;
    }
    else if(toCopy->type == VARIABLE){
        copy->data.var = myStrDup(toCopy->data.var);
    }
    else if(toCopy->type == OPERATOR){
        copy->data.op = myStrDup(toCopy->data.op);
    }
}

bool freeExpressionNodeData(treeNode_t* node, bool withoutRoot, int depth){
    assert(node);
    
    LPRINTF("entered freeExpressionNodeData func with node %p, withoutRoot = %d, depth = %d", node, withoutRoot, depth);

    if(!withoutRoot || (withoutRoot && depth != 1)){
        if(node->type == VARIABLE){
            free(node->data.var);
        }
        if(node->type == OPERATOR){
            free(node->data.op);
        }
        return true;
    }

    return false;
}

static treeNode_t* getG(treeNode_t* node, char* buffer, char* curBufferPos){
    assert(curBufferPos);
    
    LPRINTF("Зашел в G. Строка сейчас: %s", curBufferPos);
    node = getE(node, buffer, &curBufferPos);
    if(*curBufferPos != '$'){
        SyntaxError();
    }
    curBufferPos++;

    LPRINTF("Завершаю чтение и возвращаю ноду: %p", node);

    return node;
}

static treeNode_t* getE(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(curBufferPos);

    LPRINTF("Зашел в E. Строка сейчас: %s", *curBufferPos);

    treeNode_t* val1 = getT(node, buffer, curBufferPos);
    assert(val1);

    LPRINTF("Начинаю анализ на знак сложения/вычитания. s = %s", *curBufferPos);

    char* opName = (char*) calloc(MAX_VARIABLE_SIZE, sizeof(char));
    while(**curBufferPos == '+' || **curBufferPos == '-') {
        opName[0] = **curBufferPos;
        opName[1] = '\0';
        (*curBufferPos)++;

        treeNode_t* val2 = getT(node, buffer, curBufferPos);
        assert(val2);

        val1 = createNewNodeOperator(opName, val1, val2);
    }
    free(opName);

    return val1;
}

static treeNode_t* getT(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(curBufferPos);

    LPRINTF("Зашел в T. Строка сейчас: %s", *curBufferPos);

    treeNode_t* val1 = getP(node, buffer, curBufferPos);
    assert(val1);
    
    LPRINTF("Начинаю анализ на знак умножения/деления. s = %s", *curBufferPos);

    char* opName = (char*) calloc(MAX_VARIABLE_SIZE, sizeof(char));
    while(**curBufferPos == '*' || **curBufferPos == '/') {
        opName[0] = **curBufferPos;
        opName[1] = '\0';
        (*curBufferPos)++;

        treeNode_t* val2 = getP(node, buffer, curBufferPos);
        assert(val2);

        val1 = createNewNodeOperator(opName, val1, val2);
    }
    free(opName);

    return val1;
}

static treeNode_t* getP(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(curBufferPos);

    LPRINTF("Зашел в P. Строка сейчас: %s", *curBufferPos);

    if(**curBufferPos == '('){
        (*curBufferPos)++;
        treeNode_t* val = getE(node, buffer, curBufferPos);
        assert(val);

        (*curBufferPos)++;
        return val;
    }
    else{
        if('0' <= **curBufferPos && **curBufferPos <= '9'){
            return getN(node, buffer, curBufferPos);
        }
        
        char* opName = (char*) calloc(MAX_VARIABLE_SIZE, sizeof(char));
        assert(opName);

        treeNode_t* result = NULL;
        if(tryParseLongNameOp(opName, curBufferPos)){
            (*curBufferPos)++;

            treeNode_t* param = getE(node, buffer, curBufferPos);

            (*curBufferPos)++;

            result = createNewNodeOperator(opName, param, NULL);
        }
        else{
            result = getV(node, buffer, curBufferPos);
        }

        free(opName);
        return result;
    }
}

static treeNode_t* getN(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(curBufferPos);

    LPRINTF("Зашел в N. Строка сейчас: %s", *curBufferPos);

    char* startS = *curBufferPos;

    int val = 0;
    while('0' <= **curBufferPos && **curBufferPos <= '9'){
        LPRINTF("Получаю число val = %d", val);

        val = val * 10 + (**curBufferPos - '0');

        LPRINTF("Получаю число val = %d", val);

        (*curBufferPos)++;

        LPRINTF("Строка сейчас: %s", *curBufferPos);
    }
    if(*curBufferPos == startS){
        SyntaxError();
    }

    return createNewNodeNumber(val, NULL, NULL);;
}

static treeNode_t* getV(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(buffer);

    LPRINTF("Зашел в V. Строка сейчас: %s", *curBufferPos);   
    char* startS = *curBufferPos;
    
    char* variable = (char*) calloc(MAX_VARIABLE_SIZE, sizeof(char));

    size_t curVarPos = 0;
    while(isalpha(**curBufferPos)){
        variable[curVarPos] = **curBufferPos;

        (*curBufferPos)++;
        curVarPos++;
    }
    variable[curVarPos] = '\0';

    if(*curBufferPos == startS){
        SyntaxError();
    }

    treeNode_t* result = createNewNodeVariable(variable, NULL, NULL);
    free(variable);

    return result;
}

static void SyntaxError(){
    printf("Синтаксическая ошибка!!");
}

static bool isSupportedOperation(char* readOpName){
    assert(readOpName);

    for(size_t curOper = 0; curOper < sizeof(operations) / sizeof(operation_t); curOper++){
        if(isEqualStrings(readOpName, operations[curOper].nameString)){
            LPRINTF("Операция доступна");
            return true;
        }
    }

    return false;
}

static bool isOperationPrefix(const char* prefix) {
    for (size_t curOper = 0; curOper < sizeof(operations)/sizeof(operation_t); curOper++) {
        const char* op = operations[curOper].nameString;
        if (strncmp(prefix, op, strlen(prefix)) == 0) {
            return true;
        }
    }
    return false;
}

static bool tryParseLongNameOp(char* opName, char** curBufferPos){
    assert(opName);
    assert(curBufferPos);

    size_t curOpNamePos = 0;

    char* saveStartPos = *curBufferPos;

    while (isalpha(**curBufferPos)) {
        opName[curOpNamePos++] = **curBufferPos;
        opName[curOpNamePos] = '\0';

        if (!isOperationPrefix(opName)) {
            opName[--curOpNamePos] = '\0';
            return false;
        }

        (*curBufferPos)++;
    }

    if (curOpNamePos == 0) {
        *curBufferPos = saveStartPos;  
        return false;
    }

    if (!stringNameOpHaveBracket(opName, **curBufferPos)) {
        *curBufferPos = saveStartPos;
        return false;
    }

    return true;
}

static bool stringNameOpHaveBracket(char* opName, char nextChar){
    assert(opName);

    if (isSupportedOperation(opName) && nextChar == '(') return true;
    return false;
}