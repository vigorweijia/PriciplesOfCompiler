#include <stdio.h>

#include "tree.h"
#include "syntax.tab.h"
#include "semanteme.h"
#include "IRCode.h"
#include "GenMips32.h"

#define PRINT_TREE 1

extern TreeNode* root;
extern int syntaxErrorFlag;
extern int lexicalErrorFlag;
extern void ErrorPrint();

int main(int argc, char** argv) 
{
    //if(argc <= 1) return 1;
    FILE *f = fopen(argv[1], "r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();

    if(syntaxErrorFlag == 0 && lexicalErrorFlag == 0) {
#ifdef PRINT_TREE
#ifdef WDEBUG
        TreeTraverse(root, 0);
        //TreeDestroy(root);
#endif
#endif
        Program(root);
        //InterCodePrint(argv[2]);
        MipsPrint(argv[2]);
    }
    else
    {
        ErrorPrint();
    }
    
    return 0;
}