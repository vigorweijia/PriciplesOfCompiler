#ifndef __GENMIPS32_H__
#define __GENMIPS32_H__

#include <stdio.h>

#include "IRCode.h"

FILE* mipsFp;

typedef int Register;
typedef struct SymInfo_* SymInfo;

typedef struct SymInfo_{
    char* name;
    int offset;
    Register reg;
    SymInfo next;
}SymInfo_;

void MipsPrint(const char* fileName);
void MipsPrintTraverse();
void MipsPrint4InterCode(InterCode);

void Mips4Assign(InterCode);
void Mips4Op(InterCode);
void Mips4Func(InterCode);
void Mips4Param(InterCode);
void Mips4Return(InterCode);
void Mips4Label(InterCode);
void Mips4Goto(InterCode);
void Mips4Read(InterCode);
void Mips4Write(InterCode);
void Mips4Call(InterCode);
void Mips4Args(InterCode);
void Mips4Ifgoto(InterCode);
void Mips4Dec(InterCode);
void Mips4Addr(InterCode);

char* gRegName[]={
    "$zero","$at","$v0","$v1",
    "$a0","$a1","$a2","$a3",
    "$t0","$t1","$t2","$t3",
    "$t4","$t5","$t6","$t7",
    "$s0","$s1","$s2","$s3",
    "$s4","$s5","$s6","$s7",
    "$t8","$t9","$k0","$k1",
    "$gp","$sp","$fp","$ra"
};
Register AssignRegister(Operand);
Register SelectRegister();

void Reg2Mem(Register); //sw
void Mem2Reg(Register,SymInfo); //lw
SymInfo GetSymbolInfo(const char*);
SymInfo NewSymbol(char*,int,Register);
void AppendSymbol();
void SymbolDestroy(SymInfo);

SymInfo gSymbolHead = NULL;
SymInfo gSymbolTail = NULL;

int gStackFrameSize = 0;

#endif