#include "kernel/types.h"
#include "user/user.h"

int main() {
  int sem = sem_init(0);
  if(sem < 0){
    printf("sem_init failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid == 0){
    printf("Child: sleeping 100 ticks, then post\n");
    sleep(100);
    sem_post(sem);
    printf("Child: post done\n");
    exit(0);
  } else {
    printf("Parent: waiting on semaphore...\n");
    sem_wait(sem);
    printf("Parent: semaphore acquired!\n");
    sem_free(sem);
    wait(0);
    exit(0);
  }
}
