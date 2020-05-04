#ifndef __SEMANTEME_H__
#define __SEMANTEME_H__

#include "tree.h"
#include "SymbolTable.h"
#include "IRCode.h"

//#define WDEBUG 1

void Program(TreeNode *ptr);
void ExtDefList(TreeNode *ptr);
void ExtDef(TreeNode *ptr);
void ExtDecList(TreeNode* ptr, Type type);

Type Specifier(TreeNode *ptr);
void StructSpecifier(TreeNode *ptr, Type type);
void OptTag(TreeNode *ptr, Type type);
void Tag(TreeNode *ptr, Type type);

void VarDec(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);
Type FunDec(TreeNode *ptr, Type type);
void VarList(TreeNode *ptr, Type type, Origin origin, Type fundecSpecifier);
void ParamDec(TreeNode *ptr, Type type, Origin origin, Type fundecSpecifier);

void CompSt(TreeNode *ptr, Type rtnType);
void StmtList(TreeNode *ptr, Type rtnType);
void Stmt(TreeNode *ptr, Type rtnType);

void DefList(TreeNode *ptr, Type type, Origin origin);
void Def(TreeNode *ptr, Type type, Origin origin);
void DecList(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);
void Dec(TreeNode *ptr, Type type, Origin origin, Type structSpecifier);

Type Exp(TreeNode *ptr, Operand place);
Type ExpCond(TreeNode *ptr, Operand trueLabel, Operand falseLabel);
void Args(TreeNode *ptr, FieldList param, Operand args);

int TypeEqual(Type typeA, Type typeB);
int StructTypeEqual(Type typeA, Type typeB);
int ArrayTypeEqual(Type typeA, Type typeB);

void SematicDebug(TreeNode* ptr);
void wDebug(const char* msg);

#endif
