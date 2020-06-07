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

func:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  subu $sp, $sp, 128
  li $s3, 2
  move $v0, $s3
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  subu $sp, $sp, 128
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal func
  move $t0, $v0
  sw $t0, -4($fp)
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t2, -4($fp)
  move $t1, $t2
  sw $t1, -8($fp)
  lw $t3, -8($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  move $a0, $t3
  sw $t3, -8($fp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $s3, 0
  move $v0, $s3
  addi $sp, $sp, 128
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
