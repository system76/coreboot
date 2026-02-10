/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/asm.h>

/*
 * Routine for context switching between threads.
 *
 * asmlinkage void switch_to_thread(uintptr_t new_stack,
 *                                  uintptr_t *saved_stack)
 * x0 - new_stack
 * x1 - saved_stack
 */
ENTRY(switch_to_thread)
	/* Save the callee saved registers, frame pointer and link register */
	stp x29, x30, [sp, #-16]!
	stp x27, x28, [sp, #-16]!
	stp x25, x26, [sp, #-16]!
	stp x23, x24, [sp, #-16]!
	stp x21, x22, [sp, #-16]!
	stp x19, x20, [sp, #-16]!

	/* Save current stack pointer */
	mov x9, sp
	str x9, [x1]

	/* Restore new stack pointer */
	mov sp, x0

	/* Restore the callee saved registers and return to new thread */
	ldp x19, x20, [sp], #16
	ldp x21, x22, [sp], #16
	ldp x23, x24, [sp], #16
	ldp x25, x26, [sp], #16
	ldp x27, x28, [sp], #16
	ldp x29, x30, [sp], #16
	ret
ENDPROC(switch_to_thread)

/*
 * This is the entry point for a new thread.
 * x19 - thread_arg
 * x20 - thread_entry
 */
ENTRY(thread_entry_wrapper)
	mov x0, x19
	br x20
ENDPROC(thread_entry_wrapper)
