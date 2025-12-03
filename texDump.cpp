#include "texDump.h"
#include "derivative/operations.h"

#define DEBUG

#include "general/file.h"
#include "general/debug.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

static void defineMacros(FILE* texFilePtr);
static void texDumpNode(FILE* texFilePtr, treeNode_t* node);

const char* texDumpFileName = "DumpDifferentiator.tex";

static void dumpRandomCleverMatanPhrase(FILE* texFilePtr);

static void generateGraphic(FILE* texFilePtr, tree_t* expression, tree_t* scaleGraphic);
static void dumpExpressionForGraphic(FILE* texFilePtr, treeNode_t* node);

static bool needBracketsOp(treeNode_t* node, operation_t curOp);

const char* cleverMatanPhrases[] = {
    "Обладая базовыми знаниями матеметики нетрудно заметить, что...\n",
    "Объяснять не буду т.к. если вам это не очевидно, значит вам пора перестать заниматься математикой.\n",
    "Здесь все тривиальнейшим образом сокращается\n",
    "Данные преобразования поистенне чудесны, но у меня не хватает свободного пространство, чтобы показать это. Пусть это останется несложным упражнением для читателя.\n",
    "Т.к. 1+0=1, следовательно 0+1=1. Дальнейшие преобразования элементарны, поэтому не буду утруждать вас их чтением\n",
    "константы схлопываются, отвыты получаются, кафедра матана беснуется.\n",
    "Впахивают роботы, а не человек...Тут тоже уходит пару слагаемых.\n",
    "Для того, чтобы точно понять данное преобразование советую обратиться к пособию Саблезубова Петра Ивановича к тому 5 сочинений по теме \"1000 способов вскрыть черепную коробку при помощи интеграла\" страница 666 3 абзац формула (17.2) для точного ее понимаю желательно прочитать предыдущие 3 параграфа\n"
};

void texDumpTree(tree_t* expression, FILE* texFilePtr, bool isTailorTree){
    assert(expression);

    static size_t callsTexDump = 0;
    callsTexDump++;

    bool openFileHere = false;
    if(!texFilePtr){
        openFileHere = true;
        if(callsTexDump == 1){
            fileDescription texDumpFile{
                texDumpFileName,
                "wb"
            };

            texFilePtr = myOpenFile(&texDumpFile);
            assert(texFilePtr);

            fprintf(texFilePtr, "\\documentclass[a4paper,12pt]{article}\n"

                                "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,right=1.5cm,marginparwidth=0.75cm]{geometry}\n"

                                "\\usepackage[T2A]{fontenc}\n"
                                "\\usepackage[utf8]{inputenc}\n"
                                "\\usepackage[english,russian]{babel}\n"
                                "\\usepackage{graphicx}\n"
                                "\\usepackage{breqn}"
                                "\\usepackage{pgfplots}\n\n");

            fprintf(texFilePtr, "\\pgfplotsset{compat=1.18}\n");

            defineMacros(texFilePtr);

            fprintf(texFilePtr, "\\begin{document}\n");

            fprintf(texFilePtr, "\\thispagestyle{empty}\n"
                                "\\newgeometry{left=0cm,right=0cm,top=0cm,bottom=0cm}\n");

            fprintf(texFilePtr, "\\begin{figure}\n"
                                "\\centering\n"
                                "\\includegraphics[width=\\paperwidth,height=\\paperheight,keepaspectratio]{matan.jpg}\n"
                                "\\caption{Параметры системы во время исполнения теста}\n"
                                "\\end{figure}\n");

            fprintf(texFilePtr, "\\restoregeometry\n");

            fprintf(texFilePtr, "\\section{Предостережения}\n");
            fprintf(texFilePtr, "\\begin{itemize}\n");
            fprintf(texFilePtr, "\\item (ГЛАВНАЯ АКСИОМА МАТАНА) Помни, что если не продифференцируешь ты, то продиффиринцируют тебя!!\n");
            fprintf(texFilePtr, "\\item Никаких действительных чисел! Мне хватило сечений Дедекинда. Ради вашего же блага прошу вводить только целые. При попытке использовать другие числа я не могу ручаться за сохранность французского на вашем компьютере!!!\n");
            fprintf(texFilePtr, "\\end{itemize}\n");

            fprintf(texFilePtr, "\\section{Постановка задачи}\n");
            fprintf(texFilePtr, "Возьмем производную данного выражения:\n");

            fprintf(texFilePtr, "\\begin{dmath}\n y(x)=");
            texDumpNode(texFilePtr, expression->root);
            fprintf(texFilePtr, "\\end{dmath}\n");
            
            fclose(texFilePtr);

            return;
        }
        else{
            fileDescription texDumpFile{
                texDumpFileName,
                "ab"
            };

            texFilePtr = myOpenFile(&texDumpFile);
            assert(texFilePtr);
        }
    }

    LPRINTF("texFilePtr: %p, expression->root: %p", texFilePtr, expression->root);
    
    if(callsTexDump == 2){
        fprintf(texFilePtr, "\\section{Непосредственно (прости господи) дифференцирование и элементарное упрощение выражения}\n");
    }

    if((callsTexDump % 2) == 0){
        dumpRandomCleverMatanPhrase(texFilePtr);
    }

    fprintf(texFilePtr, "\\begin{dmath}\n y'(x)=");
    texDumpNode(texFilePtr, expression->root);
    if(isTailorTree) fprintf(texFilePtr, "+o(x^%lu)\n", PRECISION_TERM_TAILOR);
    fprintf(texFilePtr, "\\end{dmath}\n");

    if(openFileHere){
        fclose(texFilePtr);
    }
}

void endTexFile(tree_t* expression, tree_t* scaleGraphic){
    assert(expression);
    assert(scaleGraphic);

    fileDescription texDumpFile{
        texDumpFileName,
        "ab"
    };

    FILE* texFilePtr = myOpenFile(&texDumpFile);
    assert(texFilePtr);

    fprintf(texFilePtr, "\\section{Результат вычислений:}\n");
    fprintf(texFilePtr, "Вот мы и получили результат, который с легкостью получал любой пятиклассник в СССР быстрее данной программы. Живите с этим.\n");

    fprintf(texFilePtr, "\\begin{dmath}\ny'(x)=");
    texDumpNode(texFilePtr, expression->root);
    fprintf(texFilePtr, "\\end{dmath}\n");

    generateGraphic(texFilePtr, expression, scaleGraphic);

    fprintf(texFilePtr, "\\section{P.S}");
    fprintf(texFilePtr, "Уважаемая кафедра высшей математики не принимайте всерьез данную работу. Автор на самом деле очень любит матан. Все персонажи вымышлены(почти) и ни один учебник математики не пострадал.");

    fprintf(texFilePtr, "\\end{document}\n");

    fclose(texFilePtr);
}

static void defineMacros(FILE* texFilePtr){
    assert(texFilePtr);

    fprintf(texFilePtr, "\n");

    for(size_t curOpInd = 0; curOpInd < sizeof(operations) / sizeof(operation_t); curOpInd++){
        if(!operations[curOpInd].texExists){
            switch(operations[curOpInd].paramCount){
                case 1: fprintf(texFilePtr, "\\newcommand{%s}[%lu]{%s(#1)}\n", operations[curOpInd].texCode, operations[curOpInd].paramCount, operations[curOpInd].nameString); break;
                case 2: fprintf(texFilePtr, "\\newcommand{%s}[%lu]{#1 %s #2}\n", operations[curOpInd].texCode, operations[curOpInd].paramCount, operations[curOpInd].nameString); break;
                default: break;
            }
        }
    }
}

static void texDumpNode(FILE* texFilePtr, treeNode_t* node){

    if(node->type == NUMBER){
        fprintf(texFilePtr, "%d", node->data.num);
    }
    else if(node->type == VARIABLE){
        fprintf(texFilePtr, "%s", node->data.var);
    }
    else if(node->type == OPERATOR){
        operation_t curOp = *getCurrentOperation(node->data.op);

        fprintf(texFilePtr, "%s", curOp.texCode);

        if(node->left){
            fprintf(texFilePtr, "{");

            if(needBracketsOp(node, curOp))fprintf(texFilePtr, "(");

            texDumpNode(texFilePtr, node->left);

            if(needBracketsOp(node, curOp)) fprintf(texFilePtr, ")");
            fprintf(texFilePtr, "}");
        }



        if(node->right){
            fprintf(texFilePtr, "{");
            if(needBracketsOp(node, curOp)) fprintf(texFilePtr, "(");

            texDumpNode(texFilePtr, node->right);

            if(needBracketsOp(node, curOp)) fprintf(texFilePtr, ")");
            fprintf(texFilePtr, "}");
        }
    }
    
}

static void dumpRandomCleverMatanPhrase(FILE* texFilePtr){
    assert(texFilePtr);

    int curPhraseInd = rand() % (int) (sizeof(cleverMatanPhrases) / sizeof(char*));

    fprintf(texFilePtr, "%s\n", cleverMatanPhrases[curPhraseInd]);
}

static void generateGraphic(FILE* texFilePtr, tree_t* expression, tree_t* scaleGraphic){
    assert(expression);
    assert(texFilePtr);
    assert(scaleGraphic);

    fprintf(texFilePtr, "\\section{Теперь, чтобы все стало совсем понятно(ахахахахаахахаххаха).\\\\Построим график полученной производной исходной функции}\n");

    fprintf(texFilePtr, "\\begin{center}\n");

    fprintf(texFilePtr, "\\begin{tikzpicture}\n"
                        "\\begin{axis}[\n"
                        "width=16cm,\n"
                        "height=8cm,\n"
                        "domain=%d:%d,\n"
                        "samples=200,\n"
                        "axis lines=middle,\n"
                        "xlabel={$x$},\n"
                        "ylabel={$y$},\n"
                        "grid=both]\n", 
                        scaleGraphic->root->left->data.num, 
                        scaleGraphic->root->right->data.num);

    fprintf(texFilePtr, "\\addplot[thick, red] {");
    dumpExpressionForGraphic(texFilePtr, expression->root);
    fprintf(texFilePtr, "};\n");

    fprintf(texFilePtr, "\\end{axis}\n"
                        "\\end{tikzpicture}\n");
    fprintf(texFilePtr, "\\end{center}\n\n");
}

static void dumpExpressionForGraphic(FILE* texFilePtr, treeNode_t* node){
    assert(texFilePtr);
    assert(node);

    if(node->type == NUMBER){
        fprintf(texFilePtr, "%d", node->data.num);
    }
    else if(node->type == VARIABLE){
        fprintf(texFilePtr, "%s", node->data.var);
    }
    else if(node->type == OPERATOR){
        operation_t curOp = *getCurrentOperation(node->data.op);

        if(curOp.paramCount == 1){
            fprintf(texFilePtr, "%s", curOp.nameString);
        }

        if(node->left){
            fprintf(texFilePtr, "(");

            dumpExpressionForGraphic(texFilePtr, node->left);

            fprintf(texFilePtr, ")");
        }

        if(curOp.paramCount == 2){
            fprintf(texFilePtr, "%s", curOp.nameString);
        }

        if(node->right){
            fprintf(texFilePtr, "(");

            dumpExpressionForGraphic(texFilePtr, node->right);

            fprintf(texFilePtr, ")");
        }
    }
}

void startTexDumpTailor(){
    fileDescription texDumpFile{
        texDumpFileName,
        "ab"
    };

    FILE* texFilePtr = myOpenFile(&texDumpFile);
    assert(texFilePtr);

    fprintf(texFilePtr, "\\section{И снова Тейлор ...- - -...}\n");
    fprintf(texFilePtr, "Расчленим исходную функцию до $o(x^%lu)$.\n", PRECISION_TERM_TAILOR);

    fclose(texFilePtr);
}

static bool needBracketsOp(treeNode_t* node, operation_t curOp){
    assert(node);


    if(isEqualStrings(curOp.nameString, "*") && (node->left->type != OPERATOR || node->right->type != OPERATOR)){
        return false;
    }
    else{
        if(curOp.texNeedBrackets){
            return true;
        }
        else{
            return false;
        }
    }

    return false;
}