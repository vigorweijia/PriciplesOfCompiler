#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "semanteme.h"

//High-level Definitions
void Program(TreeNode *ptr)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier, "ExtDefList") == 0) ExtDefList(child);
    else if(child == NULL) return;
    else assert(0);
}

void ExtDefList(TreeNode *ptr)
{
    TreeNode* child = ptr->firstChild;

    if(strcmp(child->m_identifier, "ExtDef") == 0)
    {
        //ExtDefList -> ExtDef ExtDefList
        TreeNode* nextChild = child->nextSibling;
        ExtDef(child);
        ExtDefList(nextChild);
    }
    else if(child == NULL) 
    {
        //ExtDefList -> epsilon
        return;
    }
    else assert(0);
}

void ExtDef(TreeNode *ptr)
{
    assert(ptr != NULL);

    //ExtDef -> Specifier ...
    ptr = ptr->firstChild;
    Type type = Specifier(ptr);

    TreeNode* child = ptr->firstChild;
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

    }
    else
    {
        assert(0);
    }
}

void ExtDecList(TreeNode* ptr, Type type)
{

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