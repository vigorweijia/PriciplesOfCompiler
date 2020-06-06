#include <assert.h>
#include <stdlib.h>

#include "GenMips32.h"

extern int gStackFrameSize;

void MipsPrint(const char* fileName)
{
    mipsFp = fopen(fileName, "w");
    if(mipsFp == NULL)
    {
        printf("Cannot open file %s\n",fileName);
        exit(-1);
    } 

    fputs(".data",mipsFp);
    fputs("_prompt: .asciiz \"Enter an integer:\"",mipsFp);
    fputs("_ret: .asciiz \"\\n\"",mipsFp);
    fputs(".global main",mipsFp);
    fputs(".text",mipsFp);
    fputs("read:",mipsFp);
    fputs("  li $v0, 4",mipsFp);
    fputs("  la $a0, _prompt",mipsFp);
    fputs("  syscall",mipsFp);
    fputs("  jr $ra",mipsFp);
    fputs("",mipsFp);
    fputs("write:",mipsFp);
    fputs("  li $v0, 1",mipsFp);
    fputs("  syscall",mipsFp);
    fputs("  li $v0, 4",mipsFp);
    fputs("  la $a0, _ret",mipsFp);
    fputs("  syscall",mipsFp);
    fputs("  move $v0, $0",mipsFp);
    fputs("  jr $ra",mipsFp);
    
    MipsPrintTraverse();
}

void MipsPrintTraverse()
{
    InterCode p = gInterCodeHead;
    while(p != NULL)
    {
        MipsPrint4InterCode(p);
        p = p->next;
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
            fprintf(mipsFp, "  move %s, 0(%s)\n", gRegName[rx], gRegName[ry]);
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
            fprintf(mipsFp, "  addi %s, %s, -%s\n", gRegName[rx], gRegName[ry], op2->value);
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
            //TODO
            fprintf(mipsFp, "  addi %s, %s, -%s\n", gRegName[rx], gRegName[rz], op1->value);
            break;
        default:
            assert(0);
            break;
        }
    }
    else if(op1->kind == CONSTANT_O && op2->kind == CONSTANT_O)
    {
        //TODO: x := #y + #z
        assert(0);
    }

    Reg2Mem(rx);
}

void Mips4Func(InterCode p)
{
    Operand op = p->singleOp.op;
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
        fprintf(mipsFp, "  $a%d, %d($fp)\n", gParamCount, paramSym->offset);
    }
    else
    {
        fprintf(mipsFp, "  lw $s0, %d($fp)\n", (gParamCount-2)*4);
        fprintf(mipsFp, "  sw $s0, %d($fp)\n", paramSym->offset);
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
        fprintf(mipsFp, "  move $v0, %s\n", op->value);
    }
    else if(op->kind == VARIABLE_O || op->kind == CONSTANT_O)
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
    fprintf(mipsFp, "%label%d:\n", op->varNo);
}

void Mips4Goto(InterCode p)
{
    Operand op = p->singleOp.op;
    fprintf(mipsFp, "  j %s\n", op->value);
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
    if(op->kind == TEMPORARY_O || op->kind == VARIABLE_O)
    {
        fprintf(mipsFp, "  move $a0, %s\n", gRegName[rx]);
    }
    else if(op->kind == TADDR_O || op->kind == VADDR_O)
    {
        fprintf(mipsFp, "  lw $a0, 0(%s)\n", gRegName[rx]);
    }
    else assert(0);
    Reg2Mem(rx);
    fprintf(mipsFp, "  addi $sp, $sp, -4\n");
    fprintf(mipsFp, "  sw $ra, 0($sp)\n");
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
}

void Mips4Args(InterCode p)
{
    Operand op = p->singleOp.op;
    SymInfo argSym = NULL;
    assert(op->kind == VARIABLE_O || op->kind == TEMPORARY_O);
    if(op->kind == VARIABLE_O)
    {
        argSym = GetSymbolInfo(op->value);
    }
    else
    {
        char* name = (char*)malloc(20);
        sprintf(name, "t%d", op->varNo);
        argSym = GetSymbolInfo(name);
    }
    
    if(gArgsCount < 4)
    {
        fprintf(mipsFp, "  lw $a%d, %d($fp)\n", gArgsCount, argSym->offset);
    }
    else
    {
        fprintf(mipsFp, "  lw $s0, %d($fp)\n", argSym->offset);
        fprintf(mipsFp, "  addi $sp, $sp, -4\n");
        fprintf(mipsFp, "  sw $s0, 0($sp)\n");
    }
    
    gArgsCount++;
    if(p->next == NULL || p->next->kind != ARGS_C) gArgsCount = 0;
}

void Mips4Ifgoto(InterCode p)
{
    Operand op1 = p->tripleOp.op1;
    Operand op2 = p->tripleOp.op2;
    Operand label = p->tripleOp.label;
    char* opr = p->tripleOp.opr;
    Register rx, ry;

    if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && (op2->kind == TEMPORARY_O && op2->kind == VARIABLE_O))
    {
        rx = AssignRegister(op1);
        ry = AssignRegister(op2);
        if(opr[0] == '=')
        {
            //IF x == y GOTO z
            //beq r(x), r(y), z
            fprintf(mipsFp, "  beq %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
        }
        else if(opr[0] == '>')
        {
            if(opr[1] == '=')
            {
                //IF x >= y GOTO z
                //bge r(x), r(y), z
                fprintf(mipsFp, "  bge %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
            }
            else
            {
                //IF x > y GOTO z
                //bgt r(x), r(y), z
                fprintf(mipsFp, "  bgt %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
            }   
        }
        else if(opr[0] == '<')
        {
            if(opr[1] == '=')
            {
                //IF x <= y GOTO z
                //ble r(x), r(y), z
                fprintf(mipsFp, "  ble %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
            }
            else
            {
                //IF x < y GOTO z
                //blt r(x), r(y), z
                fprintf(mipsFp, "  blt %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
            }
        }
        else if(opr[0] == '!')
        {
            //IF x != y GOTO z
            //bne r(x), r(y), z
            fprintf(mipsFp, "  bne %s, %s, %s\n",gRegName[rx],gRegName[ry],label->value);
        }
        else assert(0);   
    }
    else if((op1->kind == TEMPORARY_O || op1->kind == VARIABLE_O) && op2->kind == CONSTANT_O)
    {
        rx = AssignRegister(op1);
        if(opr[0] == '=')
        {
            //IF x == #k GOTO z
            //beq r(x), k, z
            fprintf(mipsFp, "  beq %s, %s, %s\n",gRegName[rx],op2->value,label->value);
        }
        else if(opr[0] == '>')
        {
            if(opr[1] == '=')
            {
                //IF x >= #k GOTO z
                //bge r(x), k, z
                fprintf(mipsFp, "  bge %s, %s, %s\n",gRegName[rx],op2->value,label->value);
            }
            else
            {
                //IF x > #k GOTO z
                //bgt r(x), r(y), z
                fprintf(mipsFp, "  bgt %s, %s, %s\n",gRegName[rx],op2->value,label->value);
            }   
        }
        else if(opr[0] == '<')
        {
            if(opr[1] == '=')
            {
                //IF x <= #k GOTO z
                //ble r(x), k, z
                fprintf(mipsFp, "  ble %s, %s, %s\n",gRegName[rx],op2->value,label->value);
            }
            else
            {
                //IF x < #k GOTO z
                //blt r(x), k, z
                fprintf(mipsFp, "  blt %s, %s, %s\n",gRegName[rx],op2->value,label->value);
            }
        }
        else if(opr[0] == '!')
        {
            //IF x != #k GOTO z
            //bne r(x), k, z
            fprintf(mipsFp, "  bne %s, %s, %s\n",gRegName[rx],op2->value,label->value);
        }
        else assert(0);   
    }
    else if(op1->kind == CONSTANT_O && (op2->kind == TEMPORARY_O || op2->kind == VARIABLE_O))
    {
        ry = AssignRegister(op2);
        if(opr[0] == '=')
        {
            //IF #k == y GOTO z
            //beq r(y), k, z
            fprintf(mipsFp, "  beq %s, %s, %s\n",gRegName[ry],op1->value,label->value);
        }
        else if(opr[0] == '>')
        {
            if(opr[1] == '=')
            {
                //IF #k >= y GOTO z
                //ble r(y), k, z
                fprintf(mipsFp, "  ble %s, %s, %s\n",gRegName[ry],op1->value,label->value);
            }
            else
            {
                //IF #k > y GOTO z
                //bgt r(y), k, z
                fprintf(mipsFp, "  blt %s, %s, %s\n",gRegName[ry],op1->value,label->value);
            }   
        }
        else if(opr[0] == '<')
        {
            if(opr[1] == '=')
            {
                //IF #k <= y GOTO z
                //bge r(y), k, z
                fprintf(mipsFp, "  bge %s, %s, %s\n",gRegName[ry],op1->value,label->value);
            }
            else
            {
                //IF #k < y GOTO z
                //bgt r(y), k, z
                fprintf(mipsFp, "  bgt %s, %s, %s\n",gRegName[ry],op1->value,label->value);
            }
        }
        else if(opr[0] == '!')
        {
            //IF #k != y GOTO z
            //bne r(y), k, z
            fprintf(mipsFp, "  bne %s, %s, %s\n",gRegName[ry],op1->value,label->value);
        }
        else assert(0);   
    }
    else
    {
        assert(0);
    }
}

void Mips4Dec(InterCode p)
{
    Operand op = p->dec.op;
    int decSize = p->dec.size;
    SymInfo decSym = NULL;
    char* name = NULL;

    assert(op->kind == TEMPORARY_O || op->kind == VARIABLE_O);
    if(op->kind == TEMPORARY_O)
    {
        name = (char*)malloc(20);
        sprintf(name, "t%d", op->varNo);
    }
    else
    {
        name = op->value;
    }
    decSym = NewSymbol(name, gOffset4Sp, -1);
    gOffset4Sp -= decSize;

    AppendSymbol(decSym);

    fprintf(mipsFp, "  addi $s0, $fp, %d\n",gOffset4Sp);
    fprintf(mipsFp, "  sw $s0, %d($fp)\n",decSym->offset);
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
    fprintf(mipsFp, "  sw %s, %d($fp)\n", gRegName[r], gRegBind2Sym[r]->offset);
}

void Mem2Reg(Register r, SymInfo symInfo)
{
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
    assert(op->kind == TEMPORARY_O || op->kind == VARIABLE_O || op->kind == VADDR_O || op->kind == TADDR_O);
    char* symName = NULL;
    if(op->kind == TEMPORARY_O || op->kind == VADDR_O)
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