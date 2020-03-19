%{
    #include <stdio.h>
    #include <string.h>

    #include "tree.h"
    #include "lex.yy.c"

    #define LENTH 128

    const char* defaultErr = "Syntax Error";

    TreeNode* root;

    int syntaxErrorFlag = 0;

    typedef enum{
        ERROR_COMMON = 0,
        ERROR_DETAIL = 1
    }ErrorType;

    typedef struct ErrorMsg {
        ErrorType errorType;
        int lineno;
        char msg[LENTH];
    }ErrorMsg;

    ErrorMsg errorMsgList[LENTH];
    int errorCnt = 0;

    void ErrorProcess(int lineno, const char* msg)
    {
        for(int i = 0; i < errorCnt; i++)
        {
            if(errorMsgList[i].lineno == lineno)
            {
                strcpy(errorMsgList[i].msg, msg);
                errorMsgList[i].errorType = ERROR_DETAIL;
            }
        }
    }

    void ErrorPrint() 
    {
        for(int i = 0; i < errorCnt; i++)
        {
            if(errorMsgList[i].errorType == ERROR_DETAIL) printf("Error type B at Line %d: %s.\n",errorMsgList[i].lineno,errorMsgList[i].msg);
            else printf("Error type B at Line %d: %s.\n",errorMsgList[i].lineno,defaultErr);
        }
    }
%}

/*declared types*/
%union {
    int type_int;
    float type_float;
    TreeNode* tree_node;
}

/*declared tokens*/
%token <tree_node> SEMI
%token <tree_node> COMMA
%token <tree_node> ASSIGNOP
%token <tree_node> RELOP
%token <tree_node> PLUS MINUS STAR DIV
%token <tree_node> AND OR NOT
%token <tree_node> DOT
%token <tree_node> TYPE
%token <tree_node> LP RP //( )
%token <tree_node> LB RB //[ ]
%token <tree_node> LC RC //{ }
%token <tree_node> STRUCT
%token <tree_node> RETURN
%token <tree_node> IF ELSE
%token <tree_node> WHILE
%token <tree_node> INT
%token <tree_node> FLOAT
%token <tree_node> ID

%type <tree_node> Program ExtDefList ExtDef ExtDecList
%type <tree_node> Specifier StructSpecifier OptTag DefList Tag
%type <tree_node> VarDec FunDec VarList ParamDec
%type <tree_node> CompSt StmtList Stmt
%type <tree_node> Def DecList Dec
%type <tree_node> Exp Args

%right ASSIGNOP
%left OR
%left AND
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB
%left DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/*High-level Definitions*/
Program : ExtDefList { 
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Program", "");
        root = $$;
        TreeInsert($$, $1);
    }
    ;
ExtDefList : ExtDef ExtDefList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDefList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | /* empty */ {$$ = NULL;}
    ;
ExtDef : Specifier ExtDecList SEMI {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDef", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Specifier SEMI {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDef", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | Specifier FunDec CompSt {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDef", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | error SEMI {
        ErrorProcess(yylineno, defaultErr);
    }
    //| Specifier ExtDecList error {
    //    ErrorProcess(@3.first_line, "Missing \';\'");
    //}
    //| Specifier error {
    //    ErrorProcess(@2.first_line, "Missing \';\'");
    //}
    //| Specifier error SEMI {
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    //| Specifier error CompSt{
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    ;
ExtDecList : VarDec {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDecList", "");
        TreeInsert($$, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ExtDecList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    //| VarDec error ExtDecList {
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    ;

/*Specifier*/
Specifier : TYPE {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Specifier", "");
        TreeInsert($$, $1);
    }
    | StructSpecifier {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Specifier", "");
        TreeInsert($$, $1);
    }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "StructSpecifier", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
        TreeInsert($$, $5);
    }
    | STRUCT Tag {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "StructSpecifier", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    //| STRUCT OptTag LC error RC {
    //    ErrorProcess(@4.first_line, defaultErr);
    //}
    //| STRUCT OptTag LC DefList error {
    //    ErrorProcess(@4.first_line, "Missing \'{\'");
    //}
    ;
OptTag : ID {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "OptTag", "");
        TreeInsert($$, $1);
    }
    | /* empty */ {$$ = NULL;}
    ;
Tag : ID {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Tag", "");
        TreeInsert($$, $1);
    }
    ;

/*Declarators*/
VarDec : ID {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "VarDec", "");
        TreeInsert($$, $1);
    }
    | VarDec LB INT RB {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "VarDec", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
    }
    //| VarDec LB error RB {
    //    ErrorProcess(@3.first_line, "Content in \"[]\" must be INT");
    //}
    //| VarDec LB INT error {
    //    ErrorProcess(@3.first_line, "Missing \']\'");
    //}
    ;
FunDec : ID LP VarList RP {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "FunDec", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
    }
    | ID LP RP {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "FunDec", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | error RP {
        ErrorProcess(yylineno, defaultErr);
    }
    //| ID LP VarList error {
    //    ErrorProcess(@4.first_line, "Missing \'(\'");
    //}
    ;
VarList : ParamDec COMMA VarList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "VarList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | ParamDec {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "VarList", "");
        TreeInsert($$, $1);
    }
    ;
ParamDec : Specifier VarDec {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "ParamDec", "");
        TreeInsert($$, $1);
    }
    ;

/*Statements*/
CompSt : LC DefList StmtList RC {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "CompSt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
    }
    | error RC {
        ErrorProcess(yylineno, defaultErr);
    }
    //| LC DefList StmtList error {
    //    ErrorProcess(@2.first_line, "Missing \'}\'");
    //}
    ;
StmtList : Stmt StmtList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "StmtList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | /* empty */ {$$ = NULL;}
    ;
Stmt : Exp SEMI {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | CompSt {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
    }
    | RETURN Exp SEMI {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
        TreeInsert($$, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
        TreeInsert($$, $5);
        TreeInsert($$, $6);
        TreeInsert($$, $7);
    }
    | WHILE LP Exp RP Stmt {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Stmt", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
        TreeInsert($$, $5);
    }
    //| Exp error SEMI{
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    | error SEMI {
        ErrorProcess(yylineno, defaultErr);
    }
    //| RETURN Exp error {
    //    ErrorProcess(@3.first_line, "Missing \';\'");
    //}
    //| IF LP error RP Stmt %prec LOWER_THAN_ELSE {
    //    ErrorProcess(@3.first_line, defaultErr);
    //}
    //| IF LP Exp error Stmt %prec LOWER_THAN_ELSE {
    //    ErrorProcess(@4.first_line, "Missing \')\'");
    //}
    //| IF LP error RP Stmt ELSE Stmt {
    //    ErrorProcess(@3.first_line, defaultErr);
    //}
    //| WHILE LP error RP Stmt {
    //    ErrorProcess(@3.first_line, defaultErr);
    //}
    //| WHILE LP Exp error Stmt {
    //    ErrorProcess(@4.first_line, "Missing \')\'");
    //}
    ;

/*Local Definitions*/
DefList : Def DefList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "DefList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | /* empty */ {$$ = NULL;}
    ;
Def : Specifier DecList SEMI {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Def", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    //| Specifier error SEMI {
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    //| Specifier DecList error {
    //    ErrorProcess(@3.first_line, "Missing \';\'");
    //}
    ;
DecList : Dec {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "DecList", "");
        TreeInsert($$, $1);
    }
    | Dec COMMA DecList {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "DecList", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    ;
Dec : VarDec {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Dec", "");
        TreeInsert($$, $1);
    }
    | VarDec ASSIGNOP Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Dec", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    ;

/*Expression*/
Exp : Exp ASSIGNOP Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp AND Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp OR Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp RELOP Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp PLUS Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp MINUS Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp STAR Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp DIV Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | LP Exp RP {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | MINUS Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | NOT Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
    }
    | ID LP Args RP {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
    }
    | ID LP RP {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp LB Exp RB {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
        TreeInsert($$, $4);
    }
    | Exp DOT ID {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | ID {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
    }
    | INT {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
    }
    | FLOAT {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
    }
    //| LP error RP {
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    //| ID LP error RP {
    //    ErrorProcess(@3.first_line, defaultErr);
    //}
    //| Exp LB error RB {
    //    ErrorProcess(@2.first_line, defaultErr);
    //}
    ;
Args : Exp COMMA Args {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Args", "");
        TreeInsert($$, $1);
        TreeInsert($$, $2);
        TreeInsert($$, $3);
    }
    | Exp {
        $$ = CreateTreeNode(TYPE_NONTERMINAL, @$.first_line, "Exp", "");
        TreeInsert($$, $1);
    }
    ;
%%

int yyerror(char *msg) {
    strcpy(errorMsgList[errorCnt].msg, msg);
    errorMsgList[errorCnt].lineno = yylineno;
    errorMsgList[errorCnt].errorType = ERROR_COMMON;
    errorCnt++;
    syntaxErrorFlag = 1;
    return 0;
}
