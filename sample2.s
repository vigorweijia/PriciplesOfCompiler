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

fact:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  subu $sp, $sp, 128
  sw $a0, -4($fp)
  lw $t0, -4($fp)
  beq $t0, 1, label1
  j label2
label1:
  lw $t1, -4($fp)
  move $v0, $t1
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
  j label3
label2:
  lw $t3, -4($fp)
  addi $t2, $t3, -1
  sw $t2, -8($fp)
  lw $a0, -8($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  move $t4, $v0
  sw $t4, -12($fp)
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t6, -4($fp)
  lw $t7, -12($fp)
  mul $t5, $t6, $t7
  sw $t5, -16($fp)
  lw $t0, -16($fp)
  move $v0, $t0
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
label3:

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  subu $sp, $sp, 128
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  move $t1, $v0
  sw $t1, -4($fp)
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t3, -4($fp)
  move $t2, $t3
  sw $t2, -8($fp)
  lw $t4, -8($fp)
  bgt $t4, 1, label4
  j label5
label4:
  lw $a0, -8($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  move $t5, $v0
  sw $t5, -12($fp)
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t7, -12($fp)
  move $t6, $t7
  sw $t6, -16($fp)
  j label6
label5:
  lw $t0, -16($fp)
  li $t0, 1
  sw $t0, -16($fp)
label6:
  lw $t1, -16($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  move $a0, $t1
  sw $t1, -16($fp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $s3, 0
  move $v0, $s3
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
