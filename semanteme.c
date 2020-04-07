#include <string.h>
#include <assert.h>

#include "semanteme.h"

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