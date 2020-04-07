#ifndef __SEMANTEME_H__
#define __SEMANTEME_H__

#include "tree.h"

typedef enum{
    T_EMPTY,
    
    N_Program,
    N_ExtDefList,
    N_ExtDef,
    
    N_Specifier,
    N_StructSpecifier,
    N_OptTag,
    N_Tag,

    N_VarDec,
    N_FunDec,
    N_VarList,
    N_ParamDec,
    
    N_CompSt,
    N_StmtList,
    N_Stmt,

    N_DefList,
    N_Def,
    N_DecList,
    N_Dec,

    N_Exp,
    N_Args,

    T_SEMI,
    T_COMMA,
    T_TYPE,
    T_STRUCT,

    T_LC,
    T_RC,
    T_LB,
    T_RB,
    T_LP,
    T_RP,

    T_INT,
    T_FLOAT,

    T_ID,

    T_RETURN,
    T_WHILE,
    T_IF,
    T_ELSE,

    T_ASSIGNOP,
    T_AND,
    T_OR,
    T_RELOP,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DIV,
    T_NOT,
    T_DOT,

    T_UNKNOWN
}SyntaxType;

void Program(TreeNode *ptr);
void ExtDefList(TreeNode *ptr);
void ExtDef(TreeNode *ptr);

void Specifier(TreeNode *ptr);
void StructSpecifier(TreeNode *ptr);
void OptTag(TreeNode *ptr);
void Tag(TreeNode *ptr);

void VarDec(TreeNode *ptr);
void FunDec(TreeNode *ptr);
void VarList(TreeNode *ptr);
void ParamDec(TreeNode *ptr);

void CompSt(TreeNode *ptr);
void StmtList(TreeNode *ptr);
void Stmt(TreeNode *ptr);

void DefList(TreeNode *ptr);
void Def(TreeNode *ptr);
void DecList(TreeNode *ptr);
void Dec(TreeNode *ptr);

void Exp(TreeNode *ptr);
void Args(TreeNode *ptr);

SyntaxType GetSyntaxType(const char* identifier);

#endif
