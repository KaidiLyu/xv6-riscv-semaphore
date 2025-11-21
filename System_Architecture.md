# xv6-riscv System Architecture Description

## I. Startup Process

1. QEMU loads the kernel image `kernel/kernel`.
2. `entry.S` sets up the kernel stack and jumps to `start.c`.
3. The `main()` function initializes subsystems:
   - Physical memory allocator (`kalloc.c`)
   - Virtual memory and paging (`vm.c`)
   - Process table and scheduler (`proc.c`)
   - File system (`fs.c`)
   - Interrupts and traps (`trap.c`, `plic.c`)
4. The first user process `init` is created and runs `/init`.
5. The user shell (`sh.c`) starts.

---

## II. Process Scheduling

- The scheduler is implemented in `scheduler()` (`proc.c`).
- Uses **Round Robin scheduling**.
- Processes yield the CPU via `yield()` or the system call `sleep()`.
- Each CPU core (`hart`) runs its own scheduler loop.

---

## III. Memory Management

- Implements **RISC-V Sv39 paging**.
- Separates kernel space from user space.
- Each process maintains an independent page table.
- Major components:
  - `kalloc.c` — Physical page allocator
  - `vm.c` — Page table setup, mapping, and copying
  - `trap.c` — Handles page faults and interrupts

---

## IV. File System

- Based on an **inode + log-structured file system**.
- Hierarchical call flow:
sysfile.c → file.c → fs.c → bio.c
- Features:
- Block caching (`bio.c`)
- Journaling and recovery (`log.c`)
- Supports regular files, directories, and device files (`mknod`)

---

## V. System Calls

- User mode invokes **`ecall`** (RISC-V trap) to enter kernel mode.
- The kernel handles traps in `trap.c` and dispatches to `syscall.c`.
- System call dispatch table:
static uint64 (*syscalls[])(void);
Each system call number (defined in syscall.h) maps to a kernel handler.
Newly Added Semaphore System Calls
sem_init(int value) — initialize a semaphore
sem_wait(int id) — wait (decrement)
sem_post(int id) — signal (increment)
sem_free(int id) — release the semaphore

---

## VI. Synchronization Using Semaphores
- Kernel module: kernel/semaphore.c
- User-level interface: system calls sys_sem_*
- Uses sleep() and wakeup() to implement blocking and signaling.
- Demonstration programs:
- semtest.c — basic parent-child synchronization
- prodcons.c — producer-consumer model using semaphores

---

## VII. Shell
- File: user/sh.c
- Functions:
    - Parses command-line input
    - Supports redirection (>, <) and pipelines (|)
- Execution flow:
    - fork() creates a new process
    - exec() loads and runs the program
    - The parent waits using wait()

---

## VIII. Summary
xv6 is a minimalist educational operating system for the RISC-V platform.
- It includes:
    - Multiprocessor scheduling
    - Virtual memory management
    - File system with journaling
    - System call mechanism
    - Process synchronization using semaphores
This modular architecture provides a complete view of how an operating system boots, manages memory, schedules processes, and synchronizes concurrent activities.