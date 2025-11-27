#include "expressionTree.h"
#include "tree.h"

#define DEBUG

#include "general/strFunc.h"
#include "general/debug.h"
#include "general/poison.h"

#include <assert.h>
#include <malloc.h>
#include <ctype.h>

const size_t MAX_VARIABLE_SIZE = 64;

static treeNode_t* getG(treeNode_t* node, char* buffer, char* curBufferPos);
static treeNode_t* getE(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getT(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getP(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getN(treeNode_t* node, char* buffer, char** curBufferPos);
static treeNode_t* getV(treeNode_t* node, char* buffer, char** curBufferPos);

void SyntaxError();

treeNode_t* readNode(tree_t* expression, char* buffer, size_t* curBufferPos){
    LPRINTF("begin reading root: %p", expression->root);

    expression->root = getG(expression->root, buffer, &buffer[*curBufferPos]);

    return expression->root;
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

    while(**curBufferPos == '+' || **curBufferPos == '-'){
        int op = **curBufferPos;
        (*curBufferPos)++;
        treeNode_t* val2 = getT(node, buffer, curBufferPos);
        assert(val2);

        if(op == '+'){
            val1 = createNewNodeOperator("+", val1, val2);
        }
        else if(op == '-'){
            val1 = createNewNodeOperator("-", val1, val2);
        }
    }


    return val1;
}

static treeNode_t* getT(treeNode_t* node, char* buffer, char** curBufferPos){
    assert(curBufferPos);

    LPRINTF("Зашел в T. Строка сейчас: %s", *curBufferPos);

    treeNode_t* val1 = getP(node, buffer, curBufferPos);
    assert(val1);
    
    LPRINTF("Начинаю анализ на знак умножения/деления. s = %s", *curBufferPos);

    while(**curBufferPos == '*' || **curBufferPos == '/'){
        LPRINTF("Нашел знак: s = %c", **curBufferPos);

        int op = **curBufferPos;
        (*curBufferPos)++;
        treeNode_t* val2 = getP(node, buffer, curBufferPos);
        assert(val2);

        if(op == '*'){
            val1 = createNewNodeOperator("*", val1, val2);
        }
        else{
            val1 = createNewNodeOperator("\\", val1, val2);
        }
    }

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
        else if(**curBufferPos != '*' && **curBufferPos != '/' && **curBufferPos != '+' && **curBufferPos != '-'){
            return getV(node, buffer, curBufferPos);
        }
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
    }
    if(*curBufferPos == startS){
        SyntaxError();
    }

    treeNode_t* result = createNewNodeVariable(variable, NULL, NULL);
    free(variable);

    return result;
}

void SyntaxError(){
    printf("Синтаксическая ошибка!!");
}

treeNode_t* createNewNodeNumber(int value, treeNode_t* left, treeNode_t* right){
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = NUMBER;

    newNode->data.num = value;

    return newNode;
}

treeNode_t* createNewNodeVariable(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = VARIABLE;
    newNode->data.var = myStrDup(name);
    
    setParent(newNode);
    
    return newNode;
}

treeNode_t* createNewNodeOperator(char* name, treeNode_t* left, treeNode_t* right){
    assert(name);
    assert(left);
    assert(right);

    treeNode_t* newNode = createNewNode(left, right);

    newNode->type = OPERATOR;
    newNode->data.op = myStrDup(name);

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

