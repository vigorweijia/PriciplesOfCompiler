#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "IRCode.h"

InterCode gInterCodeHead = NULL;
InterCode gInterCodeTail = NULL;

int gTempVarCount = 1;
int gLabelCount = 1;

void InsertCode(InterCode ptr)
{
    assert(ptr != NULL);

    ptr->next = NULL;
    ptr->prev = NULL;
    if(gInterCodeHead == NULL)
    {
        gInterCodeHead = ptr;
        gInterCodeTail = ptr;
    }
    else
    {
        gInterCodeTail->next = ptr;
        ptr->prev = gInterCodeTail;
        gInterCodeTail = ptr;
    }
}

void DeleteCode(InterCode ptr)
{
    assert(ptr != NULL);

    if(ptr == gInterCodeTail)
    {
        gInterCodeTail = ptr->prev;
        gInterCodeTail->next = NULL;
    }
    else
    {
        ptr->next->prev = ptr->prev;
    }
    
    if(ptr == gInterCodeHead)
    {
        gInterCodeHead = ptr->next;
        gInterCodeHead->prev = NULL;
    }
    else
    {
        ptr->prev->next = ptr->next;
    }
    
    free(ptr);
}

Operand NewLabel()
{
    Operand tempLabel = (Operand)malloc(sizeof(Operand_));
    memset(tempLabel, 0, sizeof(Operand_));
    tempLabel->kind = LABEL_O;
    tempLabel->varNo = gLabelCount;
    gLabelCount++;
    return tempLabel;
}

Operand NewTempVar()
{
    Operand tempVar = (Operand)malloc(sizeof(Operand_));
    memset(tempVar, 0, sizeof(Operand_));
    tempVar->kind = TEMPORARY_O;
    tempVar->varNo = gTempVarCount;
    gTempVarCount++;
    return tempVar;
}

Operand NewConstant(const char* v)
{
    Operand tempConstant = (Operand)malloc(sizeof(Operand_));
    memset(tempConstant, 0, sizeof(tempConstant));
    tempConstant->kind = CONSTANT_O;
    tempConstant->value = v;
    return tempConstant;
}

InterCode GenSingleOp(Operand op, InterCodeType t)
{
    InterCode tempCode = (InterCode)malloc(sizeof(InterCode_));
    memset(tempCode, 0, sizeof(InterCode_));
    tempCode->kind = t;
    tempCode->singleOp.op = op;
    return tempCode;
}

InterCode GenDoubleOp(Operand result, Operand op1, Operand op2, InterCodeType t)
{
    InterCode tempCode = (InterCode)malloc(sizeof(InterCode_));
    memset(tempCode, 0, sizeof(InterCode_));
    tempCode->kind = t;
    tempCode->doubleOp.op1 = op1;
    tempCode->doubleOp.op2 = op2;
    tempCode->doubleOp.result = result;
    return tempCode;
}

InterCode GenAssign(Operand left, Operand right)
{
    InterCode tempCode = (InterCode)malloc(sizeof(InterCode_));
    memset(tempCode, 0, sizeof(InterCode_));
    tempCode->kind = ASSIGN_C;
    tempCode->assign.left = left;
    tempCode->assign.right = right;
    return tempCode;
}

InterCode GenCall(Operand left, Operand right)
{
    InterCode tempCode = (InterCode)malloc(sizeof(InterCode_));
    memset(tempCode, 0, sizeof(InterCode_));
    tempCode->kind = CALL_C;
    tempCode->assign.left = left;
    tempCode->assign.right = right;
    return tempCode;
}