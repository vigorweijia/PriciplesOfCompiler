#ifndef __IRCODE_H__
#define __IRCODE_H__

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef int InterCodeType;

typedef struct Operand_{
    enum{UNKOWN_O, VARIABLE_O, CONSTANT_O, CONSTANT_O, ADDRESS_O, FUNCTION_O, TEMPORARY_O, LABEL_O} kind;
    union{
        int varNo;
        char* value; //do not split int/float at this time, seen as str
    };
    Operand next;
}Operand_;

typedef struct InterCode_{
    enum{ASSIGN_C, ADD_C, SUB_C, MUL_C, DIV_C, FUNCTION_C, PARAM_C, RETURN_C, LABEL_C, GOTO_C, READ_C, WRITE_C, CALL_C, ARGS_C, IFGOTO_C}kind;
    union{
        struct {Operand right, left;} assign;
        struct {Operand result, op1, op2;} doubleOp;
        struct {Operand op;} singleOp;
        struct {Operand op1, op2, label; char *opr;} tripleOp;
    };
    InterCode prev;
    InterCode next;
}InterCode_;

void InsertCode(InterCode);
void DeleteCode(InterCode);

Operand NewLabel();
Operand NewTempVar();
Operand NewConstant(const char* v);
Operand NewFunction(const char* funcName);

InterCode GenSingleOp(Operand,InterCodeType);
InterCode GenDoubleOp(Operand result,Operand op1,Operand op2,InterCodeType t);
InterCode GenTripleOp(Operand op1,Operand op2,Operand label,const char *opr);
InterCode GenAssign(Operand left,Operand right);
InterCode GenCall(Operand left,Operand right);

extern int gTempVarCount;
extern int gLabelCount;

#endif
