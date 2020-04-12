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
        VarDec(child, type, O_ExtDecList, NULL);
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

void VarDec(TreeNode* ptr, Type type, Origin origin, Type structSpecifier)
{
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"ID") == 0)
    {
        //VarDec -> ID
        switch (origin)
        {
        case O_CompSt:
        case O_ExtDecList:
            if(HashTableFind(child->idName) != NULL)
            {
                printf("Error type 3 at Line %d: (E)Redefined variable \"%s\".\n",child->m_lineno,child->idName);
            }
            else if(HashTableInsert(child->idName,type) == 0)
            {
                printf("Error type 3 at Line %d: (I)Redefined variable \"%s\".\n",child->m_lineno,child->idName);
            }
            break;
        case O_StructSpecifier:
            {
                FieldList field = (FieldList)malloc(sizeof(FieldList_));
                field->name = child->idName;
                field->next = NULL;
                field->type = type;
                //pay attention to structure equal
                if(HashTableFind(field->name) != NULL)
                {
                    printf("Error type 15 at Line %d: Redefined field \"%s\".\n",child->m_lineno,child->idName);
                }

                if(structSpecifier->structure == NULL)
                {
                    structSpecifier->structure = field;
                }
                else
                {
                    FieldList temp = structSpecifier->structure;
                    while (temp->next != NULL)
                    {
                        temp = temp->next;
                    }
                    temp->next = field;
                }
            }
            break;
        case O_FunDec:
            {
                FieldList field = (FieldList)malloc(sizeof(FieldList_));
                field->name = child->idName;
                field->next = NULL;
                field->type = type;
                if(HashTableFind(field->name) != NULL)
                {
                    printf("Error type 3 at Line %d: Redefined varialble \"%s\".\n",child->m_lineno,child->idName);
                }

                if(structSpecifier->function.params == NULL)
                {
                    structSpecifier->function.params = field;
                }
                else
                {
                    FieldList temp = structSpecifier->function.params;
                    while (temp->next != NULL)
                    {
                        temp = temp->next;
                    }
                    temp->next = field;
                }
                structSpecifier->function.cnt++;
            }
            break;
        default:
            assert(0);
            break;
        }
    }
    else if(strcmp(child->m_identifier,"VarDec") == 0)
    {
        //VarDec -> VarDec LB INT RB
        Type arrayType = (Type)malloc(sizeof(Type_));
        arrayType->kind = ARRAY;
        arrayType->array.elem = type;
        arrayType->array.size = child->nextSibling->nextSibling->intValue;
        VarDec(child,arrayType,origin,structSpecifier);
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
        Type fundecSpecifier = (Type)malloc(sizeof(Type_));
        fundecSpecifier->kind = FUNCTION;
        fundecSpecifier->function.rtnType = type;
        fundecSpecifier->function.params = NULL;
        fundecSpecifier->function.cnt = 0;
        VarList(nextChild, type, O_FunDec, fundecSpecifier);
        if(HashTableInsert(child->idName, type) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",child->m_lineno,child->idName);
        }
    }
    else if(strcmp(nextChild->m_identifier,"RP") == 0)
    {
        //FunDec -> ID LP RP
        Type fundecSpecifier = (Type)malloc(sizeof(Type_));
        fundecSpecifier->kind = FUNCTION;
        fundecSpecifier->function.rtnType = type;
        fundecSpecifier->function.params = NULL;
        fundecSpecifier->function.cnt = 0;
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

void VarList(TreeNode* ptr, Type type, Origin origin, Type fundecSpecifier)
{
    assert(origin == O_FunDec);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"ParamDec") == 0)
    {
        TreeNode* nextChild = child->nextSibling;
        if(nextChild == NULL)
        {
            //VarList -> ParamDec
            ParamDec(child, type, origin, fundecSpecifier);
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            //VarList -> ParamDec COMMA VarList
            ParamDec(child, type, origin, fundecSpecifier);
            nextChild = nextChild->nextSibling;
            VarList(nextChild, type, origin, fundecSpecifier);
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

void ParamDec(TreeNode* ptr, Type type, Origin origin, Type fundecSpecifier)
{
    assert(origin == O_FunDec);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //ParamDec -> Specifier VarDec
        Type specifier = Specifier(child);
        VarDec(child->nextSibling, specifier, origin, fundecSpecifier);
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
        else if(strcmp(elseChild->m_identifier,"ELSE") == 0)
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
    assert(origin == O_CompSt || origin == O_StructSpecifier);
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
    assert(origin == O_CompSt || origin == O_StructSpecifier);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //Def -> Specifier DecList SEMI
        Type fieldType = Specifier(child);
        if(origin == O_CompSt) DecList(child->nextSibling, fieldType, origin, NULL);
        else DecList(child->nextSibling, fieldType, origin, type);
    }
    else 
    {
        assert(0);
    }
}

void DecList(TreeNode* ptr, Type type, Origin origin, Type structSpecifier)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"Dec") == 0)
    {
        Dec(child, type, origin, structSpecifier);
        if(nextChild == NULL)
        {
            return;
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            nextChild = nextChild->nextSibling;
            DecList(nextChild, type, origin, structSpecifier);
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

void Dec(TreeNode* ptr, Type type, Origin origin, Type structSpecifier)
{
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"VarDec") == 0)
    {
        VarDec(child,type,origin,structSpecifier);
        if(nextChild == NULL)
        {
            //Dec -> VarDec
        }
        else if(strcmp(nextChild->m_identifier,"ASSIGNOP") == 0)
        {
            //Dev -> VarDec ASSIGNOP Exp
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
        TreeNode* op = expA->nextSibling;
        TreeNode* expB = expA->nextSibling->nextSibling;
        if(strcmp(op->m_identifier,"LB") == 0)
        {
            Type typeA = Exp(expA);
            Type typeB = Exp(expB);
            int flag = 0;
            if(typeA->kind != ARRAY)
            {
                flag = 1;
                printf("Error type 10 at Line %d: \"%s\" is not an array.\n",expA->m_lineno,expA->idName);
            }
            if(!(typeB->kind == BASIC && typeB->basic == B_INT))
            {
                flag = 1;
                printf("Error type 12 at Line %d: Element in [] is not an integer.\n",expB->m_lineno);
            }
            if(flag == 1) return NULL;
            else return typeA->array.elem;
        }
        else if(strcmp(op->m_identifier,"DOT") == 0)
        {
            Type typeA = Exp(expA);
            if(typeA->kind != STRUCTURE)
            {
                printf("Error type 13 at Line %d: Illegal use of \'.\'\n",expA->m_lineno);
                return NULL;
            }
            //TODO: find in structure, error 14

        }
        else 
        {
            Type typeA = Exp(expA);
            Type typeB = Exp(expB);
            if(strcmp(op->m_identifier,"ASSIGNOP") == 0)
            {
                //Exp -> Exp ASSIGNOP Exp
                TreeNode* expAChild = expA->firstChild;
                if( !(strcmp(expAChild->m_identifier,"ID") == 0
                    || strcmp(expAChild->m_identifier,"Exp") == 0 && strcmp(expAChild->nextSibling->m_identifier,"DOT") == 0
                    || strcmp(expAChild->m_identifier,"Exp") == 0 && strcmp(expAChild->nextSibling->m_identifier,"LB") == 0) 
                )
                {
                    printf("Error Type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",expA->m_lineno);
                    return NULL;
                }
                if(TypeEqual(typeA,typeB) == 0)
                {
                    printf("Error Type 5 at Line %d: Type mismatched for assignment.\n",child->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"AND") == 0)
            {
                if(!(typeA->kind == BASIC && typeB->kind == BASIC && typeA->basic == B_INT && typeB->basic == B_INT))
                {
                    printf("Error type 7 at Line %d: \'&&\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"OR") == 0)
            {
                if(!(typeA->kind == BASIC && typeB->kind == BASIC && typeA->basic == B_INT && typeB->basic == B_INT))
                {
                    printf("Error type 7 at Line %d: \'||\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"RELOP") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'RELOP\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"PLUS") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'+\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"MINUS") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'-\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"STAR") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'*\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else if(strcmp(op->m_identifier,"DIV") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'\\\' Type mismatched for operands.\n", expA->m_lineno);
                    return NULL;
                }
            }
            else 
            {
                assert(0);
            }
        }
    }
    else if(strcmp(child->m_identifier,"LP") == 0)
    {
        //Exp -> LP EXP RP
    }
    else if(strcmp(child->m_identifier,"MINUS") == 0)
    {
        //Exp -> MINUS Exp
        Type childExpType = Exp(nextChild);
        if(childExpType->kind != BASIC)
        {
            printf("Error type 7 at Line %d: Operator \'-\' mismatched for operands.\n",nextChild->m_lineno);
            return NULL;
        }
        return childExpType;
    }
    else if(strcmp(child->m_identifier,"NOT") == 0)
    {
        //Exp -> NOT Exp
        Type childExpType = Exp(nextChild);
        if(childExpType->kind != BASIC || (childExpType->kind == BASIC && childExpType->basic != B_INT))
        {
            printf("Error type 7 at Line %d: Operator \'!\' mismatched for operands.\n",nextChild->m_lineno);
            return NULL;
        }
        return childExpType;
    }
    else if(strcmp(child->m_identifier,"ID") == 0)
    {
        if(nextChild == NULL)
        {
            //Exp -> ID
            Symbol symbol = HashTableFind(child->m_identifier);
            if(symbol == NULL)
            {
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",child->m_lineno,child->idName);
                return NULL;
            }
            return symbol->type;
        }
        else
        {
            TreeNode* args = nextChild->nextSibling;
            Symbol funSymbol = HashTableFind(child->idName);
            if(funSymbol == NULL)
            {
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n",child->m_lineno,child->idName);
                return NULL;
            }
            if(funSymbol->type->kind != FUNCTION)
            {
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n",child->m_lineno,child->idName);
                return NULL;
            }
            if(strcmp(args->m_identifier,"Args") == 0)
            {
                //Exp -> ID LP Args RP
                Args(args, funSymbol->type->function.params);
                return funSymbol->type->function.rtnType;
            }
            else if(strcmp(args->m_identifier,"RP") == 0)
            {
                //Exp -> ID LP RP
                if(funSymbol->type->function.cnt != 0)
                {
                    printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n",child->m_lineno,child->idName);
                    return funSymbol->type->function.rtnType;
                }
                return funSymbol->type->function.rtnType;
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
    assert(0);
}

void Args(TreeNode* ptr, FieldList param)
{
    if(ptr == NULL && param == NULL)
    {
        return;
    }
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"Exp") == 0)
    {
        Type type = Exp(child);
        if(TypeEqual(type,param->type) == 0)
        {
            printf("Error type 9 at Line %d: Function parameter's type dismatches.\n",ptr->m_lineno);
            return;
        }
        if(nextChild == NULL)
        {
            //Args -> Exp
            return;
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            nextChild = nextChild->nextSibling;
            if((nextChild == NULL && param->next != NULL) || (nextChild != NULL && param->next == NULL))
            {
                printf("Error type 9 at Line %d: Function parameter's amount dismatches.\n",ptr->m_lineno);
                return;
            }
            Args(nextChild,param->next);
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

int TypeEqual(Type typeA, Type typeB)
{
    if(typeA->kind != typeB->kind) return 0;
    if(typeA->kind == FUNCTION) assert(0);
    if(typeA->kind == STRUCTURE) return StructTypeEqual(typeA, typeB);
    if(typeA->kind == ARRAY) return ArrayTypeEqual(typeA, typeB);
    if(typeA->basic == typeB->basic) return 1;
    else return 0;
}

int StructTypeEqual(Type typeA, Type typeB)
{
    assert(typeA->kind == STRUCTURE && typeB->kind == STRUCTURE);
    FieldList tempA = typeA->structure;
    FieldList tempB = typeB->structure;
    while (tempA != NULL && tempB != NULL)
    {
        if(TypeEqual(tempA->type,tempB->type) == 0) return 0;
        tempA = tempA->next;
        tempB = tempB->next;
    }
    if(tempA == NULL && tempB != NULL) return 0;
    if(tempA != NULL && tempB == NULL) return 0;
    return 1;
}

int ArrayTypeEqual(Type typeA, Type typeB)
{
    if(typeA->kind != typeB->kind) return 0;
    if(typeA->kind == BASIC)
    {
        return (typeA->basic == typeB->basic);
    }
    else if(typeA->kind == ARRAY)
    {
        if(typeA->array.size != typeB->array.size) return 0;
        else return ArrayTypeEqual(typeA->array.elem, typeB->array.elem);
    }
    else
    {
        assert(0);
    }
}