[BITS 32]

section .asm

global restore_general_purpose_registers
global task_return
global user_registers

; void task_return(struct registers* regs);
task_return:
  mov ebp, esp
  ; Push the DATA SEGMENT (SS will be fine)
  ; Push the stack address
  ; Push the flags
  ; Push the CODE SEGMENT
  ; Push IP
  
  ; Let's access the structure passed to us
  mov ebx, [ebp+4]
  push dword [ebx+44] ; push SS
  push dword [ebx+40] ; push SP
  pushf
  pop eax
  or eax, 0x200 ; if set bit enables interrupts on iret
  push eax    ; finally push flags
  push dword [ebx+32] ; push CS
  push dword [ebx+28] ; push IP (remember this is virt addr)

  ; setup some segment registers
  mov ax, [ebx+44]
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  push dword [ebp+4]
  call restore_general_purpose_registers
  add esp, 4
  
  ; leave kernel for the userland
  iretd

; void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
  push ebp
  mov ebp, esp
  mov ebx, [ebp+8]
  mov edi, [ebx]
  mov esi, [ebx+4]
  mov ebp, [ebx+8]
  mov edx, [ebx+16]
  mov ecx, [ebx+20]
  mov eax, [ebx+24]
  mov ebx, [ebx+12]
  pop ebp
  ret

; void user_registers()
user_registers:
  mov ax, 0x23
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  ret
