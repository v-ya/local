#ifndef _slime_syscall_h_
#define _slime_syscall_h_

#include <stdint.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/wait.h>
#include <linux/futex.h>
#include <errno.h>

void* syscall_mmap(void *addr, uintptr_t len, uintptr_t prot, uintptr_t flags, intptr_t fd, uintptr_t offset);
int syscall_munmap(void *addr, uintptr_t len);
int syscall_clone(uintptr_t flags, void *child_stack, uintptr_t *parent_pid, uintptr_t *child_pid, void *tls_value);
void syscall_exit(int status);
int syscall_futex(uintptr_t *uaddr, int futex_op, uintptr_t val, const struct timespec *timeout, uintptr_t *uaddr2, uintptr_t val3);
int syscall_yield(void);
int syscall_nanosleep(const struct timespec *req, struct timespec *rem);

void inner_mclear(void *dst, uintptr_t size);
void inner_mcopy(void *dst, const void *src, uintptr_t size);

struct slime_t* get_self(void);

void* slime_initial_get(void);
void* slime_finally_get(void);

#endif
