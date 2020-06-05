#include <assert.h>

#include "GenMips32.h"

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
            fprintf(mipsFp, "  move %s, %s\n", GetStrRegister(rx), GetStrRegister(ry));
            break;
        case TADDR_O:
        case VADDR_O:
            //x := *y
            //lw r(x), 0(r(y))
            rx = AssignRegister(left);
            ry = AssignRegister(right);
            fprintf(mipsFp, "  move %s, 0(%s)\n", GetStrRegister(rx), GetStrRegister(ry));
            break;
        case CONSTANT_O:
            //r1 := #k
            //li r(x), k
            rx = AssignRegister(left);
            fprintf(mipsFp, "  li %s, %s\n", GetStrRegister(rx), right->value);
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
            fprintf(mipsFp, "  sw %s, 0(%s)\n", GetStrRegister(ry), GetStrRegister(rx)); 
        }
    }
    else
    {
        assert(0);
    }
    
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
            fprintf(mipsFp, "  add %s, %s, %s\n", GetStrRegister(rx), GetStrRegister(ry), GetStrRegister(rz));
            break;
        case SUB_C:
            fprintf(mipsFp, "  sub %s, %s, %s\n", GetStrRegister(rx), GetStrRegister(ry), GetStrRegister(rz));
            break;
        case MUL_C:
            fprintf(mipsFp, "  mul %s, %s, %s\n", GetStrRegister(rx), GetStrRegister(ry), GetStrRegister(rz));
            break;
        case DIV_C:
            fprintf(mipsFp, "  div %s, %s\n", GetStrRegister(ry), GetStrRegister(rz));
            fprintf(mipsFp, "  mflo %s\n", GetStrRegister(rx));
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
            fprintf(mipsFp, "  addi %s, %s, %s\n", GetStrRegister(rx), GetStrRegister(ry), op2->value);
            break;
        case SUB_C:
            fprintf(mipsFp, "  addi %s, %s, -%s\n", GetStrRegister(rx), GetStrRegister(ry), op2->value);
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
            fprintf(mipsFp, "  addi %s, %s, %s\n", GetStrRegister(rx), GetStrRegister(rz), op1->value);
            break;
        case SUB_C:
            //TODO
            fprintf(mipsFp, "  addi %s, %s, -%s\n", GetStrRegister(rx), GetStrRegister(rz), op1->value);
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
}

void Mips4Func(InterCode p)
{

}

void Mips4Label(InterCode p)
{
    Operand op = p->singleOp.op;
    fprintf(mipsFp, "%s:\n", op->value);
}

void Mips4Goto(InterCode p)
{
    Operand op = p->singleOp.op;
    fprintf(mipsFp, "  j %s\n", op->value);
}

void Mips4Call(InterCode p)
{
    Operand left = p->assign.left;
    Operand right = p->assign.right;
    
}
