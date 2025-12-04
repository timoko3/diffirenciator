#include "differentiatorConfig.h"
#include "tree.h"
#include "expressionTree.h"

#define DEBUG

#include "../general/debug.h"

#include <stdio.h>
#include <assert.h>

const size_t MAX_CONFIG_NAME_SIZE = 30;

static bool getConfigParameter(const char* configParamName, configParam** curConfigParam);

void differentiatorReadConfigParam(string curString){
    LPRINTF("read config param");

    size_t curPos = 0;

    char* curBufferPtr = curString.ptr;
    
    char curNameConfigParam[MAX_CONFIG_NAME_SIZE] = "";

    int amountReadSymbols = 0;

    sscanf(curString.ptr, "%s%n", curNameConfigParam, &amountReadSymbols);
    LPRINTF("amountReadSymbols = %d", amountReadSymbols);

    curPos += amountReadSymbols;

    while(curString.ptr[curPos] == ' ' || curString.ptr[curPos] == '='){
        curPos++;
    }

    configParam* curConfigParam = NULL;

    if(getConfigParameter(curNameConfigParam, &curConfigParam)){
        LPRINTF("start getting configParam");
        curConfigParam->treeParam.root = readExpression(&curConfigParam->treeParam, curString.ptr, &curPos);
        LPRINTF("treeParam.root = %p", curConfigParam->treeParam.root);

        
    }
        
}

tree_t* findConfigParameter(differentiatorConfigParams type){
    for(size_t curConfigInd = 0; curConfigInd < sizeof(config) / sizeof(configParam); curConfigInd++){
        if(type == config[curConfigInd].paramType){
            LPRINTF("find tree config[curConfigInd].paramType = %d, config[curConfigInd].root = %p", config[curConfigInd].paramType, config[curConfigInd].treeParam.root);

            return &config[curConfigInd].treeParam;
        }
    }
}

void configDtor(){
    for(size_t curConfigParam = 0; curConfigParam < sizeof(config) / sizeof(configParam); curConfigParam++){
        treeDtor(&config[curConfigParam].treeParam);
    }
}

static bool getConfigParameter(const char* configParamName, configParam** curConfigParam){
    assert(configParamName);
    assert(curConfigParam);

    for(size_t curConfigInd = 0; curConfigInd < sizeof(config) / sizeof(configParam); curConfigInd++){
        if(isEqualStrings(configParamName, config[curConfigInd].name)){
            *curConfigParam = &config[curConfigInd];
            
            return true;
        }
    }

    printf("Синтаксическая ошибка\n");
    return false;
}

