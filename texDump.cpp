#include "texDump.h"
#include "derivative/operations.h"

#include "general/file.h"

#include <stdio.h>
#include <assert.h>

static void texDumpNode(FILE* texFilePtr, treeNode_t* toDump);

const char* texDumpFileName = "DumpDifferentiator.tex";

void texDumpTree(tree_t* expression){
    assert(expression);
    printf("MEOW\n");
    fileDescription texDumpFile{
        texDumpFileName,
        "wb"
    };

    FILE* texFilePtr = myOpenFile(&texDumpFile);
    assert(texFilePtr);

    fprintf(texFilePtr, "\\documentclass[a4paper,12pt]{article}\n"

                        "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,right=1.5cm,marginparwidth=0.75cm]{geometry}\n"

                        "\\usepackage[T2A]{fontenc}\n"
                        "\\usepackage[utf8]{inputenc}\n"
                        "\\usepackage[english,russian]{babel}\n");

    fprintf(texFilePtr, "\n\\newcommand{\\plus}[2]{#1 + #2}\n"
                        "\\newcommand{\\minus}[2]{#1 - #2}\n"
                        "\\newcommand{\\mul}[2]{#1 \\cdot #2}\n");

    fprintf(texFilePtr, "\\begin{document}\n");
    
    fprintf(texFilePtr, "\\[\n");

    texDumpNode(texFilePtr, expression->root);
    fprintf(texFilePtr, "\n");

    fprintf(texFilePtr, "\\]\n");

    fprintf(texFilePtr, "\\end{document}\n");
}

static void texDumpNode(FILE* texFilePtr, treeNode_t* toDump){

    if(toDump->type == NUMBER){
        fprintf(texFilePtr, "%d", toDump->data.num);
    }
    else if(toDump->type == VARIABLE){
        fprintf(texFilePtr, "%s", toDump->data.var);
    }
    else if(toDump->type == OPERATOR){
        operation_t curOp = getCurrentOperation(toDump->data.op);

        fprintf(texFilePtr, "%s", curOp.texCode);

        if(toDump->left){
            fprintf(texFilePtr, "{");

            texDumpNode(texFilePtr, toDump->left);

            fprintf(texFilePtr, "}");
        }



        if(toDump->right){
            fprintf(texFilePtr, "{");

            texDumpNode(texFilePtr, toDump->right);

            fprintf(texFilePtr, "}");
        }
    }
    

}