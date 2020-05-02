#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "semanteme.h"

//High-level Definitions
void Program(TreeNode *ptr)
{
    HashTableInit(); //!!!!!!!!!!!!!!!!!!!!!!!
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    if(child == NULL) return; //ExtDefList is null
    else if(strcmp(child->m_identifier, "ExtDefList") == 0) ExtDefList(child);
    else assert(0);
}

void ExtDefList(TreeNode *ptr)
{ 
    if(ptr == NULL) return;
    SematicDebug(ptr);
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
    SematicDebug(ptr);
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
        Type func = FunDec(child, type);
        assert(func != NULL);

        Operand funcOp = (Operand)malloc(sizeof(Operand_));
        funcOp->kind = FUNCTION;
        funcOp->value = func->function.funcName;
        InterCode funcCode = (InterCode)malloc(sizeof(InterCode_));
        funcCode->kind = FUNCTION_C;
        funcCode->singleOp.op = funcOp;
        InsertCode(funcCode);

        FieldList funcParam = func->function.params;
        while (funcParam != NULL)
        {
            Operand paramOp = (Operand)malloc(sizeof(Operand_));
            paramOp->kind = VARIABLE_O;
            paramOp->value = funcParam->name;
            InterCode paramCode = (InterCode)malloc(sizeof(InterCode_));
            paramCode->kind = PARAM_C;
            paramCode->singleOp.op = paramOp;
            InsertCode(paramCode);

            funcParam = funcParam->next;
        }

        CompSt(child->nextSibling, type);
    }
    else
    {
        assert(0);
    }
}

void ExtDecList(TreeNode* ptr, Type type)
{
    SematicDebug(ptr);
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
    SematicDebug(ptr);
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
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"STRUCT") != 0)
    {
        assert(0);
    }

    child = child->nextSibling;
    if(strcmp(child->m_identifier,"LC") == 0)
    {
        //OptTag is null
        //StructSpecifier -> STRUCT LC DefList RC
        TreeNode* childDefList = child->nextSibling->nextSibling;
        DefList(childDefList, type, O_StructSpecifier);
    }
    else if(strcmp(child->m_identifier,"OptTag") == 0)
    {
        //StructSpecifier -> STRUCT OptTag LC DefList RC
        TreeNode* childDefList = child->nextSibling->nextSibling;
        OptTag(child, type);
        if(strcmp(childDefList->m_identifier,"DefList") == 0)
        {
            //in case DefList is null
            DefList(childDefList, type, O_StructSpecifier);
        }
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
    if(ptr == NULL) return;
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    if(child == NULL)
    {
        //OptTag -> epsilon
        return;
    }
    else if(strcmp(child->m_identifier, "ID") == 0)
    {
        Symbol symbol = HashTableFind(child->idName);
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
    SematicDebug(ptr);
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
    SematicDebug(ptr);
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
            //printf("insert secc\n");
            break;
        case O_StructSpecifier:
            {
                FieldList field = (FieldList)malloc(sizeof(FieldList_));
                //pay attention to structure equal
                if(!(HashTableFind(child->idName) == NULL && HashTableFindStructureMember(child->idName,structSpecifier) == NULL))
                {
                    printf("Error type 15 at Line %d: Redefined field \"%s\".\n",child->m_lineno,child->idName);
                    return;
                }
                field->name = child->idName;
                field->next = NULL;
                field->type = type;

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
                else if(HashTableInsert(field->name, type) == 0)
                {
                    printf("Error type 3 at Line %d: (I)Redefined varialble \"%s\".\n",child->m_lineno,child->idName);
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

Type FunDec(TreeNode* ptr, Type type)
{
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling->nextSibling;
    Type fundecSpecifier = (Type)malloc(sizeof(Type_));
    if(strcmp(nextChild->m_identifier,"VarList") == 0)
    {
        //FunDec -> ID LP VarList RP
        fundecSpecifier->kind = FUNCTION;
        fundecSpecifier->function.rtnType = type;
        fundecSpecifier->function.params = NULL;
        fundecSpecifier->function.cnt = 0;
        fundecSpecifier->function.funcName = child->idName;
        VarList(nextChild, type, O_FunDec, fundecSpecifier);
        if(HashTableInsert(child->idName, fundecSpecifier) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",child->m_lineno,child->idName);
        }
    }
    else if(strcmp(nextChild->m_identifier,"RP") == 0)
    {
        //FunDec -> ID LP RP
        fundecSpecifier->kind = FUNCTION;
        fundecSpecifier->function.rtnType = type;
        fundecSpecifier->function.params = NULL;
        fundecSpecifier->function.cnt = 0;
        fundecSpecifier->function.funcName = child->idName;
        if(HashTableInsert(child->idName, fundecSpecifier) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",child->m_lineno,child->idName);
        }
    }
    else
    {
        assert(0);
    }
    return fundecSpecifier;
}

void VarList(TreeNode* ptr, Type type, Origin origin, Type fundecSpecifier)
{
    SematicDebug(ptr);
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
        //printf("paramcnt at varlist:%d\n",fundecSpecifier->function.cnt);
    }
    else
    {
        assert(0);
    }
}

void ParamDec(TreeNode* ptr, Type type, Origin origin, Type fundecSpecifier)
{
    SematicDebug(ptr);
    assert(origin == O_FunDec);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //ParamDec -> Specifier VarDec
        Type specifier = Specifier(child);
        VarDec(child->nextSibling, specifier, origin, fundecSpecifier);
        //printf("paramcnt:%d\n",fundecSpecifier->function.cnt);
    }
    else
    {
        assert(0);
    }
}

void CompSt(TreeNode* ptr, Type rtnType)
{
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"LC") == 0)
    {
        //CompSt -> LC DefList StmtList RC
        if(strcmp(nextChild->m_identifier,"RC") == 0)
        {
            //Both DefList and StmtList is null
            return;
        }
        else if(strcmp(nextChild->m_identifier,"StmtList") == 0)
        {
            //DefList is null
            StmtList(child->nextSibling, rtnType);
        }
        else if(strcmp(nextChild->nextSibling->m_identifier,"RC") == 0)
        {
            //StmtList is null
            DefList(child->nextSibling, NULL, O_CompSt);
        }
        else 
        {
            DefList(child->nextSibling, NULL, O_CompSt);
            StmtList(child->nextSibling->nextSibling, rtnType);
        }
        wDebug("CompSt succ");
    }
    else
    {
        assert(0);
    }
}

void StmtList(TreeNode* ptr, Type rtnType)
{
    if(ptr == NULL) return;
    SematicDebug(ptr);
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
        wDebug("StmtList succ");
    }
    else
    {
        assert(0);
    }
}

void Stmt(TreeNode* ptr, Type rtnType)
{   
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    Type expType = NULL;
    if(strcmp(child->m_identifier,"Exp") == 0)
    {
        //Stmt -> Exp SEMI
        expType = Exp(child, NULL);
        wDebug("Stmt succ");
    }
    else if(strcmp(child->m_identifier,"CompSt") == 0)
    {
        //Stmt -> CompSt
        CompSt(child, rtnType);
    }
    else if(strcmp(child->m_identifier,"RETURN") == 0)
    {
        //Stmt -> RETURN Exp SEMI
        Operand rtnOp = NewTempVar();
        expType = Exp(nextChild, rtnOp);
        if(TypeEqual(expType, rtnType) == 0)
        {
            printf("Error type 8 at Line %d: Type mismatched for return.\n",child->m_lineno);
        }
        InterCode rtnCode = (InterCode)malloc(sizeof(InterCode_));
        rtnCode->kind = RETURN_C;
        rtnCode->singleOp.op = rtnOp;
        InsertCode(rtnCode);
    }
    else if(strcmp(child->m_identifier,"IF") == 0)
    {
        Operand label1 = NewLabel();
        Operand label2 = NewLabel();
        TreeNode* expChild = nextChild->nextSibling;
        TreeNode* stmtChild = expChild->nextSibling->nextSibling;
        TreeNode* elseChild = stmtChild->nextSibling;

        expType = ExpCond(expChild, label1, label2); // code 1

        InterCode label1Code = (InterCode)malloc(sizeof(InterCode_));
        label1Code->kind = LABEL_C;
        label1Code->singleOp.op = label1;
        InsertCode(label1Code); // label 1

        Stmt(stmtChild, rtnType); // code 2

        if(elseChild == NULL)
        {
            //Stmt -> IF LP Exp RP Stmt
            InterCode label2Code = (InterCode)malloc(sizeof(InterCode_));
            label2Code->kind = LABEL_C;
            label2Code->singleOp.op = label2;
            InsertCode(label2Code); // label 2
            return;
        }
        else if(strcmp(elseChild->m_identifier,"ELSE") == 0)
        {
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt
            Operand label3 = NewLabel();

            InterCode label3Goto = (InterCode)malloc(sizeof(InterCode_));
            label3Goto->kind = GOTO_C;
            label3Goto->singleOp.op = label3;
            InsertCode(label3Goto); // goto label3

            InterCode label2Code = (InterCode)malloc(sizeof(InterCode_));
            label2Code->kind = LABEL_C;
            label2Code->singleOp.op = label2;
            InsertCode(label2Code); // label2

            elseChild = elseChild->nextSibling;
            Stmt(elseChild, rtnType); // code3

            InterCode label3Code = (InterCode)malloc(sizeof(InterCode_));
            label3Code->kind = LABEL_C;
            label3Code->singleOp.op = label3;
            InsertCode(label3Code); // label3
        }
    }
    else if(strcmp(child->m_identifier,"WHILE") == 0)
    {
        //Stmt -> WHILE LP Exp RP Stmt
        Operand label1 = NewLabel();
        Operand label2 = NewLabel();
        Operand label3 = NewLabel();
        TreeNode* expChild = nextChild->nextSibling;
        TreeNode* stmtChild = expChild->nextSibling->nextSibling;

        InterCode label1Code = (InterCode)malloc(sizeof(InterCode_));
        label1Code->kind = LABEL_C;
        label1Code->singleOp.op = label1;
        InsertCode(label1Code); //label 1

        expType = ExpCond(expChild, label2, label3); //code 1

        InterCode label2Code = (InterCode)malloc(sizeof(InterCode_));
        label2Code->kind = LABEL_C;
        label2Code->singleOp.op = label2;
        InsertCode(label2Code); //label 2

        Stmt(stmtChild, rtnType); //code 2

        InterCode label1Goto = (InterCode)malloc(sizeof(InterCode_));
        label1Goto->kind = GOTO_C;
        label1Goto->singleOp.op = label1;
        InsertCode(label1Goto);

        InterCode label3Code = (InterCode)malloc(sizeof(InterCode_));
        label3Code->kind = LABEL_C;
        label3Code->singleOp.op = label3;
        InsertCode(label3Code); //label 3
    }
    else
    {
        assert(0);
    }
}

void DefList(TreeNode* ptr, Type type, Origin origin)
{
    if(ptr == NULL) return;
    SematicDebug(ptr);
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
        wDebug("Def succ at DefList");
        DefList(child->nextSibling, type, origin);
        wDebug("DefList yes");
    }
    else 
    {
        assert(0);
    }
}

void Def(TreeNode* ptr, Type type, Origin origin)
{
    SematicDebug(ptr);
    assert(origin == O_CompSt || origin == O_StructSpecifier);
    TreeNode* child = ptr->firstChild;
    if(strcmp(child->m_identifier,"Specifier") == 0)
    {
        //Def -> Specifier DecList SEMI
        Type fieldType = Specifier(child);
        if(origin == O_CompSt) DecList(child->nextSibling, fieldType, origin, NULL);
        else DecList(child->nextSibling, fieldType, origin, type);
        wDebug("Def yes");
    }
    else 
    {
        assert(0);
    }
}

void DecList(TreeNode* ptr, Type type, Origin origin, Type structSpecifier)
{
    SematicDebug(ptr);
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"Dec") == 0)
    {
        Dec(child, type, origin, structSpecifier);
        if(nextChild == NULL)
        {
            wDebug("DecList yes null");
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
        wDebug("DecList yes");
    }
    else
    {
        assert(0);
    }
}

void Dec(TreeNode* ptr, Type type, Origin origin, Type structSpecifier)
{
    SematicDebug(ptr);
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
            if(origin == O_StructSpecifier)
            {
                printf("Error type 15 at Line %d: Invalid definition in structure.\n",child->m_lineno);
                return;
            }
            Operand place = (Operand)malloc(sizeof(Operand_));
            place->kind = VARIABLE_O;
            //place->value = 
            Type expType = Exp(nextChild->nextSibling);
            if(TypeEqual(type,expType) == 0)
            {
                printf("Error type 5 at Line %d: Type dismatched for assignment.\n",child->m_lineno);
                return;
            }
        }
        else
        {
            assert(0);
        }
        //printf("dec yes\n");
    }
    else
    {
        assert(0);
    }
}

Type Exp(TreeNode* ptr, Operand place)
{
    SematicDebug(ptr);
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
                printf("Error type 10 at Line %d: Variable before [] is not an array.\n",expA->m_lineno);
            }
            if(!(typeB->kind == BASIC && typeB->basic == B_INT))
            {
                flag = 1;
                printf("Error type 12 at Line %d: Element in [] is not an integer.\n",expB->m_lineno);
            }
            if(flag == 1) 
            {
                wDebug("return gError");
                if(gError == NULL) wDebug("gError is null");
                return gError;
            }
            else 
            {
                return typeA->array.elem;
            }
        }
        else if(strcmp(op->m_identifier,"DOT") == 0)
        {
            Type typeA = Exp(expA);
            if(typeA == NULL) wDebug("NULL Detected");
            if(typeA->kind != STRUCTURE)
            {
                printf("Error type 13 at Line %d: Illegal use of \'.\'\n",expA->m_lineno);
                return gError;
            }
            //TODO: find in structure, error 14
            Symbol memberSymbol = HashTableFindStructureMember(expB->idName, typeA);
            if(memberSymbol == NULL)
            {
                printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",expB->m_lineno,expB->idName);
                return gError;
            }
            return memberSymbol->type;
        }
        else 
        {
            Type typeA = Exp(expA);
            Type typeB = Exp(expB);
            wDebug("Get type succ");
            if(strcmp(op->m_identifier,"ASSIGNOP") == 0)
            {
                //Exp -> Exp ASSIGNOP Exp
                TreeNode* expAChild = expA->firstChild;
                if( !(strcmp(expAChild->m_identifier,"ID") == 0 && expAChild->nextSibling == NULL
                    || strcmp(expAChild->m_identifier,"Exp") == 0 && strcmp(expAChild->nextSibling->m_identifier,"DOT") == 0
                    || strcmp(expAChild->m_identifier,"Exp") == 0 && strcmp(expAChild->nextSibling->m_identifier,"LB") == 0) 
                )
                {
                    printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",expA->m_lineno);
                    return gError;
                }
                if(TypeEqual(typeA,typeB) == 0)
                {
                    printf("Error type 5 at Line %d: Type mismatched for assignment.\n",child->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"AND") == 0)
            {
                if(!(typeA->kind == BASIC && typeB->kind == BASIC && typeA->basic == B_INT && typeB->basic == B_INT))
                {
                    printf("Error type 7 at Line %d: \'&&\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"OR") == 0)
            {
                if(!(typeA->kind == BASIC && typeB->kind == BASIC && typeA->basic == B_INT && typeB->basic == B_INT))
                {
                    printf("Error type 7 at Line %d: \'||\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"RELOP") == 0)
            {
                //printf("typeA:%d typeB:%d\n",typeA->kind,typeB->kind);
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'RELOP\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"PLUS") == 0)
            {
                wDebug("Plus start");
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'+\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
                wDebug("Plus succ");
            }
            else if(strcmp(op->m_identifier,"MINUS") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'-\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"STAR") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'*\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else if(strcmp(op->m_identifier,"DIV") == 0)
            {
                if(!(TypeEqual(typeA,typeB) && typeA->kind == BASIC && typeB->kind == BASIC))
                {
                    printf("Error type 7 at Line %d: \'/\' Type mismatched for operands.\n", expA->m_lineno);
                    return gError;
                }
            }
            else 
            {
                assert(0);
            }
            return typeA;
        }
    }
    else if(strcmp(child->m_identifier,"LP") == 0)
    {
        //Exp -> LP EXP RP
        return Exp(child->nextSibling);
    }
    else if(strcmp(child->m_identifier,"MINUS") == 0)
    {
        //Exp -> MINUS Exp
        Type childExpType = Exp(nextChild);
        if(childExpType->kind != BASIC)
        {
            printf("Error type 7 at Line %d: Operator \'-\' mismatched for operands.\n",nextChild->m_lineno);
            return gError;
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
            return gError;
        }
        return childExpType;
    }
    else if(strcmp(child->m_identifier,"ID") == 0)
    {
        if(nextChild == NULL)
        {
            //Exp -> ID
            Symbol symbol = HashTableFind(child->idName);
            if(symbol == NULL)
            {
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",child->m_lineno,child->idName);
                return gError;
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
                return gError;
            }

            if(funSymbol->type->kind != FUNCTION)
            {
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n",child->m_lineno,child->idName);
                return gError;
            }

            //printf("args: %s\n",args->m_identifier);
            if(strcmp(args->m_identifier,"Args") == 0)
            {
                //Exp -> ID LP Args RP
                //printf("funcparam:%d\n",funSymbol->type->function.cnt);
                Args(args, funSymbol->type->function.params);
                //printf("rtnType:%d\n",funSymbol->type->function.rtnType->kind);
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
        wDebug("INT succ");
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

Type ExpCond(TreeNode* ptr, Operand trueLabel, Operand falseLabel)
{

}

void Args(TreeNode* ptr, FieldList param)
{
    SematicDebug(ptr);
    if(ptr == NULL && param == NULL)
    {
        return;
    }
    TreeNode* child = ptr->firstChild;
    TreeNode* nextChild = child->nextSibling;
    if(strcmp(child->m_identifier,"Exp") == 0)
    {
        Type type = Exp(child);
        wDebug("get type succ at args");
        if(param == NULL) assert(0);
        if(TypeEqual(type,param->type) == 0)
        {
            printf("Error type 9 at Line %d: Function parameter's type dismatches.\n",ptr->m_lineno);
            return;
        }
        wDebug("type equal at args");
        if(nextChild == NULL)
        {
            //Args -> Exp
            return;
        }
        else if(strcmp(nextChild->m_identifier,"COMMA") == 0)
        {
            //Args -> Exp COMMA Args
            
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
    if(typeA == NULL || typeB == NULL) return 0;
    if(typeA->kind != typeB->kind) return 0;
    if(typeA->kind == FUNCTION) return 0;
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
        //if(typeA->array.size != typeB->array.size) return 0;
        return ArrayTypeEqual(typeA->array.elem, typeB->array.elem);
    }
    else if(typeA->kind == STRUCTURE)
    {
        return StructTypeEqual(typeA, typeB);
    }
}

void SematicDebug(TreeNode* ptr)
{
#ifdef WDEBUG
    printf("ptr:%s %d\n",ptr->m_identifier,ptr->m_lineno);
#endif
}

void wDebug(const char* msg)
{
#ifdef WDEBUG
    printf("%s\n",msg);
#endif
}