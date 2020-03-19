%{
    #include <stdio.h>
    #include "syntax.tab.h"
    #include "tree.h"
    //#define DEBUG 1

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

    static void CreateNode(TreeNodeType type, const char* identifier)
    {
        yylval.tree_node = CreateTreeNode(type, yylineno, identifier, yytext);
    }

    static void Debug(const char* msg) 
    {
        #ifdef DEBUG
            printf("%s\n",msg);
        #endif
    }

    int lexicalErrorFlag = 0;
%}

%option yylineno

DIGIT [0-9]
LETTER [_a-zA-Z]

DELIM [ \t\r]
WS {DELIM}+

INT 0|[1-9][0-9]*
FLOAT {INT}(\.{DIGIT}+)?
ID {LETTER}({LETTER}|{DIGIT})*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|==|>=|<=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

%%

"//" {Debug("Comments");
    char c = input();
    while(c != '\n') c = input();
    }

"/*"(([^\*]*(\*[^\/])?)*)"*/" {Debug("Comments");}

{SEMI} {Debug("SEMI");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "SEMI", yytext);
    return SEMI;
    }

{COMMA} {Debug("COMMA");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "COMMA", yytext);
    return COMMA;
    }

{ASSIGNOP} {Debug("ASSIGNOP");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "ASSIGNOP", yytext);
    return ASSIGNOP;
    }

{RELOP} {Debug("RELOP");
    yylval.tree_node = CreateTreeNode(TYPE_STR, yylineno, "RELOP", yytext);
    return RELOP;
    }

{PLUS} {Debug("PLUS");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "PLUS", yytext);
    return PLUS;
    }

{MINUS} {Debug("MINUS");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "MINUS", yytext);
    return MINUS;
    }

{STAR} {Debug("STAR");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "STAR", yytext);
    return STAR;
    }

{DIV} {Debug("DIV");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "DIV", yytext);
    return DIV;
    }

{AND} {Debug("AND");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "AND", yytext);
    return AND;
    }

{OR} {Debug("OR");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "OR", yytext);
    return OR;
    }

{DOT} {Debug("DOT");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "DOT", yytext);
    return DOT;
    }

{NOT} {Debug("NOT");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "NOT", yytext);
    return NOT;
    }

{TYPE} {Debug("TYPE");
    yylval.tree_node = CreateTreeNode(TYPE_STR, yylineno, "TYPE", yytext);
    return TYPE;
    }

{LP} {Debug("LP");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "LP", yytext);
    return LP;
    }

{RP} {Debug("RP");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "RP", yytext);
    return RP;
    }

{LB} {Debug("LB");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "LB", yytext);
    return LB;
    }

{RB} {Debug("RB");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "RB", yytext);
    return RB;
    }

{LC} {Debug("LC");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "LC", yytext);
    return LC;
    }

{RC} {Debug("RC");
    yylval.tree_node = CreateTreeNode(TYPE_OTHER_TERMINAL, yylineno, "RC", yytext);
    return RC;
    }

{STRUCT} {Debug("STRUCT");
    CreateNode(TYPE_OTHER_TERMINAL, "STRUCT");
    return STRUCT;
    }

{RETURN} {Debug("RETURN");
    CreateNode(TYPE_OTHER_TERMINAL, "RETURN");
    return RETURN;
    }

{IF} {Debug("IF");
    CreateNode(TYPE_OTHER_TERMINAL, "IF");
    return IF;
    }

{ELSE} {Debug("ELSE");
    CreateNode(TYPE_OTHER_TERMINAL, "ELSE");
    return ELSE;
    }

{WHILE} {Debug("WHILE");
    CreateNode(TYPE_OTHER_TERMINAL, "WHILE");
    return WHILE;
    }

{INT} {Debug("INT");
    CreateNode(TYPE_INT, "INT");
    return INT;
    }

{FLOAT} {Debug("FLOAT");
    CreateNode(TYPE_FLOAT, "FLOAT");
    return FLOAT;
    }

{ID} {Debug("ID");
    CreateNode(TYPE_ID, "ID");
    return ID;
    }

"\n" {yycolumn = 1;}

<<EOF>> {yyterminate();}

{WS} {}

. { 
    printf("Error type A at line %d: Mysterious characters \'%s\' \n",yylineno,yytext);
    lexicalErrorFlag = 1;
    }

%%