[BITS 32]

global _start 
extern c_start
extern biscuitos_exit

section .asm 

_start:
  call c_start
  call biscuitos_exit
  ret
