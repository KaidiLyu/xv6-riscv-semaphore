#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NITEMS 5   // Number of items to produce/consume

int main(void)
{
  int empty, full, mutex;
  int pid;

  // Initialize semaphores
  empty = sem_init(NITEMS);   // Start with N empty slots
  full = sem_init(0);         // No items initially
  mutex = sem_init(1);        // Mutex for critical section

  printf("Producer-Consumer demo starting...\n");

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // ----------- Child process: Consumer -----------
    for(int i = 0; i < NITEMS; i++){
      sem_wait(full);       // Wait until there's something to consume
      sem_wait(mutex);      // Enter critical section
      printf("Consumer: consumed item %d\n", i + 1);
      sem_post(mutex);      // Leave critical section
      sem_post(empty);      // Signal that there's one more empty slot
      sleep(20);            // Simulate time to process
    }
    printf("Consumer finished!\n");
  } else {
    // ----------- Parent process: Producer -----------
    for(int i = 0; i < NITEMS; i++){
      sem_wait(empty);      // Wait until there's space to produce
      sem_wait(mutex);      // Enter critical section
      printf("Producer: produced item %d\n", i + 1);
      sem_post(mutex);      // Leave critical section
      sem_post(full);       // Signal that a new item is ready
      sleep(10);            // Simulate production delay
    }
    printf("Producer finished!\n");
    wait(0);
  }

  // Cleanup
  sem_free(empty);
  sem_free(full);
  sem_free(mutex);

  printf("All items processed!\n");
  exit(0);
}
