#include "task.h"
#include "kernel.h"
#include "status.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "process.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "idt/idt.h"
#include "string/string.h"
#include "loader/formats/elfloader.h"

// current task that is running
struct task* current_task = 0;

// Task linked list 
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process);

struct task* task_current(){
  return current_task;
}

struct task* task_get_next(){
  if(!current_task->next){
    return task_head;
  }
  return current_task->next;
}

static void task_list_remove(struct task* task){
  if(task->prev){
    task->prev->next = task->next;
  }

  if( task == task_head ){
    task_head = task->next;
  }

  if( task == task_tail ){
    task_tail = task->prev;
  }

  if( task == current_task ){
    current_task = task_get_next();
  }
}

int task_free( struct task* task){
  paging_free_4gb(task->page_directory);
  task_list_remove(task);

  kfree(task);
  
  return 0;
}

struct task* task_new(struct process* process){
  int res = 0;
  struct task* task = kzalloc(sizeof(struct task));
  if(!task){
    res = -ENOMEM;
    goto out;
  }

  res = task_init(task, process);
  if ( res != ALL_OK ){
    goto out;
  }

  if( !task_head ){
    task_head = task;
    task_tail = task;
    current_task = task;
    goto out;
  }

  task_tail->next = task;
  task->prev = task_tail;
  task_tail = task;

out:
  if(ISERR(res)){
    task_free(task);
    return ERROR(res);
  }

  return task;
}

void task_next(){
  struct task* next_task = task_get_next();
  if(!next_task){
    panic("No task to execute\n");
  }
  task_switch(next_task);
  task_return(&next_task->registers);
}

int task_switch(struct task* task){
  current_task = task;
  paging_switch(task->page_directory);
  return 0;
}

void task_save_state(struct task* task, struct interrupt_frame* frame){
  task->registers.ip = frame->ip;
  task->registers.cs = frame->cs;
  task->registers.flags = frame->flags;
  task->registers.esp = frame->esp;
  task->registers.ss = frame->ss;
  
  task->registers.eax = frame->eax;
  task->registers.ebp = frame->ebp;
  task->registers.ebx = frame->ebx;
  task->registers.ecx = frame->ecx;
  task->registers.edi = frame->edi;
  task->registers.edx = frame->edx;
  task->registers.esi = frame->esi;
}

// need to be in the kernel page table before calling this
int copy_string_from_task(struct task* task, void* virtual, void* phys, int max){
  if ( max >= PAGING_PAGE_SIZE ){
    return -EINVARG;
  }
  int res;
  char* tmp = kzalloc(max);
  if ( !tmp ){
    res = -ENOMEM;
    goto out;
  }
  uint32_t* task_directory = task->page_directory->directory_entry;
  uint32_t old_entry = paging_get(task_directory, tmp);

  paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  paging_switch(task->page_directory);

  strncpy(tmp, virtual, max);
  kernel_page();

  res = paging_set(task_directory, tmp, old_entry);
  if ( res < 0){
    res = -EIO;
    goto out_free;
  }

  strncpy(phys, tmp, max);

out_free:
  kfree(tmp);
out:
  return res;
}

void task_current_save_state(struct interrupt_frame* frame){
  if (!task_current()){
    panic("No current task to save\n");
  }
  struct task* task = task_current();
  task_save_state(task, frame);
}

int task_page(){
  user_registers();
  task_switch(current_task);
  return 0;
}

int task_page_task( struct task* task){
  user_registers();
  paging_switch(task->page_directory);
  return 0;
}

void task_run_first_ever_task(){
  if(!current_task){
    panic("task_run_first_ever_task(): no current task exists!\n");
  }
  task_switch(task_head);
  task_return(&task_head->registers);
}

int task_init(struct task* task, struct process* process){
  memset(task, 0, sizeof(struct task));
  // Map entire 4GB address space to itself
  task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  if(!task->page_directory){
    return -EIO;
  }

  task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
  if(process->filetype == PROCESS_FILETYPE_ELF){
    task->registers.ip = elf_header(process->elf_file)->e_entry;
  }
  task->registers.ss = USER_DATA_SEGMENT;
  task->registers.cs = USER_CODE_SEGMENT;
  task->registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;

  task->process = process;

  return 0;
}

// Must be in the kernel page table to invoke this 
void* task_get_stack_item(struct task* task, int index){
  void* result = 0;

  uint32_t* sp_ptr = (uint32_t*) task->registers.esp;
  // switch to the given task page
  task_page_task(task);
  result = (void*)sp_ptr[index];
  kernel_page();

  return result;
}

void* task_virtual_address_to_physical(struct task* task, void* virtual_address){
  return paging_get_physical_address(task->page_directory->directory_entry, virtual_address);
}