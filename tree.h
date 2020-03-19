#ifndef __TREE_H__
#define __TREE_H__

#define IDENTIFIER_LENTH 32
#define ID_LENTH 32

#ifndef TREENODETYPE
#define TREENODETYPE
typedef enum{
    TYPE_NONTERMINAL = 1,
    TYPE_INT = 2,
    TYPE_FLOAT = 4,
    TYPE_ID = 8,
    TYPE_STR = 16, //{TYPE} int|float
    TYPE_OTHER_TERMINAL = 32
}TreeNodeType;
#endif

typedef struct TreeNode{

    TreeNodeType m_type;
    int m_lineno;

    char m_identifier[IDENTIFIER_LENTH];  
    
    union
    {
        int intValue; //TYPE_INT
        float floatValue; //TYPE_FLOAT
        char idName[ID_LENTH]; //TYPE_ID,TYPE_SPECIFIER
    };

    struct TreeNode* firstChild;
    struct TreeNode* nextSibling;

}TreeNode;

extern TreeNode* CreateTreeNode(TreeNodeType type, int lineno, const char* identifier, const char* value);
extern void TreeInsert(TreeNode* fa, TreeNode *ch);
extern void TreeTraverse(TreeNode* root, int wsCount);
extern void TreeDestroy(TreeNode* root);

#endif