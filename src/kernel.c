#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char (char c, char color){
  return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color){
  video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color){
  if (c == '\n'){
    terminal_row += 1;
    terminal_col = 0;
    return;
  }
  terminal_putchar(terminal_col, terminal_row, c, color);
  terminal_col += 1;
  if (terminal_col >= VGA_WIDTH){
    terminal_col = 0;
    terminal_row += 1;
  }
}

void terminal_initialize(){
  video_mem = (uint16_t*) 0xB8000;
  for(int i = 0; i < VGA_WIDTH*VGA_HEIGHT; i++) 
    video_mem[i] = terminal_make_char(' ', 0);
}

size_t strlen(const char* str){
  size_t len = 0;
  while(str[len]) len++;
  return len;
}

void print(const char* str){
  size_t len = strlen(str);
  for(size_t i = 0; i < len; i++){
    terminal_writechar(str[i], 0x0f);
  }
}

void kernel_main(){
  terminal_initialize();
  print("C says :\nHello World!\n");

  kheap_init();

  // initialize the idt
  idt_init();
  enable_interrupts();

  void* ptr = kmalloc(50);
  void* ptr2 = kmalloc(5000);
  void* ptr3 = kmalloc(5000);
  kfree(ptr);
  void* ptr4 = kmalloc(50);
  if(ptr || ptr2 || ptr3 || ptr4){
  }
}
