#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "GenMips32.h"

#define REG_MAX 32

SymInfo gSymbolHead = NULL;
SymInfo gSymbolTail = NULL;

int gStackFrameSize = 256;
int gOffset4Sp = 0;
int gArgsCount = 0;
int gParamCount = 0;
SymInfo gRegBind2Sym[REG_MAX];

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

void MipsPrint(const char* fileName)
{
    mipsFp = fopen(fileName, "w");
    if(mipsFp == NULL)
    {
        printf("Cannot open file %s\n",fileName);
        exit(-1);
    } 

    fputs(".data\n",mipsFp);
    fputs("_prompt: .asciiz \"Enter an integer:\"\n",mipsFp);
    fputs("_ret: .asciiz \"\\n\"\n",mipsFp);
    fputs(".globl main\n",mipsFp);
    fputs(".text\n",mipsFp);
    fputs("read:\n",mipsFp);
    fputs("  li $v0, 4\n",mipsFp);
    fputs("  la $a0, _prompt\n",mipsFp);
    fputs("  syscall\n",mipsFp);
    fputs("  li $v0, 5\n",mipsFp);
    fputs("  syscall\n",mipsFp);
    fputs("  jr $ra\n",mipsFp);
    fputs("\n",mipsFp);
    fputs("write:\n",mipsFp);
    fputs("  li $v0, 1\n",mipsFp);
    fputs("  syscall\n",mipsFp);
    fputs("  li $v0, 4\n",mipsFp);
    fputs("  la $a0, _ret\n",mipsFp);
    fputs("  syscall\n",mipsFp);
    fputs("  move $v0, $0\n",mipsFp);
    fputs("  jr $ra\n",mipsFp);

    MipsPrintTraverse();

    fclose(mipsFp);
}

void MipsPrintTraverse()
{
    InterCode p = gInterCodeHead;
    int cnt = 1;
    while(p != NULL)
    {
        MipsPrint4InterCode(p);
        p = p->next;
        //printf("%d yes\n",cnt++);
    }
}

void MipsPrint4InterCode(InterCode p)
{
    switch (p->kind)
    {
    case ASSIGN_C:
        Mips4Assign(p);
        break;
    case ADD_C:
    case SUB_C:
    case MUL_C:
    case DIV_C:
        Mips4Op(p);
        break;
    case FUNCTION_C:
        Mips4Func(p);
        break;
    case PARAM_C:
        Mips4Param(p);
        break;
    case RETURN_C:
        Mips4Return(p);
        break;
    case LABEL_C:
        Mips4Label(p);
        break;
    case GOTO_C:
        Mips4Goto(p);
        break;
    case READ_C:
        Mips4Read(p);
        break;
    case WRITE_C:
        Mips4Write(p);
        break;
    case CALL_C:
        Mips4Call(p);
        break;
    case ARGS_C:
        Mips4Args(p);
        break;
    case IFGOTO_C:
        Mips4Ifgoto(p);
        break;
    case DEC_C:
        Mips4Dec(p);
        break;
    case ADDR_C:
        Mips4Addr(p);
        break;
    default:
        assert(0);
        break;
    }
}

void Mips4Assign(InterCode p)
{
    Operand left = p->assign.left;
    Operand right = p->assign.right;
    Register rx, ry;
    if(left->kind == VARIABLE_O || left->kind == TEMPORARY_O)
    {
        switch (right->kind)
        {
        case VARIABLE_O:
        case TEMPORARY_O:
            //x := y
            //move r(x), r(y)
            rx = AssignRegister(left);
            ry = AssignRegister(right);
            fprintf(mipsFp, "  move %s, %s\n", gRegName[rx], gRegName[ry]);
            break;
        case TADDR_O:
        case VADDR_O:
            //x := *y
            //lw r(x), 0(r(y))
            rx = AssignRegister(left);
            ry = AssignRegister(right);
            fprintf(mipsFp, "  lw %s, 0(%s)\n", gRegName[rx], gRegName[ry]);
            break;
        case CONSTANT_O:
            //r1 := #k
            //li r(x), k
            rx = AssignRegister(left);
            fprintf(mipsFp, "  li %s, %s\n", gRegName[rx], right->value);
            break;
        default:
            break;
        }
    }
    else if(left->kind == VADDR_O || left->kind == TADDR_O)
    {
        if(right->kind == TEMPORARY_O || right->kind == VARIABLE_O)
        {
            rx = AssignRegister(left);
            ry = AssignRegister(right);
            fprintf(mipsFp, "  sw %s, 0(%s)\n", gRegName[ry], gRegName[rx]); 
        }
        else if(right->kind == CONSTANT_O)
        {
            rx = AssignRegister(left);
            fprintf(mipsFp, "  li $s4, %s\n", right->value);
            fprintf(mipsFp, "  sw $s4, 0(%s)\n", gRegName[rx]);
        }
        else if(right->kind == VADDR_O || right->kind == TADDR_O)
        {
            rx = AssignRegister(left);
            ry = AssignRegister(right);
            fprintf(mipsFp, "  lw $s4, 0(%s)\n", gRegName[ry]);
            fprintf(mipsFp, "  sw $s4, 0(%s)\n", gRegName[rx]);
        }
        else assert(0);
    }
    else
    {
        assert(0);
    }

    Reg2Mem(rx);
}

void Mips4Op(InterCode p)
{
    Operand result = p->doubleOp.result;
    Operand op1 = p->doubleOp.op1;
    Operand op2 = p->doubleOp.op2;
    Register rx, ry, rz;

    assert(result->kind == TEMPORARY_O || result->kind == VARIABLE_O);

    if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && (op2->kind == TEMPORARY_O || op2->kind == VARIABLE_O))
    {
        //x := y + z
        rx = AssignRegister(result);
        ry = AssignRegister(op1);
        rz = AssignRegister(op2);
        switch (p->kind)
        {
        case ADD_C:
            fprintf(mipsFp, "  add %s, %s, %s\n", gRegName[rx], gRegName[ry], gRegName[rz]);
            break;
        case SUB_C:
            fprintf(mipsFp, "  sub %s, %s, %s\n", gRegName[rx], gRegName[ry], gRegName[rz]);
            break;
        case MUL_C:
            fprintf(mipsFp, "  mul %s, %s, %s\n", gRegName[rx], gRegName[ry], gRegName[rz]);
            break;
        case DIV_C:
            fprintf(mipsFp, "  div %s, %s\n", gRegName[ry], gRegName[rz]);
            fprintf(mipsFp, "  mflo %s\n", gRegName[rx]);
            break;
        default:
            break;
        }
    }
    else if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && op2->kind == CONSTANT_O)
    {
        //x := y + #z
        rx = AssignRegister(result);
        ry = AssignRegister(op1);
        switch (p->kind)
        {
        case ADD_C:
            fprintf(mipsFp, "  addi %s, %s, %s\n", gRegName[rx], gRegName[ry], op2->value);
            break;
        case SUB_C:
            fprintf(mipsFp, "  subu %s, %s, %s\n", gRegName[rx], gRegName[ry], op2->value);
            break;
        case MUL_C:
            fprintf(mipsFp, "  li $s4, %s\n", op2->value);
            fprintf(mipsFp, "  mul %s, %s, $s4\n", gRegName[rx], gRegName[ry]);
            break;
        case DIV_C:
            fprintf(mipsFp, "  li $s4, %s\n", op2->value);
            fprintf(mipsFp, "  div %s, $s4\n", gRegName[ry]);
            fprintf(mipsFp, "  mflo %s\n", gRegName[rx]);
            break;
        default:
            assert(0);
            break;
        }
    }
    else if(op1->kind == CONSTANT_O && (op2->kind == TEMPORARY_O || op2->kind == VARIABLE_O))
    {
        //x := #y + z
        rx = AssignRegister(result);
        rz = AssignRegister(op2);
        switch (p->kind)
        {
        case ADD_C:
            fprintf(mipsFp, "  addi %s, %s, %s\n", gRegName[rx], gRegName[rz], op1->value);
            break;
        case SUB_C:
            fprintf(mipsFp, "  li $s4, %s\n", op1->value);
            fprintf(mipsFp, "  subu %s, $s4, %s\n", gRegName[rx], gRegName[rz]);
            break;
        case MUL_C:
            fprintf(mipsFp, "  li $s4, %s\n", op1->value);
            fprintf(mipsFp, "  mul %s, %s, $s4\n", gRegName[rx], gRegName[rz]);
            break;
        case DIV_C:
            fprintf(mipsFp, "  li $s4, %s\n", op1->value);
            fprintf(mipsFp, "  div $s4, %s\n", gRegName[rz]);
            fprintf(mipsFp, "  mflo %s\n", gRegName[rx]);
            break;
        default:
            assert(0);
            break;
        }
    }
    else
    {
        //x := #k1 + #k2
        rx = AssignRegister(result);
        switch (p->kind)
        {
        case ADD_C:
            fprintf(mipsFp, "  li $s4, %s\n", op1->value);
            fprintf(mipsFp, "  addi %s, $s4, %s\n", gRegName[rx], op2->value);
            break;
        case SUB_C:
            fprintf(mipsFp, "  li $s4, %s\n", op1->value);
            fprintf(mipsFp, "  subu %s, $s4, %s\n", gRegName[rx], op2->value);
            break;
        default:
            assert(0);
            break;
        }
    }

    Reg2Mem(rx);
}

void Mips4Func(InterCode p)
{
    Operand op = p->singleOp.op;
    fprintf(mipsFp, "\n");
    fprintf(mipsFp, "%s:\n", op->value);
    fprintf(mipsFp, "  addi $sp, $sp, -4\n"); //$sp = $sp - 4
    fprintf(mipsFp, "  sw $fp, 0($sp)\n"); //0($sp) = old fp
    fprintf(mipsFp, "  move $fp, $sp\n");//$fp = $sp 
    fprintf(mipsFp, "  subu $sp, $sp, %d\n",gStackFrameSize);//$sp = $sp - gStackFrameSize
    gOffset4Sp = 0;
}

void Mips4Param(InterCode p)
{
    Operand op = p->singleOp.op;
    gOffset4Sp = gOffset4Sp - 4;
    SymInfo paramSym = NewSymbol(op->value, gOffset4Sp, -1);
    AppendSymbol(paramSym);

    if(gParamCount < 4)
    {
        fprintf(mipsFp, "  sw $a%d, %d($fp)\n", gParamCount, paramSym->offset);
    }
    else
    {
        fprintf(mipsFp, "  lw $s2, %d($fp)\n", (gParamCount-2)*4);
        fprintf(mipsFp, "  sw $s2, %d($fp)\n", paramSym->offset);
    }
    gParamCount++;
    if(p->next == NULL || p->next->kind != ARGS_C) gParamCount = 0;
}

void Mips4Return(InterCode p)
{
    Operand op = p->singleOp.op;
    Register rx;
    if(op->kind == CONSTANT_O)
    {
        fprintf(mipsFp, "  li $s3, %s\n",op->value);
        fprintf(mipsFp, "  move $v0, $s3\n");
    }
    else if(op->kind == VARIABLE_O || op->kind == TEMPORARY_O)
    {
        rx = AssignRegister(op);
        fprintf(mipsFp, "  move $v0, %s\n", gRegName[rx]);
    }
    fprintf(mipsFp, "  addi $sp, $sp, %d\n", gStackFrameSize);
    fprintf(mipsFp, "  lw $fp, 0($sp)\n");
    fprintf(mipsFp, "  addi $sp, $sp, 4\n");
    fprintf(mipsFp, "  jr $ra\n");
}

void Mips4Label(InterCode p)
{
    Operand op = p->singleOp.op;
    fprintf(mipsFp, "label%d:\n", op->varNo);
}

void Mips4Goto(InterCode p)
{
    Operand op = p->singleOp.op;
    if(op->kind == LABEL_O)
    {
        fprintf(mipsFp, "  j label%d\n", op->varNo);
    }
    else
    {
        fprintf(mipsFp, "  j %s\n", op->value);
    }
}

void Mips4Read(InterCode p)
{
    Operand op = p->singleOp.op;
    Register rx = AssignRegister(op);
    //addi $sp, $sp, -4
    //sw $ra, 0($sp)
    //jal read
    //move r(x), $v0
    //sw r(x)
    //lw $ra, 0($sp)
    //addi $sp, $sp, 4
    fprintf(mipsFp, "  addi $sp, $sp, -4\n");
    fprintf(mipsFp, "  sw $ra, 0($sp)\n");
    fprintf(mipsFp, "  jal read\n");
    fprintf(mipsFp, "  move %s, $v0\n", gRegName[rx]);
    Reg2Mem(rx);
    fprintf(mipsFp, "  lw $ra, 0($sp)\n");
    fprintf(mipsFp, "  addi $sp, $sp, 4\n");
}

void Mips4Write(InterCode p)
{
    Operand op = p->singleOp.op;
    Register rx = AssignRegister(op);
    //move $a0, r(x)
    //sw r(x)
    //addi $sp, $sp, -4
    //sw $ra, 0($sp)
    //jal write
    //lw $ra, 0($sp)
    //addi $sp, $sp, 4
    
    
    fprintf(mipsFp, "  addi $sp, $sp, -4\n");
    fprintf(mipsFp, "  sw $ra, 0($sp)\n");

    if(op->kind == TEMPORARY_O || op->kind == VARIABLE_O)
    {
        fprintf(mipsFp, "  move $a0, %s\n", gRegName[rx]);
    }
    else if(op->kind == TADDR_O || op->kind == VADDR_O)
    {
        fprintf(mipsFp, "  lw $a0, 0(%s)\n", gRegName[rx]);
    }
    else if(op->kind == CONSTANT_O)
    {
        fprintf(mipsFp, "  li $a0, %s\n", op->value);
    }
    else assert(0);
    Reg2Mem(rx);

    fprintf(mipsFp, "  jal write\n");
    fprintf(mipsFp, "  lw $ra, 0($sp)\n");
    fprintf(mipsFp, "  addi $sp, $sp, 4\n");
}

void Mips4Call(InterCode p)
{
    Operand left = p->assign.left;
    Operand right = p->assign.right;
    Register rx = AssignRegister(left);
    //x := CALL f
    //addi $sp, $sp, -4
    //sw $ra, 0($sp)
    //jal f
    //move r(x), $v0
    //sw r(x), 
    //lw $ra, 0($sp)
    //addi $sp, $sp, 4
    fprintf(mipsFp, "  addi $sp, $sp, -4\n");
    fprintf(mipsFp, "  sw $ra, 0($sp)\n");
    fprintf(mipsFp, "  jal %s\n", right->value);
    fprintf(mipsFp, "  move %s, $v0\n", gRegName[rx]);
    Reg2Mem(rx);
    fprintf(mipsFp, "  lw $ra, 0($sp)\n");
    fprintf(mipsFp, "  addi $sp, $sp, 4\n");
    gArgsCount = 0;
}

void Mips4Args(InterCode p)
{
    Operand op = p->singleOp.op;
    SymInfo argSym = NULL;
    if(op->kind == VARIABLE_O || op->kind == VADDR_O)
    {
        argSym = GetSymbolInfo(op->value);
    }
    else if(op->kind == TEMPORARY_O || op->kind == TADDR_O)
    {
        char* name = (char*)malloc(20);
        sprintf(name, "t%d", op->varNo);
        argSym = GetSymbolInfo(name);
    }
    else if(op->kind == CONSTANT_O)
    {
        
    }
    else
    {
        assert(0);
    }
    
    
    if(gArgsCount < 4)
    {
        if(op->kind == VADDR_O || op->kind == TADDR_O)
        {
            Register r = AssignRegister(op);
            fprintf(mipsFp, "  lw $s4, 0(%s)\n", gRegName[r]);
            Reg2Mem(r);
            fprintf(mipsFp, "  move $a%d, $s4\n", gArgsCount);
        }
        else if(op->kind == TEMPORARY_O || op->kind == VARIABLE_O)
        {
            fprintf(mipsFp, "  lw $a%d, %d($fp)\n", gArgsCount, argSym->offset);
        }
        else
        {
            fprintf(mipsFp, "  li $a%d, %s\n", gArgsCount, op->value);
        }
    }
    else
    {   if(op->kind == VADDR_O || op->kind == TADDR_O)
        {
            Register r = AssignRegister(op);
            fprintf(mipsFp, "  lw $s0, 0(%s)\n", gRegName[r]);
            Reg2Mem(r);
            fprintf(mipsFp, "  addi $sp, $sp, -4\n");
            fprintf(mipsFp, "  sw $s0, 0($sp)\n");
        }
        else if(op->kind == TEMPORARY_O || op->kind == VARIABLE_O)
        {
            fprintf(mipsFp, "  lw $s0, %d($fp)\n", argSym->offset);
            fprintf(mipsFp, "  addi $sp, $sp, -4\n");
            fprintf(mipsFp, "  sw $s0, 0($sp)\n");
        }
        else
        {
            fprintf(mipsFp, "  li $s0, %s\n", op->value);
            fprintf(mipsFp, "  addi $sp, $sp, -4\n");
            fprintf(mipsFp, "  sw $s0, 0($sp)\n");
        }
    }
    
    gArgsCount++;
    if(p->next == NULL || p->next->kind != ARGS_C) gArgsCount = 0;
}

void Mips4Ifgoto(InterCode p)
{
    Operand op1 = p->tripleOp.op1;
    Operand op2 = p->tripleOp.op2;
    Operand label = p->tripleOp.label;
    char* opr = (char*)malloc(5);
    Register rx, ry;
    strcpy(opr,p->tripleOp.opr);

    //printf("op1kind:%d op2kind:%d\n",op1->kind, op2->kind);

    if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && (op2->kind == TEMPORARY_O || op2->kind == VARIABLE_O))
    {
        rx = AssignRegister(op1);
        ry = AssignRegister(op2);
        if(strcmp(opr,"==") == 0)
        {
            //IF x == y GOTO z
            //beq r(x), r(y), z
            fprintf(mipsFp, "  beq %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else if(strcmp(opr,">=") == 0)
        {
            //IF x >= y GOTO z
            //bge r(x), r(y), z
            fprintf(mipsFp, "  bge %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else if(strcmp(opr,">") == 0)
        {
            //IF x > y GOTO z
            //bgt r(x), r(y), z
            fprintf(mipsFp, "  bgt %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else if(strcmp(opr,"<=") == 0)
        {
            //IF x <= y GOTO z
            //ble r(x), r(y), z
            fprintf(mipsFp, "  ble %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else if(strcmp(opr,"<") == 0)
        {
            //IF x < y GOTO z
            //blt r(x), r(y), z
            fprintf(mipsFp, "  blt %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else if(strcmp(opr,"!=") == 0)
        {
            //IF x != y GOTO z
            //bne r(x), r(y), z
            fprintf(mipsFp, "  bne %s, %s, label%d\n",gRegName[rx],gRegName[ry],label->varNo);
        }
        else assert(0);   
    }
    else if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && op2->kind == CONSTANT_O)
    {
        rx = AssignRegister(op1);
        if(strcmp(opr, "==") == 0)
        {
            //IF x == #k GOTO z
            //beq r(x), k, z
            fprintf(mipsFp, "  beq %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }
        else if(strcmp(opr, ">=") == 0)
        {
            //IF x >= #k GOTO z
            //bge r(x), k, z
            fprintf(mipsFp, "  bge %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }
        else if(strcmp(opr, ">") == 0)
        {
            //IF x > #k GOTO z
            //bgt r(x), r(y), z
            fprintf(mipsFp, "  bgt %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }   
        else if(strcmp(opr, "<=") == 0)
        {
            //IF x <= #k GOTO z
            //ble r(x), k, z
            fprintf(mipsFp, "  ble %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }
        else if(strcmp(opr, "<") == 0)
        {
            //IF x < #k GOTO z
            //blt r(x), k, z
            fprintf(mipsFp, "  blt %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }
        else if(strcmp(opr, "!=") == 0)
        {
            //IF x != #k GOTO z
            //bne r(x), k, z
            fprintf(mipsFp, "  bne %s, %s, label%d\n",gRegName[rx],op2->value,label->varNo);
        }
        else assert(0);   
    }
    else if(op1->kind == CONSTANT_O && (op2->kind == TEMPORARY_O || op2->kind == VARIABLE_O))
    {
        ry = AssignRegister(op2);
        if(strcmp(opr, "==") == 0)
        {
            //IF #k == y GOTO z
            //beq r(y), k, z
            fprintf(mipsFp, "  beq %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }
        else if(strcmp(opr, ">=") == 0)
        {
            //IF #k >= y GOTO z
            //ble r(y), k, z
            fprintf(mipsFp, "  ble %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }
        else if(strcmp(opr, ">") == 0)
        {
            //IF #k > y GOTO z
            //bgt r(y), k, z
            fprintf(mipsFp, "  blt %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }   
        else if(strcmp(opr, "<=") == 0)
        {
            //IF #k <= y GOTO z
            //bge r(y), k, z
            fprintf(mipsFp, "  bge %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }
        else if(strcmp(opr, "<") == 0)
        {
            //IF #k < y GOTO z
            //bgt r(y), k, z
            fprintf(mipsFp, "  bgt %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }
        else if(strcmp(opr, "!=") == 0)
        {
            //IF #k != y GOTO z
            //bne r(y), k, z
            fprintf(mipsFp, "  bne %s, %s, label%d\n",gRegName[ry],op1->value,label->varNo);
        }
        else assert(0);   
    }
    else if ((op1->kind == VADDR_O || op1->kind == TADDR_O) && (op2->kind == VADDR_O || op2->kind == TADDR_O))
    {
        rx = AssignRegister(op1);
        ry = AssignRegister(op2);
        fprintf(mipsFp, "  lw $s4, 0(%s)\n",gRegName[rx]);
        fprintf(mipsFp, "  lw $s5, 0(%s)\n",gRegName[ry]);
        if(strcmp(opr,"==") == 0)
        {
            fprintf(mipsFp, "  beq $s4, $s5, label%d\n",label->varNo);
        }
        else if(strcmp(opr,">=") == 0)
        {
            fprintf(mipsFp, "  bge $s4, $s5, label%d\n",label->varNo);
        }
        else if(strcmp(opr,">") == 0)
        {
            fprintf(mipsFp, "  bgt $s4, $s5, label%d\n",label->varNo);
        }
        else if(strcmp(opr,"<=") == 0)
        {
            fprintf(mipsFp, "  ble $s4, $s5, label%d\n",label->varNo);
        }
        else if(strcmp(opr,"<") == 0)
        {
            fprintf(mipsFp, "  blt $s4, $s5, label%d\n",label->varNo);
        }
        else if(strcmp(opr,"!=") == 0)
        {
            fprintf(mipsFp, "  bne $s4, $s5, label%d\n",label->varNo);
        }
    }

}

void Mips4Dec(InterCode p)
{
    Operand op = p->dec.op;
    int decSize = p->dec.size;
    SymInfo decSym = NULL;

    gOffset4Sp = gOffset4Sp - 4;

    assert(op->kind == TEMPORARY_O || op->kind == VARIABLE_O);
    if(op->kind == TEMPORARY_O)
    {
        char* name = (char*)malloc(20);
        sprintf(name, "t%d", op->varNo);
        decSym = NewSymbol(name, gOffset4Sp, -1);
    }
    else
    {
        decSym = NewSymbol(op->value, gOffset4Sp, -1);
    }
    
    gOffset4Sp -= decSize;

    AppendSymbol(decSym);

    fprintf(mipsFp, "  addi $s1, $fp, %d\n",gOffset4Sp);
    fprintf(mipsFp, "  sw $s1, %d($fp)\n",decSym->offset);
}

void Mips4Addr(InterCode p)
{
    Operand left = p->assign.left;
    Operand right = p->assign.right;
    SymInfo symInfo = NULL;
    Register r;
    assert(right->kind == TEMPORARY_O || right->kind == VARIABLE_O);
    if(right->kind == TEMPORARY_O)
    {
        char* name = (char*)malloc(20);
        sprintf(name, "t%d", right->varNo);
        symInfo = GetSymbolInfo(name);
    }
    else
    {
        symInfo = GetSymbolInfo(right->value);
    }
    
    r = AssignRegister(left);
    fprintf(mipsFp, "  lw %s, %d($fp)\n", gRegName[r], symInfo->offset);
    Reg2Mem(r);
}

void Reg2Mem(Register r)
{
    if(r == -1) return;
    fprintf(mipsFp, "  sw %s, %d($fp)\n", gRegName[r], gRegBind2Sym[r]->offset);
}

void Mem2Reg(Register r, SymInfo symInfo)
{
    if(r == -1) return;
    gRegBind2Sym[r] = symInfo;
    fprintf(mipsFp, "  lw %s, %d($fp)\n",gRegName[r], symInfo->offset);
}

Register SelectRegister()
{
    static int freeReg = 7;
    freeReg = (freeReg + 1)%8 + 8;
    return (Register)freeReg;
}

Register AssignRegister(Operand op)
{
    if(op->kind == CONSTANT_O) return -1;
    assert(op->kind == TEMPORARY_O || op->kind == VARIABLE_O || op->kind == VADDR_O || op->kind == TADDR_O);
    char* symName = NULL;
    if(op->kind == TEMPORARY_O || op->kind == TADDR_O)
    {
        symName = (char*)malloc(20);
        sprintf(symName,"t%d",op->varNo);
    }
    else
    {
        symName = op->value;
    }

    SymInfo symInfo = GetSymbolInfo(symName);
    Register r = SelectRegister();
    if(symInfo == NULL)
    {
        gOffset4Sp -= 4;
        symInfo = NewSymbol(symName, gOffset4Sp, r);
        AppendSymbol(symInfo);
        gRegBind2Sym[r] = symInfo;
    }
    else
    {
        symInfo->reg = r;
        Mem2Reg(r, symInfo);
    }
    return r;
}

SymInfo GetSymbolInfo(const char* name)
{
    SymInfo p = gSymbolHead;
    while(p != NULL)
    {
        if(strcmp(p->name, name) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

SymInfo NewSymbol(char* name, int offset, Register reg)
{
    SymInfo symInfo = (SymInfo)malloc(sizeof(SymInfo_));
    symInfo->name = name;
    symInfo->offset = offset;
    symInfo->reg = reg;
    symInfo->next = NULL;
    return symInfo;
}

void AppendSymbol(SymInfo symInfo)
{
    assert(symInfo != NULL);
    if(gSymbolHead == NULL)
    {
        gSymbolHead = symInfo;
        gSymbolTail = symInfo;
    }
    else
    {
        gSymbolTail->next = symInfo;
        gSymbolTail = symInfo;
    }
}

void DestroySymbol(SymInfo p)
{
    if(p == NULL) return;
    while(p != NULL)
    {
        SymInfo q = p;
        p = p->next;
        free(q);
    }
}