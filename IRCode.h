#ifndef __IRCODE_H__
#define __IRCODE_H__

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

typedef struct Operand_{
    enum{VARIABLE, CONSTANT, ADDRESS} kind;
    union{
        int varNo;
        char* value;
    };
}Operand_;

typedef struct InterCode_{
    enum{ASSIGN, ADD, SUB, MUL, DIV}kind;
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

#endif
