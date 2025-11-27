#include "texDump.h"
#include "operations.h"

#include "general/file.h"

#include <stdio.h>
#include <assert.h>

static void texDumpNode(FILE* texFilePtr, treeNode_t* toDump);

const char* texDumpFileName = "DumpDifferentiator.tex";

void texDumpTree(expression_t* expression){
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

    fprintf(texFilePtr, "\\begin{document}\n");
    
    texDumpNode(texFilePtr, expression->root);
    fprintf(texFilePtr, "\n");

    fprintf(texFilePtr, "\\end{document}\n");
}

static void texDumpNode(FILE* texFilePtr, treeNode_t* toDump){
    assert(toDump);

    if(toDump->type == NUMBER){
        fprintf(texFilePtr, "%d", toDump->data.num);
    }
    else if(toDump->type == VARIABLE){
        fprintf(texFilePtr, "%s", toDump->data.var);
    }
    else if(toDump->type == OPERATOR){
        fprintf(texFilePtr, "(");
        texDumpNode(texFilePtr, toDump->left);
        fprintf(texFilePtr, "%s", toDump->data.op);
        texDumpNode(texFilePtr, toDump->right);
        fprintf(texFilePtr, ")");
    }
    

}