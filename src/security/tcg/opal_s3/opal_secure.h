/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_TCG_OPAL_S3_OPAL_SECURE_H
#define SECURITY_TCG_OPAL_S3_OPAL_SECURE_H

#include <stddef.h>
#include <types.h>

static inline void opal_explicit_bzero(void *ptr, size_t len)
{
	volatile u8 *p = (volatile u8 *)ptr;

	if (!ptr || !len)
		return;

	while (len--)
		*p++ = 0;

	/* Prevent the compiler from optimizing the stores away. */
	__asm__ __volatile__("" : : "r"(ptr) : "memory");
}

#endif
