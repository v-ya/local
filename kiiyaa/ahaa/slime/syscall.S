#include "define.h"
	.file	"syscall.S"
	.text

// void* syscall_mmap(void *addr, uintptr_t len, uintptr_t prot, uintptr_t flags, intptr_t fd, uintptr_t offset);
// addr:    %rdi
// len:     %rsi
// prot:    %rdx
// flags:   %rcx => %r10
// fd:      %r8
// offset:  %r9
	.p2align 4
	.globl	syscall_mmap
	.type	syscall_mmap, @function
syscall_mmap:
	movq	%rcx, %r10
	movq	$9, %rax
	syscall
	ret
	.size	syscall_mmap, .-syscall_mmap

// int syscall_munmap(void *addr, uintptr_t len);
// addr:  %rdi
// len:   %rsi
	.p2align 4
	.globl	syscall_munmap
	.type	syscall_munmap, @function
syscall_munmap:
	movq	$11, %rax
	syscall
	ret
	.size	syscall_munmap, .-syscall_munmap

// int syscall_clone(uintptr_t flags, void *child_stack, uintptr_t *parent_pid, uintptr_t *child_pid, void *tls_value);
// flags:        %rdi
// child_stack:  %rsi
// parent_pid:   %rdx
// child_pid:    %rcx => %r10
// tls_value:    %r8
	.p2align 4
	.globl	syscall_clone
	.type	syscall_clone, @function
syscall_clone:
	movq	%rcx, %r10
	movq	$56, %rax
	syscall
	ret
	.size	syscall_clone, .-syscall_clone

// void syscall_exit(int status);
// status:  %rdi
	.p2align 4
	.globl	syscall_exit
	.type	syscall_exit, @function
syscall_exit:
	movq	$60, %rax
	syscall
	ret
	.size	syscall_exit, .-syscall_exit

// int syscall_futex(uintptr_t *uaddr, int futex_op, uintptr_t val, const struct timespec *timeout, uintptr_t *uaddr2, uintptr_t val3);
// uaddr:     %rdi
// futex_op:  %rsi
// val:       %rdx
// timeout:   %rcx => %r10
// uaddr2:    %r8
// val3:      %r9
	.p2align 4
	.globl	syscall_futex
	.type	syscall_futex, @function
syscall_futex:
	movq	%rcx, %r10
	movq	$202, %rax
	syscall
	ret
	.size	syscall_futex, .-syscall_futex

// int syscall_yield(void);
	.p2align 4
	.globl	syscall_yield
	.type	syscall_yield, @function
syscall_yield:
	movq	$24, %rax
	syscall
	ret
	.size	syscall_yield, .-syscall_yield

// int syscall_nanosleep(const struct timespec *req, struct timespec *rem);
// req:  %rdi
// rem:  %rsi
	.p2align 4
	.globl	syscall_nanosleep
	.type	syscall_nanosleep, @function
syscall_nanosleep:
	movq	$35, %rax
	syscall
	ret
	.size	syscall_nanosleep, .-syscall_nanosleep

// void inner_mclear(void *dst, uintptr_t size);
	.p2align 4
	.globl	inner_mclear
	.type	inner_mclear, @function
inner_mclear:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	rep stos %al, %es:(%rdi)
	ret
	.size	inner_mclear, .-inner_mclear

// void inner_mcopy(void *dst, const void *src, uintptr_t size);
	.p2align 4
	.globl	inner_mcopy
	.type	inner_mcopy, @function
inner_mcopy:
	movq	%rdx, %rcx
	rep movsb %ds:(%rsi), %es:(%rdi)
	ret
	.size	inner_mcopy, .-inner_mcopy

// struct slime_t* get_self(void);
	.p2align 4
	.globl	get_self
	.type	get_self, @function
get_self:
	movq	%fs:slime_fs_offset, %rax
	ret
	.size	get_self, .-get_self

// void slime_initial(void);
// stack-0: [slime_finally_pika]
// stack-1: [slime_finally]
// stack-2: [func]
// stack-3: [arg]
// stack-4: [slime]
	.p2align 4
	.local	slime_initial
	.type	slime_initial, @function
slime_initial:
	popq	%rsi
	popq	%rdi
	ret
	.size	slime_initial, .-slime_initial

// void slime_finally(void);
// stack-0: [slime_finally_pika]
	.p2align 4
	.local	slime_finally
	.type	slime_finally, @function
slime_finally:
	movq	%fs:slime_fs_offset, %rdi
	ret
	.size	slime_finally, .-slime_finally

// void* slime_initial_get(void);
	.p2align 4
	.globl	slime_initial_get
	.type	slime_initial_get, @function
slime_initial_get:
	leaq	slime_initial(%rip), %rax
	ret
	.size	slime_initial_get, .-slime_initial_get

// void* slime_finally_get(void);
	.p2align 4
	.globl	slime_finally_get
	.type	slime_finally_get, @function
slime_finally_get:
	leaq	slime_finally(%rip), %rax
	ret
	.size	slime_finally_get, .-slime_finally_get
