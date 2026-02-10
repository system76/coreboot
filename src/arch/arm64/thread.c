/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <thread.h>

/* The stack frame looks like the following after switch_to_thread. */
struct saved_regs {
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t x29; /* frame pointer */
	uint64_t x30; /* link register */
};

extern void thread_entry_wrapper(void);

void arch_prepare_thread(struct thread *t,
			 asmlinkage void (*thread_entry)(void *), void *arg)
{
	t->stack_current -= sizeof(struct saved_regs);
	/* ABI requirement, should always be true */
	assert(IS_ALIGNED(t->stack_current, ARCH_STACK_ALIGN_SIZE));
	struct saved_regs *regs = (void *)t->stack_current;
	regs->x19 = (uintptr_t)arg;
	regs->x20 = (uintptr_t)thread_entry;
	regs->x30 = (uintptr_t)thread_entry_wrapper;
}
