#include "texDump.h"

#include "general/file.h"

#include <stdio.h>
#include <assert.h>

const char* texDumpFileName = "DumpDifferentiator.tex";

void texDumpTree(differentiator_t* differentiator){
    assert(differentiator);

    fileDescription texDumpFile{
        "wb",
        texDumpFileName
    };

    FILE* texFilePtr = myOpenFile(&texDumpFile);
    assert(texFilePtr);

    fprintf(texFilePtr, "\\begin{document}");

    fprintf(texFilePtr, "\\end{document}");
}
