// kernel/semaphore.c
#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define MAXSEMS 64

struct semaphore {
  struct spinlock lock;
  int used;
  int value;
};

static struct semaphore sems[MAXSEMS];

void
semaphore_init_all(void)
{
  int i;
  for(i = 0; i < MAXSEMS; i++){
    sems[i].used = 0;
    sems[i].value = 0;
    initlock(&sems[i].lock, "sem");
  }
}

// Allocate a new semaphore, return id or -1
int
semaphore_alloc(int val)
{
  int i;
  for(i = 0; i < MAXSEMS; i++){
    acquire(&sems[i].lock);
    if(!sems[i].used){
      sems[i].used = 1;
      sems[i].value = val;
      release(&sems[i].lock);
      return i;
    }
    release(&sems[i].lock);
  }
  return -1;
}

int
semaphore_free(int id)
{
  if(id < 0 || id >= MAXSEMS) return -1;
  acquire(&sems[id].lock);
  if(!sems[id].used){
    release(&sems[id].lock);
    return -1;
  }
  sems[id].used = 0;
  sems[id].value = 0;
  release(&sems[id].lock);
  return 0;
}

int
semaphore_wait(int id)
{
  if(id < 0 || id >= MAXSEMS) return -1;
  struct semaphore *s = &sems[id];
  acquire(&s->lock);
  if(!s->used){
    release(&s->lock);
    return -1;
  }
  while(s->value <= 0){
    // sleep expects a spinlock pointer that it will release and re-acquire
    sleep(s, &s->lock);
    // when woken up, loop to re-check condition
    if(!s->used){ // if semaphore was freed while sleeping
      release(&s->lock);
      return -1;
    }
  }
  s->value--;
  release(&s->lock);
  return 0;
}

int
semaphore_post(int id)
{
  if(id < 0 || id >= MAXSEMS) return -1;
  struct semaphore *s = &sems[id];
  acquire(&s->lock);
  if(!s->used){
    release(&s->lock);
    return -1;
  }
  s->value++;
  // wake up any sleepers on this semaphore's chan
  wakeup(s);
  release(&s->lock);
  return 0;
}
