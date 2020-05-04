#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tree.h"

int endLineno;

//#define TREE_DEBUG 1

TreeNode* CreateTreeNode(TreeNodeType type, int lineno, const char* identifier, const char* value)
{
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    
    node->m_type = type;
    strcpy(node->m_identifier, identifier);

    node->m_lineno = lineno;

    node->firstChild = NULL;
    node->nextSibling = NULL;

    switch (type)
    {
    case TYPE_INT:
        node->intValue = atoi(value);
        break;
    case TYPE_FLOAT:
        node->floatValue = atof(value);
        break;
    case TYPE_ID:
    case TYPE_STR:
    case TYPE_RELOP:
        strcpy(node->idName, value);
        break;
    default:
        break;
    }

    return node;
}

void TreeInsert(TreeNode* father, TreeNode *child)
{
    if(father == NULL)
    {
        assert(0);
        return;
    }

#ifdef TREE_DEBUG    
    if(child == NULL) {
        printf("father:%s  child:NULL  @lineNo:%d\n",father->m_identifier,father->m_lineno);
    }
    else {
        printf("father:%s  child:%s  @lineNo:%d\n",father->m_identifier,child->m_identifier,father->m_lineno);
    }
#endif

    if(child == NULL)
    {   
        return;
    }

    if(father->firstChild == NULL)
    {
        father->firstChild = child;
        return;
    }

    TreeNode* p = father->firstChild;
    while (p->nextSibling != NULL)
    {
        p = p->nextSibling;
    }
    p->nextSibling = child;

    return;
}

void OnTreeTraverse(TreeNode* root, int wsCount)
{
    if(root == NULL) return;
    for(int i = 0; i < wsCount; i++) printf("  ");
    switch (root->m_type)
    {
    case TYPE_NONTERMINAL:
        printf("%s (%d)\n",root->m_identifier,root->m_lineno);
        break;
    case TYPE_INT:
        printf("INT: %d\n",root->intValue);
        break;
    case TYPE_FLOAT:
        printf("FLOAT: %f\n",root->floatValue);
        break;
    case TYPE_ID:
        printf("ID: %s\n",root->idName);
        break;
    case TYPE_STR:
        printf("%s: %s\n", root->m_identifier, root->idName);
        break;
    case TYPE_OTHER_TERMINAL:
        printf("%s\n",root->m_identifier);
        break;
    default:
        assert(0);
        break;
    }
    OnTreeTraverse(root->firstChild, wsCount+1);
    OnTreeTraverse(root->nextSibling, wsCount);
}

void TreeTraverse(TreeNode* root, int wsCount)
{
    if(root == NULL || root->firstChild == NULL)
        printf("Program (%d)\n",endLineno);
    else 
        OnTreeTraverse(root, 0);
}

void TreeDestroy(TreeNode* root)
{
    if(root == NULL) return;
    if(root->firstChild != NULL) TreeDestroy(root->firstChild);
    if(root->nextSibling != NULL) TreeDestroy(root->nextSibling);
    free(root);
}