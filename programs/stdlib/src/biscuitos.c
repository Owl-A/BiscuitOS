#include "biscuitos.h"
#include "string.h"

struct command_argument* biscuitos_parse_command(const char* command, int max){
  struct command_argument* root_command = 0;
  char scommand[1025];
  if(max >= (int) sizeof(scommand)){
    return 0;
  }
  strncpy(scommand, command, sizeof(scommand));
  char* token = strtok(scommand, " ");
  if(!token){
    goto out;
  }

  root_command = biscuitos_malloc(sizeof(struct command_argument));
  if(!root_command){
    goto out;
  }
  strncpy(root_command->argument, token, sizeof(root_command->argument));
  root_command->next = 0;

  struct command_argument* current = root_command;
  token = strtok(NULL, " ");
  while(token){
    struct command_argument* new_command = biscuitos_malloc(sizeof(struct command_argument));
    if(!new_command){
      break;
    }
    strncpy(new_command->argument, token, sizeof(new_command->argument));
    new_command->next = 0x00;
    current->next = new_command;
    current = new_command;
    token = strtok(NULL, " ");
  }
out:
  return root_command;
}

int biscuitos_getkeyblock(){
  int val = 0;
  do{ 
    val = biscuitos_getkey();
  }while(val == 0);
  return val;
}

void biscuitos_terminal_readline(char* out, int max, bool output_while_typing){
  int i = 0;
  for ( i = 0; i < max -1; i++){
    char key = biscuitos_getkeyblock();
    // Carriage return means we have read a line
    if(key == 13){
      break;
    }
    if(output_while_typing){
      biscuitos_putchar(key);
    }
    // Backspace
    if ( key == 0x08 && i > 0){
      out[i-1] = 0x00;
      // -2 because we will +1 when we go to the continue
      i -= 2;
      continue;
    }
    out[i] = key; 
  }
  out[i] = 0x00; 
}

int biscuitos_system_run(const char* command){
  char buf[1024];
  strncpy(buf, command, sizeof(buf));
  struct command_argument* root_command_argument = biscuitos_parse_command(buf, sizeof(buf)); 
  if(!root_command_argument){
    return -1;
  }
  
  return biscuitos_system(root_command_argument);
}