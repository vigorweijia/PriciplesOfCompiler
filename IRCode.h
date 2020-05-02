#ifndef __IRCODE_H__
#define __IRCODE_H__

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

typedef struct Operand_{
    enum{UNKOWN_O, VARIABLE_O, CONSTANT_O, ADDRESS_O, FUNCTION_O, TEMPORARY_O, LABEL_O} kind;
    union{
        int varNo;
        char* value;
    };
}Operand_;

typedef struct InterCode_{
    enum{ASSIGN_C, ADD_C, SUB_C, MUL_C, DIV_C, FUNCTION_C, PARAM_C, RETURN_C, LABEL_C, GOTO_C}kind;
    union{
        struct {Operand right, left;} assign;
        struct {Operand result, op1, op2;} doubleOp;
        struct {Operand op;} singleOp;
    };
    InterCode prev;
    InterCode next;
}InterCode_;

void InsertCode(InterCode);
void DeleteCode(InterCode);

Operand NewLabel();
Operand NewTempVar();

extern int gTempVarCount;
extern int gLabelCount;

#endif
