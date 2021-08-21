#ifndef BISCUITOS_H
#define BISCUITOS_H
#include <stddef.h>
#include <stdbool.h>

struct command_argument{
  char argument[512];
  struct command_argument* next;
};
struct process_arguments{
  int argc;
  char **argv;
};
void print(const char* message);
int biscuitos_getkey();
void biscuitos_putchar(char c);
void* biscuitos_malloc(size_t size);
void biscuitos_free(void* ptr);
int biscuitos_getkeyblock();
void biscuitos_terminal_readline(char* out, int max, bool output_while_typing);
void biscuitos_process_load_start(const char* filename);
struct command_argument* biscuitos_parse_command(const char* command, int max);
void biscuitos_process_get_arguments(struct process_arguments* arguments);
int biscuitos_system(struct command_argument* arguments);
int biscuitos_system_run(const char* command);
void biscuitos_exit();
#endif
