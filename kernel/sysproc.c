#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

// -------------------------------------------------------------
// Basic process-related system calls
// -------------------------------------------------------------

// Exit current process
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

// Get current process ID
uint64
sys_getpid(void)
{
  return myproc()->pid;
}

// Create a new process (fork)
uint64
sys_fork(void)
{
  return kfork();
}

// Wait for a child process to exit
uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

// Adjust process memory size
uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazy memory allocation
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

// Sleep for a specific number of clock ticks
uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;

  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// Pause for N ticks (similar to sleep, for testing)
uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;

  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// Kill a process by PID
uint64
sys_kill(void)
{
  int pid;
  argint(0, &pid);
  return kkill(pid);
}

// Return the number of clock tick interrupts since system start
uint64
sys_uptime(void)
{
  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// -------------------------------------------------------------
// Semaphore-related system calls
// -------------------------------------------------------------

// Kernel-level semaphore function prototypes
int semaphore_alloc(int val);
int semaphore_wait(int id);
int semaphore_post(int id);
int semaphore_free(int id);

// sem_init(value): create a new semaphore
uint64
sys_sem_init(void)
{
  int val;
  argint(0, &val);
  return semaphore_alloc(val);
}

// sem_wait(id): decrease semaphore (wait)
uint64
sys_sem_wait(void)
{
  int id;
  argint(0, &id);
  return semaphore_wait(id);
}

// sem_post(id): increase semaphore (signal)
uint64
sys_sem_post(void)
{
  int id;
  argint(0, &id);
  return semaphore_post(id);
}

// sem_free(id): free the semaphore
uint64
sys_sem_free(void)
{
  int id;
  argint(0, &id);
  return semaphore_free(id);
}
