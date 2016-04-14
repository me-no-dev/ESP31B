/*
 postmortem.c - output of debug info on sketch crash
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp32/ets_sys.h"
#include "esp31b_peri.h"
#include "debug.h"
#include "esp31b_uart.h"
#include "freertos/xtensa_context.h"
#include "freertos/task.h"

typedef struct tskTaskControlBlock
{
  volatile portSTACK_TYPE *pxTopOfStack;    /*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */
  xListItem               xGenericListItem; /*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
  xListItem               xEventListItem;   /*< Used to reference a task from an event list. */
  unsigned portBASE_TYPE  uxPriority;       /*< The priority of the task.  0 is the lowest priority. */
  portSTACK_TYPE          *pxStack;         /*< Points to the start of the stack. */
  signed char             pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */
  unsigned portBASE_TYPE  uxCriticalNesting;/*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
  unsigned portBASE_TYPE  uxTCBNumber;      /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
  unsigned portBASE_TYPE  uxTaskNumber;     /*< Stores a number specifically for use by third party trace code. */
  unsigned portBASE_TYPE  uxBasePriority;   /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
} tskTCB;

extern tskTCB * volatile pxCurrentTCB;

static const char* s_panic_file = 0;
static int s_panic_line = 0;
static const char* s_panic_func = 0;
static bool s_abort_called = false;

void abort() __attribute__((noreturn));

void abort(){
    s_abort_called = true;
    do { *((int*)0) = 0; } while(true);
}

void __assert_func(const char *file, int line, const char *func, const char *what) {
    s_panic_file = file;
    s_panic_line = line;
    s_panic_func = func;
}

void __panic_func(const char* file, int line, const char* func) {
    s_panic_file = file;
    s_panic_line = line;
    s_panic_func = func;
    abort();
}

#define exccause_read() ((__extension__({uint32_t r;__asm__ __volatile__("esync; rsr %0,exccause":"=a" (r));r;})))
#define excvaddr_read() ((__extension__({uint32_t r;__asm__ __volatile__("esync; rsr %0,excvaddr":"=a" (r));r;})))
#define epc_read() ((__extension__({uint32_t r;__asm__ __volatile__("esync; rsr %0,epc1":"=a" (r));r;})))

static const char* exc2str(uint32_t exc){
  switch(exc){
    case EXCCAUSE_ILLEGAL: return "Illegal Instruction";
    case EXCCAUSE_SYSCALL: return "System Call (SYSCALL instruction)";
    case EXCCAUSE_INSTR_ERROR: return "Instruction Fetch Error";
    case EXCCAUSE_LOAD_STORE_ERROR: return "Load Store Error";
    case EXCCAUSE_LEVEL1_INTERRUPT: return "Level 1 Interrupt";
    case EXCCAUSE_ALLOCA: return "Stack Extension Assist (MOVSP instruction) for alloca";
    case EXCCAUSE_DIVIDE_BY_ZERO: return "Integer Divide by Zero";
    case EXCCAUSE_SPECULATION: return "Use of Failed Speculative Access (not implemented)";
    case EXCCAUSE_PRIVILEGED: return "Privileged Instruction";
    case EXCCAUSE_UNALIGNED: return "Unaligned Load or Store";
    case EXCCAUSE_INSTR_DATA_ERROR: return "PIF Data Error on Instruction Fetch";
    case EXCCAUSE_LOAD_STORE_DATA_ERROR: return "PIF Data Error on Load or Store";
    case EXCCAUSE_INSTR_ADDR_ERROR: return "PIF Address Error on Instruction Fetch";
    case EXCCAUSE_LOAD_STORE_ADDR_ERROR: return "PIF Address Error on Load or Store";
    case EXCCAUSE_ITLB_MISS: return "ITLB Miss (no ITLB entry matches, hw refill also missed)";
    case EXCCAUSE_ITLB_MULTIHIT: return "ITLB Multihit (multiple ITLB entries match)";
    case EXCCAUSE_INSTR_RING: return "Ring Privilege Violation on Instruction Fetch";
    case EXCCAUSE_INSTR_PROHIBITED: return "Cache Attribute does not allow Instruction Fetch";
    case EXCCAUSE_DTLB_MISS: return "DTLB Miss (no DTLB entry matches, hw refill also missed)";
    case EXCCAUSE_DTLB_MULTIHIT: return "DTLB Multihit (multiple DTLB entries match)";
    case EXCCAUSE_LOAD_STORE_RING: return "Ring Privilege Violation on Load or Store";
    case EXCCAUSE_LOAD_PROHIBITED: return "Cache Attribute does not allow Load";
    case EXCCAUSE_STORE_PROHIBITED: return "Cache Attribute does not allow Store";
    case EXCCAUSE_CP0_DISABLED: return "Access to Coprocessor 0 when disabled";
    case EXCCAUSE_CP1_DISABLED: return "Access to Coprocessor 1 when disabled";
    default: return "UNKNOWN EXCEPTION";
  }
}

void __wrap_system_fatal_exception_handler(){
  register uint32_t sp asm("a1");
  uint32_t cause = exccause_read();
  uint32_t addr = excvaddr_read();
  uint32_t epc = epc_read();
  uint32_t stackTop = (uint32_t)(pxCurrentTCB->pxTopOfStack);
  char stackName[17];
  memcpy(stackName, pxCurrentTCB->pcTaskName, 16);
  stackName[16] = 0;

  if(s_panic_line)
    ets_printf("Panic %s:%d %s\n", s_panic_file, s_panic_line, s_panic_func);
  else if(s_abort_called)
    ets_printf("Abort\n");

  os_printf("\nException (%u) %s\nname: %s, sp: 0x%08x, epc: 0x%08x, excvaddr: 0x%08x\n", cause, exc2str(cause), stackName, sp, epc, addr);
  os_printf(">>>stack>>>\n");
  for (uint32_t pos = sp + 0x30; pos < stackTop; pos += 0x10) {
    uint32_t* values = (uint32_t*)(pos);
    ets_printf("%08x:  %08x %08x %08x %08x\n", pos, values[0], values[1], values[2], values[3]);
  }
  ets_printf("<<<stack<<<\n");
  system_restart();
}
