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
    tempLabel->next = NULL;
    return tempLabel;
}

Operand NewTempVar()
{
    Operand tempVar = (Operand)malloc(sizeof(Operand_));
    memset(tempVar, 0, sizeof(Operand_));
    tempVar->kind = TEMPORARY_O;
    tempVar->varNo = gTempVarCount;
    gTempVarCount++;
    tempVar->next = NULL;
    return tempVar;
}

Operand NewConstant(const char* v)
{
    Operand tempConstant = (Operand)malloc(sizeof(Operand_));
    memset(tempConstant, 0, sizeof(tempConstant));
    tempConstant->kind = CONSTANT_O;
    tempConstant->value = v;
    tempConstant->next = NULL;
    return tempConstant;
}

Operand NewFunction(const char* funcName)
{
    Operand tempFunc = (Operand)malloc(sizeof(Operand_));
    memset(tempFunc, 0, sizeof(tempFunc));
    tempFunc->kind = FUNCTION_O;
    tempFunc->value = funcName;
    tempFunc->next = NULL;
    return tempFunc;
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

InterCode GenTripleOp(Operand op1, Operand op2, Operand label, const char* opr)
{
    InterCode tempCode = (InterCode)malloc(sizeof(InterCode_));
    memset(tempCode, 0, sizeof(InterCode_));
    tempCode->kind = IFGOTO_C;
    tempCode->tripleOp.op1 = op1;
    tempCode->tripleOp.op2 = op2;
    tempCode->tripleOp.label = label;
    tempCode->tripleOp.opr = opr;
    assert(label->kind == LABEL_O);
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

void InterCodePrinter(const char* msg)
{
    printf("%s",msg);
}

void InterCodePrintCal(InterCode p)
{
    OperandPrint(p->doubleOp.result);
    InterCodePrinter(":= ");
    OperandPrint(p->doubleOp.op1);
    switch (p->kind)
    {
    case ADD_C:
        InterCodePrinter("+ ");
        break;
    case SUB_C:
        InterCodePrinter("- ");
        break;
    case MUL_C:
        InterCodePrinter("* ");
        break;
    case DIV_C:
        InterCodePrinter("/ ");
        break;
    default:
        break;
    } 
    OperandPrint(p->doubleOp.op2);
}

void InterCodePrint()
{
    InterCode p = gInterCodeHead;
    while (p != NULL)
    {
        switch(p->kind)
        {
            case ASSIGN_C:
            OperandPrint(p->assign.left);
            InterCodePrinter(":= ");
            OperandPrint(p->assign.right);
            break;
            case ADD_C:
            case SUB_C:
            case MUL_C:
            case DIV_C:
            InterCodePrintCal(p);
            break;
            case FUNCTION_C:
            InterCodePrinter("FUNCTION ");
            OperandPrint(p->singleOp.op);
            InterCodePrinter(":");
            break;
            case PARAM_C:
            InterCodePrinter("PARAM ");
            OperandPrint(p->singleOp.op);
            break;
            case RETURN_C:
            InterCodePrinter("RETURN ");
            OperandPrint(p->singleOp.op);
            break;
            case LABEL_C:
            InterCodePrinter("LABEL ");
            OperandPrint(p->singleOp.op);
            InterCodePrinter(": ");
            break;
            case GOTO_C:
            InterCodePrinter("GOTO ");
            OperandPrint(p->singleOp.op);
            break;
            case READ_C:
            InterCodePrinter("READ ");
            OperandPrint(p->singleOp.op);
            break;
            case WRITE_C:
            InterCodePrinter("WRITE ");
            OperandPrint(p->singleOp.op);
            break;
            case CALL_C:
            OperandPrint(p->assign.left);
            InterCodePrinter(":= CALL ");
            OperandPrint(p->assign.right);
            break;
            case ARGS_C:
            InterCodePrinter("ARG ");
            OperandPrint(p->singleOp.op);
            break;
            case IFGOTO_C:
            InterCodePrinter("IF ");
            OperandPrint(p->tripleOp.op1);
            InterCodePrinter(p->tripleOp.opr);
            OperandPrint(p->tripleOp.op2);
            InterCodePrinter("GOTO ");
            OperandPrint(p->tripleOp.label);
            break;
            default:
            assert(0);
            break;
        }
        p = p->next;
    }
    
}

void OperandPrint(Operand op)
{
    if(op == NULL) assert(0);
    switch (op->kind)
    {
    case VARIABLE_O:
    assert(op->value != NULL);
    printf("%s ",op->value);
    break;
    case CONSTANT_O:
    printf("#%s ",op->value);
    break;
    case ADDRESS_O:
    break;
    case FUNCTION_O:
    printf("%s ",op->value);
    break;
    case TEMPORARY_O:
    printf("t%d ",op->varNo);
    break;
    case LABEL_O:
    printf("label%d ",op->varNo);
    break;
    default:
    assert(0);
    break;
    }
}