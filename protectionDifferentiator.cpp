#include "protectionDifferentiator.h"
#include "general/file.h"

#include "general/debug.h"
#define DEBUG

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

static const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDumps/graphDump.dot";
static const char* CONVERT_TO_IMAGE_COMMAND = "dot -Tpng graphDumps/graphDump.dot -o graphDumps/graph%d.png";
const size_t CONVERSION_COMMAND_SIZE = 61;

const char* LOG_FILE_NAME = "logDifferentiator.html";

static size_t logCount = 0;

const char* TREE_BRANCH_COLOR = "#785946ff";
const char* LEAF_COLOR        = "#70c07cff";
const char* LEAF_BORDER_COLOR = "#035068ff";
const char* FONT_COLOR        = "#001effff";
const char* CUR_LEAF_COLOR    = "#f8ff1fff";

const size_t START_SCALE_GRAPH_DUMP = 10;
const size_t SCALE_INCREASE_COEF    = 3;
const size_t SCALE_MAX              = 85;
const size_t SCALE_MIN              = 30;

static void initGraphNodes(const treeNode_t* node, FILE* graphFilePtr);
static void printGraphNode(const treeNode_t* node, FILE* graphFilePtr);
// static void assignErrorStruct(differentiator_t* akinator, treeStatus type);

// treeStatus verifyTree(differentiator_t* tree, const char* function, const char* file, const int line){
//     if(tree == NULL){
//         LPRINTF("tree — нулевой указатель\n");  
//     } 
//     else{
//         if(tree->root == NULL){  
//             assignErrorStruct(tree, TREE_NULL_POINTER);
//             LPRINTF("root — нулевой указатель\n");
//         }

//         else{
//             assignErrorStruct(tree, TREE_PROCESS_OK);
//             return TREE_PROCESS_OK;
//         }
//     }
//     LPRINTF("%s\n", list->status.text);
//     htmlLog(tree, file, function, line, "error", "verification", -1);
//     return tree->status.type;
// }

// static void assignErrorStruct(differentiator_t* tree, treeStatus type){
//     assert(tree);

//     for(size_t curErrInd = 0; curErrInd < sizeof(treeStatuses) / sizeof(treeStatuses); curErrInd++){
//         if(treeStatuses[curErrInd].type == type){
//             tree->status = treeStatuses[curErrInd];
//         }
//     }
// }

void htmlLog(differentiator_t* akinator, const char* callFileName, const char* callFuncName, int callLine,
                           const char* dumpDescription, ...){
    assert(akinator);
    assert(callFileName);
    assert(callFuncName);

    fileDescription logFile = {};
    if (logCount == 0)
        logFile = (fileDescription){ LOG_FILE_NAME, "wb" };
    else
        logFile = (fileDescription){ LOG_FILE_NAME, "ab" };

    FILE* logFilePtr = myOpenFile(&logFile);
    assert(logFilePtr);

    if (logCount == 0)
    {
        fprintf(logFilePtr,
            "<!DOCTYPE html><html lang=\"ru\"><head>"
            "<meta charset=\"utf-8\">"
            "<title>Tree Dump</title>"
            "<style>"
            "body{font-family:'Consolas','Menlo',monospace;background:#f7f9fb;color:#222;"
            "margin:16px;font-size:15px;line-height:1.4;}"
            ".dump{border-left:6px solid #007cad;background:#fff;border-radius:8px;"
            "padding:10px 14px;margin-bottom:20px;box-shadow:0 2px 8px rgba(0,0,0,0.06);}"
            ".header{font-size:1.1rem;margin-bottom:6px;font-weight:bold;color:#007cad;}"
            ".meta{font-size:0.9rem;color:#555;margin-bottom:8px;}"
            "table{border-collapse:collapse;width:auto;min-width:10%%;font-size:0.9rem;}"
            "th,td{border:1px solid #ccc;padding:4px 8px;text-align:left;}"
            "th{background:#eef3f7;font-weight:bold;}"
            "tr:nth-child(even){background:#fafafa;}"
            ".graph{text-align:center;margin-top:10px;}"
            "img{border-radius:6px;max-width:95%%;height:auto;}"
            ".badge{display:inline-block;background:#007cad;color:#fff;"
            "padding:2px 8px;border-radius:4px;font-size:0.85rem;margin-left:6px;}"
            "</style></head><body>\n");
    }
    fprintf(logFilePtr, "<div class=\"dump\">\n");

    fprintf(logFilePtr, "<div class=\"dump-header\">"
                        "<div>"
                        "<h2 style=\"margin:0; font-size:1.8rem;\"> DUMP — <span style=\"margin-left:10px; color:#333;\"> ");

    va_list args;
    va_start(args, dumpDescription);
    vfprintf(logFilePtr,  dumpDescription, args);
    va_end(args);

    fprintf(logFilePtr, "</span>"
                        "</h2>"
                        "</div>");

    fprintf(logFilePtr,          
            "<div class=\"location\">"
            "<b>File:</b> %s<br>"
            "<b>Function:</b> %s<br>"
            "<b>Line:</b> %d"
          "</div>"
        "</div>\n", callFileName, callFuncName, callLine);

    fprintf(logFilePtr, "<table><tbody>"
                        "<tr><th>Root</th><td>%p</td></tr>"
                        "<tr><th>Size</th><td>%lu</td></tr>"
                        "</tbody></table>\n",
                        akinator->root, akinator->size);

    double widthPercent = START_SCALE_GRAPH_DUMP +  (double)akinator->size * SCALE_INCREASE_COEF;
    if (widthPercent > SCALE_MAX) widthPercent = SCALE_MAX;  
    if (widthPercent < SCALE_MIN) widthPercent = SCALE_MIN;  

    // Генерация картинки
    treeGraphDump(akinator);

    fprintf(logFilePtr,
        "<div class=\"graph\">"
        "<a href=\"graphDumps/graph%zu.png\" target=\"_blank\">"
        "<img src=\"graphDumps/graph%zu.png\" style=\"width:%0.2f%%;\" alt=\"tree dump\"></a>"
        "<div class=\"meta-scale\">Scale: width = %.2f%%</div>"
        "</div>\n",
        logCount, logCount, widthPercent, widthPercent);

    fprintf(logFilePtr, "</div>\n");

    fclose(logFilePtr);
}


void treeGraphDump(differentiator_t* akinator){
    assert(akinator);

    logCount++;    

    fileDescription graphDump = {
        GRAPH_DUMP_DOT_FILE_NAME,
        "wb"
    };

    FILE* graphFilePtr = myOpenFile(&graphDump);
    assert(graphFilePtr);

    fprintf(graphFilePtr, "digraph G {\n");
    fprintf(graphFilePtr, "bgcolor=\"transparent\"\n");

    fprintf(graphFilePtr, "nodesep = 1;\n");
    fprintf(graphFilePtr, "ranksep = 1\n");

    fprintf(graphFilePtr, "\tnode [shape=Mrecord, style=\"filled\", fillcolor=\"%s\", fontcolor=\"%s\", color=\"%s\", penwidth=4, fontname=\"Menlo\", fontsize=30];\n\n", LEAF_COLOR, FONT_COLOR, LEAF_BORDER_COLOR);

    initGraphNodes(akinator->root, graphFilePtr);

    printGraphNode(akinator->root, graphFilePtr);

    fprintf(graphFilePtr, "root_label [shape=box, width = 2.4, height = 1.4, label=\"ROOT\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", TREE_BRANCH_COLOR);
    // fprintf(graphFilePtr, "cur_node_label [shape=box, width = 2.4, height = 1.4, label=\"curGuessNode\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", CUR_LEAF_COLOR);
    fprintf(graphFilePtr, "root_label -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", (int)(uintptr_t) akinator->root, TREE_BRANCH_COLOR);
    // fprintf(graphFilePtr, "cur_node_label -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", (int)(uintptr_t) *curNode(akinator), CUR_LEAF_COLOR);

    fprintf(graphFilePtr, "}\n");

    fclose(graphFilePtr);
    
    char convertToImageCommandString[CONVERSION_COMMAND_SIZE];

    sprintf(convertToImageCommandString, CONVERT_TO_IMAGE_COMMAND, logCount);   

    system((const char*) convertToImageCommandString);
}

static void initGraphNodes(const treeNode_t* node, FILE* graphFilePtr){
    assert(node);
    assert(graphFilePtr);

    if(node->nodeType != NUMBER){
        fprintf(graphFilePtr, "\tnode%d [label=\"{type = %d | parent = %p | address = %p | data = %s | {left = %p | right = %p}} \"];\n", 
            (int)(uintptr_t) node,
            node->nodeType,
            node->parent, 
            node, 
            node->data, 
            node->left, 
            node->right);
    }
    else{
        fprintf(graphFilePtr, "\tnode%d [label=\"{type = %d | parent = %p | address = %p | data = %d | {left = %p | right = %p}} \"];\n", 
            (int)(uintptr_t) node,
            node->nodeType,
            node->parent, 
            node, 
            node->data, 
            node->left, 
            node->right);
    }

    if(node->left){
        initGraphNodes(node->left, graphFilePtr);
    }

    if(node->right){
        initGraphNodes(node->right, graphFilePtr);
    }
}

static void printGraphNode(const treeNode_t* node, FILE* graphFilePtr){
    assert(node);
    assert(graphFilePtr);

    
    if(node->left){
        fprintf(graphFilePtr, "node%d ", (int)(uintptr_t) node);
        fprintf(graphFilePtr, "->");
        fprintf(graphFilePtr, "node%d [color=\"%s\", arrowsize=1.5, penwidth=7, weight=1000];\n", (int)(uintptr_t) node->left, TREE_BRANCH_COLOR);
        printGraphNode(node->left, graphFilePtr);
    }

    if(node->right){
        fprintf(graphFilePtr, "node%d", (int)(uintptr_t) node);
        fprintf(graphFilePtr, "->");
        fprintf(graphFilePtr, "node%d [color=\"%s\", arrowsize=1.5, penwidth=7, weight=1000];\n", (int)(uintptr_t) node->right, TREE_BRANCH_COLOR);
        printGraphNode(node->right, graphFilePtr);
    }

    
}
