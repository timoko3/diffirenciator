#include "operations.h"

#include "../general/strFunc.h"

operation_t getCurrentOperation(char* curOpStringName){
    for(size_t curOpInd = 0; curOpInd < sizeof(operations) / sizeof(operation_t); curOpInd++){
        if(isEqualStrings(curOpStringName, operations[curOpInd].nameString)){
            return operations[curOpInd];
        }
    }
}