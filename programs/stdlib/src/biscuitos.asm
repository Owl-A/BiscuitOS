[BITS 32]

section .asm

global print:function
global biscuitos_getkey:function
global biscuitos_malloc:function
global biscuitos_free:function
global biscuitos_putchar:function
global biscuitos_process_load_start:function
global biscuitos_process_get_arguments:function
global biscuitos_system:function
global biscuitos_exit:function

; void print(const char* message);
print:
  push ebp
  mov ebp, esp
  push dword[ebp+8]
  mov eax, 1; command print
  int 0x80 
  add esp, 4
  pop ebp
  ret

; int getkey();
biscuitos_getkey:
  push ebp
  mov ebp, esp
  mov eax, 2
  int 0x80
  pop ebp
  ret

; void biscuitos_putchar(char c);
biscuitos_putchar:
  push ebp
  mov ebp, esp
  mov eax, 3
  push dword[ebp+8]
  int 0x80
  add esp, 4
  pop ebp
  ret

; void* biscuitos_malloc(size_t size);
biscuitos_malloc:
  push ebp
  mov ebp, esp
  mov eax, 4 ; command malloc
  push dword[ebp+8]
  int 0x80
  add esp, 4
  pop ebp
  ret

; void biscuitos_free(void* ptr);
biscuitos_free:
  push ebp
  mov ebp, esp
  mov eax, 5
  push dword[ebp+8]
  int 0x80
  add esp, 4
  pop ebp
  ret

; void biscuitos_process_load_start(const char* filename);
biscuitos_process_load_start:
  push ebp
  mov ebp, esp
  mov eax, 6 ; command 6 process load start
  push dword[ebp+8]
  int 0x80
  add esp, 4
  pop ebp
  ret

; int biscuitos_system(struct command_argument* arguments);
biscuitos_system:
  push ebp
  mov ebp, esp
  mov eax, 7 ; command 7
  push dword[ebp+8]
  int 0x80
  add esp, 4
  pop ebp
  ret

; void biscuitos_process_get_arguments(struct process_arguments* arguments);
biscuitos_process_get_arguments:
  push ebp
  mov ebp, esp
  mov eax, 8 ; command 8
  push dword[ebp+8] ; variable arguments
  int 0x80
  add esp, 4
  pop ebp
  ret

; void biscuitos_exit();
biscuitos_exit:
  push ebp
  mov ebp, esp
  mov eax, 0 ; command 0 exit
  int 0x80
  pop ebp
  ret
