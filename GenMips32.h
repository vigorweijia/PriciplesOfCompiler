#ifndef __GENMIPS32_H__
#define __GENMIPS32_H__

#include <stdio.h>

#include "IRCode.h"

FILE* mipsFp;

typedef int Register;
typedef struct RegInfo_* RegInfo;

typedef struct RegInfo_{
    char* name;
    Register reg;
    RegInfo next;
}RegInfo_;

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

Register AssignRegister(Operand);
char* GetStrRegister(Register);

void Reg2Mem(Register); //sw
void Mem2Reg(Register,RegInfo); //lw

int gStackFrameSize = 0;

#endif