#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "semanteme.h"

//High-level Definitions
void Program(TreeNode *ptr)
{
    TreeNode* child = ptr->firstChild;
    if(child == NULL) return;
    else if(strcmp(child->m_identifier, "ExtDefList") == 0) ExtDefList(child);
    else assert(0);
}

void ExtDefList(TreeNode *ptr)
{
    TreeNode* child = ptr->firstChild;

    if(child == NULL) 
    {
        //ExtDefList -> epsilon
        return;
    }
    else if(strcmp(child->m_identifier, "ExtDef") == 0)
    {
        //ExtDefList -> ExtDef ExtDefList
        TreeNode* nextChild = child->nextSibling;
        ExtDef(child);
        ExtDefList(nextChild);
    }
    else assert(0);
}

void ExtDef(TreeNode *ptr)
{
    assert(ptr != NULL);

    //ExtDef -> Specifier ...
    ptr = ptr->firstChild;
    Type type = Specifier(ptr);

    TreeNode* child = ptr->nextSibling;
    if(strcmp(child->m_identifier, "ExtDecList") == 0)
    {
        //ExtDef -> Specifier ExtDecList SEMI
        ExtDecList(child, type);
    }
    else if(strcmp(child->m_identifier, "SEMI") == 0)
    {
        //ExtDef -> Specifiter SEMI
        return;
    }
    else if(strcmp(child->m_identifier, "FunDec") == 0)
    {
        //ExtDef -> Specifier FunDec CompSt
        Type functionType = (Type)malloc(sizeof(Type_));
        functionType->kind = FUNCTION;
        functionType->function.rtnType = type;
        FunDec(child, functionType);
        CompSt(child->nextSibling, functionType->function.rtnType);
    }
    else
    {
        assert(0);
    }
}

void ExtDecList(TreeNode* ptr, Type type)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"VarDec") == 0)
    {
        VarDec(child, type, O_ExtDecList);
        TreeNode* nextChild = child->nextSibling;
        if(nextChild == NULL)
        {
            //ExtDecList -> VarDec
            return;
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            //ExtDecList -> VarDec COMMA ExtDecList
            nextChild = nextChild->nextSibling;
            ExtDecList(nextChild, type);
        }
        else
        {
            assert(0);
        } 
    }
    else
    {
        assert(0);
    }
}

Type Specifier(TreeNode* ptr)
{
    Type type = (Type)malloc(sizeof(Type_));
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"TYPE") == 0)
    {
        type->kind = BASIC;
        if(strcmp(child->idName,"int") == 0)
        {
            type->basic = B_INT;
        }
        else if(strcmp(child->idName,"float") == 0)
        {
            type->basic = B_FLOAT;
        }
        else 
        {
            assert(0);
        }
    }
    else if(strcmp(child->m_identifier,"StructSpecifier") == 0)
    {
        type->kind = STRUCTURE;
        StructSpecifier(child,type);
    }
    else 
    {
        assert(0);
    }
    return type;
}

void StructSpecifier(TreeNode* ptr, Type type)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"STRUCT") != 0)
    {
        assert(0);
    }

    child = child->nextSibling;
    if(strcmp(child->m_identifier,"OptTag") == 0)
    {
        //StructSpecifier -> STRUCT OptTag LC DefList RC
        TreeNode* childDefList = child->nextSibling->nextSibling;
        OptTag(child, type);
        DefList(childDefList, type, O_StructSpecifier);
    }
    else if(strcmp(child->m_identifier,"Tag") == 0)
    {
        //StructSpecifier -> STRUCT Tag
        Tag(child, type);
    }
    else 
    {
        assert(0);
    }
}

void OptTag(TreeNode* ptr, Type type)
{
    TreeNode* child = ptr->firstChild;
    if(child == NULL)
    {
        //OptTag -> epsilon
        return;
    }
    else if(strcmp(child->m_identifier, "ID") == 0)
    {
        Symbol symbol = HashTableFind(child->m_identifier);
        if(symbol == NULL)
        {
            if(HashTableInsert(child->idName,type) == 0)
            {
                printf("Error type 16 at Line %d: Insert Failed!.\n",child->m_lineno);
            }
        }
        else
        {
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",child->m_lineno,child->idName);
        }
    } 
    else
    {
        assert(0);
    }
}

void Tag(TreeNode* ptr, Type type)
{
    ptr = ptr->firstChild;
    if(strcmp(ptr->m_identifier, "ID") == 0)
    {
        Symbol symbol = HashTableFind(ptr->idName);
        if(symbol != NULL)//define a STRUCT variable
        {
            assert(symbol->type->kind == STRUCTURE);
            *type = *symbol->type;
        }
        else 
        {
            printf("Error type 17 at Line %d: Undefine structure \"%s\".\n",ptr->m_lineno,ptr->idName);
        }
    }
    else
    {
        assert(0);
    }
}

void VarDec(TreeNode* ptr, Type type, Origin origin)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"ID") == 0)
    {
        //VarDec -> ID
        switch (origin)
        {
        case O_ExtDecList:
            if(HashTableInsert(child->idName,type) == 0)
            {
                printf("Error type 3 at Line %d: (E)Redefined variable \"%s\".\n",child->m_lineno,child->idName);
            }
            break;
        case O_StructSpecifier:
            assert(0);
            break;
        case O_FunDec:
            assert(0);
            break;
        default:
            assert(0);
            break;
        }
    }
    else if(strcmp(child->m_identifier,"VarDec") == 0)
    {
        //VarDec -> VarDec LB INT RB
        switch (origin)
        {
        case O_ExtDecList:
            {
                Type arrayType = (Type)malloc(sizeof(Type_));
                arrayType->kind = ARRAY;
                arrayType->array.elem = type;
                arrayType->array.size = child->nextSibling->nextSibling->intValue;
                VarDec(child,arrayType,origin);
            }
            break;
        case O_StructSpecifier:
            break;
        case O_FunDec:
            break;
        default:
            assert(0);
            break;
        }
    }
    else
    {
        assert(0);
    }
}

void FunDec(TreeNode* ptr, Type type)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling->nextSibling;
    if(strcmp(nextChild->m_identifier,"VarList") == 0)
    {
        //FunDec -> ID LP VarList RP
        type->function.params = NULL;
        type->function.cnt = 0;
        VarList(nextChild, type);
        if(HashTableInsert(child->idName, type) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",child->m_lineno,child->idName);
        }
    }
    else if(strcmp(nextChild->m_identifier,"RP") == 0)
    {
        //FunDec -> ID LP RP
        type->function.params = NULL;
        type->function.cnt = 0;
        if(HashTableInsert(child->idName, type) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",child->m_lineno,child->idName);
        }
    }
    else
    {
        assert(0);
    }
}

void VarList(TreeNode* ptr, Type type)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"ParamDec") == 0)
    {
        TreeNode* nextChild = child->nextSibling;
        if(nextChild == NULL)
        {
            //VarList -> ParamDec
            ParamDec(child, type);
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            //VarList -> ParamDec COMMA VarList
            ParamDec(child, type);
            nextChild = nextChild->nextSibling;
            VarList(nextChild, type);
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        assert(0);
    }
}

void ParamDec(TreeNode* ptr, Type type)
{
    assert(type->kind == FUNCTION);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //ParamDec -> Specifier VarDec
        Type specifier = Specifier(child);
        VarDec(child->nextSibling, type, O_FunDec);
    }
    else
    {
        assert(0);
    }
}

void CompSt(TreeNode* ptr, Type rtnType)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"LC") == 0)
    {
        //CompSt -> LC DefList StmtList RC
        DefList(child->nextSibling, NULL, O_CompSt);
        StmtList(child->nextSibling->nextSibling, rtnType);
    }
    else
    {
        assert(0);
    }
}

void StmtList(TreeNode* ptr, Type rtnType)
{
    TreeNode* child = ptr->firstChild;
    if(child == NULL)
    {
        //StmtList -> epsilon
        return;
    }
    else if(strcmp(child->m_identifier,"Stmt") == 0)
    {
        //StmtList -> Stmt StmtList
        Stmt(child, rtnType);
        StmtList(child->nextSibling, rtnType);
    }
    else
    {
        assert(0);
    }
}

void Stmt(TreeNode* ptr, Type rtnType)
{   
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    Type expType = NULL;
    if(strcmp(child->m_identifier,"Exp") == 0)
    {
        //Stmt -> Exp SEMI
        expType = Exp(child);
    }
    else if(strcmp(child->m_identifier,"CompSt") == 0)
    {
        //Stmt -> CompSt
        CompSt(ptr, rtnType);
    }
    else if(strcmp(child->m_identifier,"RETURN") == 0)
    {
        //Stmt -> RETURN Exp SEMI
        expType = Exp(nextChild);
        if(TypeEqual(expType, rtnType) == 0)
        {
            printf("Error type 8 at Line %d: Type mismatched for return.\n",child->m_lineno);
        }
    }
    else if(strcmp(child->m_identifier,"IF") == 0)
    {
        TreeNode* expChild = nextChild->nextSibling;
        TreeNode* stmtChild = expChild->nextSibling->nextSibling;
        TreeNode* elseChild = stmtChild->nextSibling;
        expType = Exp(expChild);
        Stmt(stmtChild, rtnType);
        if(elseChild == NULL)
        {
            //Stmt -> IF LP Exp RP Stmt
            return;
        }
        else if(strcmp(elseChild,"ELSE") == 0)
        {
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt
            elseChild = elseChild->nextSibling;
            Stmt(elseChild, rtnType);
        }
    }
    else if(strcmp(child->m_identifier,"WHILE") == 0)
    {
        //Stmt -> WHILE LP Exp RP Stmt
        TreeNode* expChild = nextChild->nextSibling;
        TreeNode* stmtChild = expChild->nextSibling->nextSibling;
        expType = Exp(expChild);
        Stmt(stmtChild, rtnType);
    }
    else
    {
        assert(0);
    }
}

void DefList(TreeNode* ptr, Type type, Origin origin)
{
    TreeNode* child = ptr->firstChild;
    if(child == NULL)
    {
        //DefList -> epsilon
        return;
    }
    else if(strcmp(child->m_identifier,"Def") == 0)
    {
        //DefList -> Specifier decList SEMI
        Def(child, type, origin);
        DefList(child->nextSibling, type, origin);
    }
    else 
    {
        assert(0);
    }
}

void Def(TreeNode* ptr, Type type, Origin origin)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //Def -> Specifier DecList SEMI
    }
    else 
    {
        assert(0);
    }
}

void DecList(TreeNode* ptr, Type type, Origin origin)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"Dec") == 0)
    {
        Dec(child, type, origin);
        if(nextChild == NULL)
        {
            return;
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            nextChild = nextChild->nextSibling;
            DecList(nextChild, type, origin);
        }
        else 
        {
            assert(0);
        }
    }
    else
    {
        assert(0);
    }
}

void Dec(TreeNode* ptr, Type type, Origin origin)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"VarDec") == 0)
    {
        VarDec(child,type,origin);
        if(nextChild == NULL)
        {

        }
        else if(strcmp(nextChild->m_identifier,"ASSIGNOP") == 0)
        {

        }
        else
        {
            assert(0);
        }
    }
    else
    {
        assert(0);
    }
}

Type Exp(TreeNode* ptr)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    Type type = (Type)malloc(sizeof(Type_));
    if(strcmp(child->m_identifier,"Exp") == 0)
    {
        TreeNode* expA = child;
        TreeNode* expB = expA->nextSibling->nextSibling;
        TreeNode* op = expA->nextSibling;
        
    }
    else if(strcmp(child->m_identifier,"LP") == 0)
    {

    }
    else if(strcmp(child->m_identifier,"MINUS") == 0)
    {

    }
    else if(strcmp(child->m_identifier,"NOT") == 0)
    {

    }
    else if(strcmp(child->m_identifier,"ID") == 0)
    {
        if(nextChild == NULL)
        {
            //Exp -> ID
            Symbol symbol = HashTableFind(child->m_identifier);
            if(symbol == NULL)
            {

            }
        }
        else
        {
            TreeNode* args = nextChild->nextSibling;
            if(strcmp(args->m_identifier,"Args") == 0)
            {
                //Exp -> ID LP Args RP

            }
            else if(strcmp(args->m_identifier,"RP") == 0)
            {
                //Exp -> ID LP Args RP
            }
            else
            {
                assert(0);
            }
        }
    }
    else if(strcmp(child->m_identifier,"INT") == 0)
    {
        //Exp -> INT
        type->kind = BASIC;
        type->basic = B_INT;
        return type;
    }
    else if(strcmp(child->m_identifier,"FLOAT") == 0)
    {
        //Exp -> FLOAT
        type->kind = BASIC;
        type->basic = B_FLOAT;
        return type;
    }
    else 
    {
        assert(0);
    }
}

SyntaxType GetSyntaxType(const char* str)
{
    if(str[0] == '\0') return T_EMPTY;

    switch (str[0])
    {
    case 'A':
        break;
    case 'C':
        break;
    case 'D':
        break;
    case 'E':
        break;
    case 'F':
        break;
    case 'I':
        break;
    case 'L':
        break;
    case 'M':
        break;
    case 'O':
        break;
    case 'R':
        break;
    case 'S':
        break;
    case 'W':
        break;
    default:
        return T_UNKNOWN;
        break;
    }
}

void PrintError(int errorno, int lineno, const char* msg)
{
    printf("Error type %d at Line %d: %s\n",errorno,lineno,msg);
}