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

Operand NewConstant(char* v)
{
    Operand tempConstant = (Operand)malloc(sizeof(Operand_));
    memset(tempConstant, 0, sizeof(tempConstant));
    tempConstant->kind = CONSTANT_O;
    tempConstant->value = v;
    tempConstant->next = NULL;
    return tempConstant;
}

Operand NewFunction(char* funcName)
{
    Operand tempFunc = (Operand)malloc(sizeof(Operand_));
    memset(tempFunc, 0, sizeof(tempFunc));
    tempFunc->kind = FUNCTION_O;
    tempFunc->value = funcName;
    tempFunc->next = NULL;
    return tempFunc;
}

Operand NewVariable(char* varName)
{
    Operand tempVar = (Operand)malloc(sizeof(Operand_));
    memset(tempVar, 0, sizeof(tempVar));
    tempVar->kind = VARIABLE_O;
    tempVar->value = varName;
    tempVar->next = NULL;
    return tempVar;
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

InterCode GenTripleOp(Operand op1, Operand op2, Operand label, char* opr)
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

InterCode GenAddr(Operand left, Operand right)
{
    InterCode tempAddr = (InterCode)malloc(sizeof(InterCode_));
    memset(tempAddr, 0, sizeof(InterCode_));
    tempAddr->kind = ADDR_C;
    tempAddr->assign.left = left;
    tempAddr->assign.right = right;
    return tempAddr;
}

void InterCodeEndLine(FILE* fp)
{
#ifdef PrintToConsole
    printf("\n");
#endif
    fprintf(fp,"\n");
}

void InterCodePrinter(const char* msg, FILE* fp)
{
#ifdef PrintToConsole
    printf("%s",msg);
#endif
    fprintf(fp,"%s",msg);
}

void InterCodePrintCal(InterCode p, FILE* fp)
{
    OperandPrint(p->doubleOp.result, fp);
    InterCodePrinter(":= ", fp);
    OperandPrint(p->doubleOp.op1, fp);
    switch (p->kind)
    {
    case ADD_C:
        InterCodePrinter("+ ", fp);
        break;
    case SUB_C:
        InterCodePrinter("- ", fp);
        break;
    case MUL_C:
        InterCodePrinter("* ", fp);
        break;
    case DIV_C:
        InterCodePrinter("/ ", fp);
        break;
    default:
        break;
    } 
    OperandPrint(p->doubleOp.op2, fp);
}

void InterCodePrint(const char* outputFileName)
{
    FILE* fp = fopen(outputFileName, "w");
    if(fp == NULL)
    {
        printf("Cannot open file %s.\n",outputFileName);
        exit(-1);
    }
    InterCode p = gInterCodeHead;
    while (p != NULL)
    {
        switch(p->kind)
        {
            case ASSIGN_C:
            OperandPrint(p->assign.left, fp);
            InterCodePrinter(":= ", fp);
            OperandPrint(p->assign.right, fp);
            break;
            case ADD_C:
            case SUB_C:
            case MUL_C:
            case DIV_C:
            InterCodePrintCal(p, fp);
            break;
            case FUNCTION_C:
            InterCodePrinter("FUNCTION ", fp);
            OperandPrint(p->singleOp.op, fp);
            InterCodePrinter(":", fp);
            break;
            case PARAM_C:
            InterCodePrinter("PARAM ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case RETURN_C:
            InterCodePrinter("RETURN ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case LABEL_C:
            InterCodePrinter("LABEL ", fp);
            OperandPrint(p->singleOp.op, fp);
            InterCodePrinter(": ", fp);
            break;
            case GOTO_C:
            InterCodePrinter("GOTO ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case READ_C:
            InterCodePrinter("READ ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case WRITE_C:
            InterCodePrinter("WRITE ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case CALL_C:
            OperandPrint(p->assign.left, fp);
            InterCodePrinter(":= CALL ", fp);
            OperandPrint(p->assign.right, fp);
            break;
            case ARGS_C:
            InterCodePrinter("ARG ", fp);
            OperandPrint(p->singleOp.op, fp);
            break;
            case IFGOTO_C:
            InterCodePrinter("IF ", fp);
            OperandPrint(p->tripleOp.op1, fp);
            InterCodePrinter(p->tripleOp.opr, fp);
            InterCodePrinter(" ", fp);
            OperandPrint(p->tripleOp.op2, fp);
            InterCodePrinter("GOTO ", fp);
            OperandPrint(p->tripleOp.label, fp);
            break;
            case DEC_C:
            InterCodePrinter("DEC ", fp);
            OperandPrint(p->dec.op, fp);
            char* sizeStr = (char*)malloc(20);
            sprintf(sizeStr,"%d",p->dec.size);
            InterCodePrinter(sizeStr, fp);
            break;
            case ADDR_C:
            OperandPrint(p->assign.left, fp);
            InterCodePrinter(":= &",fp);
            OperandPrint(p->assign.right,fp);
            break;
            default:
            assert(0);
            break;
        }
        InterCodeEndLine(fp);
        p = p->next;
    }
    
}

void OperandPrint(Operand op, FILE* fp)
{
    if(op == NULL) assert(0);
    switch (op->kind)
    {
    case VARIABLE_O:
    assert(op->value != NULL);
#ifdef PrintToConsole
    printf("%s ",op->value);
#endif
    fprintf(fp,"%s ",op->value);
    break;
    case CONSTANT_O:
#ifdef PrintToConsole
    printf("#%s ",op->value);
#endif
    fprintf(fp,"#%s ",op->value);
    break;
    case FUNCTION_O:
#ifdef PrintToConsole
    printf("%s ",op->value);
#endif
    fprintf(fp,"%s ",op->value);
    break;
    case TEMPORARY_O:
#ifdef PrintToConsole
    printf("t%d ",op->varNo);
#endif
    fprintf(fp,"t%d ",op->varNo);
    break;
    case LABEL_O:
#ifdef PrintToConsole
    printf("label%d ",op->varNo);
#endif
    fprintf(fp,"label%d ",op->varNo);
    break;
    case VADDR_O:
#ifdef PrintToConsole
    printf("*%s ",op->value);
#endif
    fprintf(fp,"*%s ",op->value);
    break;
    case TADDR_O:
#ifdef PrintToConsole
    printf("*t%d ",op->varNo);
#endif
    fprintf(fp,"*t%d ",op->varNo);
    break;
    default:
    assert(0);
    break;
    }
}