#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "biscuitos.h"

int main(int argc, char** argv){
  print("BiscuitOS v0.1.0\n");
  while(1){
    print("> ");
    char buf[1024];
    biscuitos_terminal_readline(buf, sizeof(buf), true);
    print("\n");
    biscuitos_system_run(buf);
    print("\n");
  }
  return 0;
}
