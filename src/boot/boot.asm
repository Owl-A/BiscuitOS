ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
  jmp short start_fake
  nop

times 33 db 0

start_fake:
  jmp 0:start

start:
  cli
  mov ax, 0x00
  mov ds, ax
  mov es, ax 
  mov ss, ax
  mov sp, 0x7c00
  sti
 
.load_protected:
  cli
  lgdt[gdt_descriptor]
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  jmp CODE_SEG:load32

gdt_start:
gdt_null:
  dd 0x0
  dd 0x0

gdt_code:
  dw 0xffff
  dw 0
  db 0
  db 0x9a
  db 11001111b
  db 0

gdt_data:
  dw 0xffff
  dw 0
  db 0
  db 0x92
  db 11001111b
  db 0

gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1
  dd gdt_start

[BITS 32]
load32:
  mov eax, 1        ; starting sector (lba starts from 0)
  mov ecx, 100      ; total sectors to load  
  mov edi, 0x0100000; where to load it 
  call ata_lba_read
  jmp CODE_SEG:0x100000

ata_lba_read:
  mov ebx, eax ; Backup the LBA
  ; Send highest 8 bits of LBA to hard disk controller
  shr eax, 24
  or eax, 0xE0
  mov dx, 0x1F6
  out dx, al
  
  ; Send the total sectors to read
  mov eax, ecx
  mov dx, 0x1F2
  out dx, al

  ; send more bits of the lba
  mov eax, ebx ; restore LBA
  mov dx, 0x1F3
  out dx, al

  ; Send more bits of the lba
  mov dx, 0x1F4
  mov eax, ebx ; restore LBA
  shr eax, 8
  out dx, al

  ; send upper 16 bits of the LBA
  mov dx, 0x1F5
  mov eax, ebx ; restore LBA
  shr eax, 16
  out dx, al

  mov dx, 0x1F7
  mov al, 0x20
  out dx, al

  ; read all the sectors
.next_sector
  push ecx

  ; Checking if we need to read
.try_again
  mov dx, 0x1F7
  in al, dx
  test al, 8
  jz .try_again

  ; need to read 256 words
  mov ecx, 256
  mov dx, 0x1F0
  rep insw
  pop ecx
  loop .next_sector

  ret

times 510 - ($ - $$) db 0
dw 0xAA55

