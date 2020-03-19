#include <stdio.h>

#include "tree.h"
#include "syntax.tab.h"

#define PRINT_TREE 1

extern TreeNode* root;
extern int syntaxErrorFlag;
extern int lexicalErrorFlag;

int main(int argc, char** argv) 
{
    if(argc <= 1) return 1;
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
        TreeTraverse(root, 0);
        TreeDestroy(root);
#endif
    }
    return 0;
}