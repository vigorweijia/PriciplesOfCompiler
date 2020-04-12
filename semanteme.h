#ifndef __SEMANTEME_H__
#define __SEMANTEME_H__

#include "tree.h"
#include "SymbolTable.h"

//#define WDEBUG 1

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
void ExtDecList(TreeNode* ptr, Type type);

Type Specifier(TreeNode *ptr);
void StructSpecifier(TreeNode *ptr, Type type);
void OptTag(TreeNode *ptr, Type type);
void Tag(TreeNode *ptr, Type type);

void VarDec(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);
void FunDec(TreeNode *ptr, Type type);
void VarList(TreeNode *ptr, Type type, Origin origin, Type fundecSpecifier);
void ParamDec(TreeNode *ptr, Type type, Origin origin, Type fundecSpecifier);

void CompSt(TreeNode *ptr, Type rtnType);
void StmtList(TreeNode *ptr, Type rtnType);
void Stmt(TreeNode *ptr, Type rtnType);

void DefList(TreeNode *ptr, Type type, Origin origin);
void Def(TreeNode *ptr, Type type, Origin origin);
void DecList(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);
void Dec(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);

Type Exp(TreeNode *ptr);
void Args(TreeNode *ptr, FieldList param);

int TypeEqual(Type typeA, Type typeB);
int StructTypeEqual(Type typeA, Type typeB);
int ArrayTypeEqual(Type typeA, Type typeB);

void SematicDebug(TreeNode* ptr);
void wDebug(const char* msg);

#endif
