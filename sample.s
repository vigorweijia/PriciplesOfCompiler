.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  subu $sp, $sp, 128
  li $t0, 0
  sw $t0, -4($fp)
  li $t1, 1
  sw $t1, -8($fp)
  li $t2, 0
  sw $t2, -12($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  move $t3, $v0
  sw $t3, -16($fp)
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t5, -16($fp)
  move $t4, $t5
  sw $t4, -20($fp)
label1:
  lw $t6, -12($fp)
  lw $t7, -20($fp)
  blt $t6, $t7, label2
  j label3
label2:
  lw $t1, -4($fp)
  lw $t2, -8($fp)
  add $t0, $t1, $t2
  sw $t0, -24($fp)
  lw $t4, -24($fp)
  move $t3, $t4
  sw $t3, -28($fp)
  lw $t5, -8($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  move $a0, $t5
  sw $t5, -8($fp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t6, -4($fp)
  lw $t7, -8($fp)
  move $t6, $t7
  sw $t6, -4($fp)
  lw $t0, -8($fp)
  lw $t1, -28($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  lw $t3, -12($fp)
  addi $t2, $t3, 1
  sw $t2, -32($fp)
  lw $t4, -12($fp)
  lw $t5, -32($fp)
  move $t4, $t5
  sw $t4, -12($fp)
  j label1
label3:
  move $v0, $0
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
