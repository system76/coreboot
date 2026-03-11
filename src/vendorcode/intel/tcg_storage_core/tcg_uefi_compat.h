/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VENDORCODE_INTEL_TCG_STORAGE_CORE_UEFI_COMPAT_H
#define VENDORCODE_INTEL_TCG_STORAGE_CORE_UEFI_COMPAT_H

#include <string.h>
#include <types.h>

typedef u8  UINT8;
typedef u16 UINT16;
typedef u32 UINT32;
typedef u64 UINT64;
typedef s8  INT8;
typedef s16 INT16;
typedef s32 INT32;
typedef s64 INT64;
typedef char CHAR8;
typedef size_t UINTN;
typedef ptrdiff_t INTN;
typedef void VOID;
typedef bool BOOLEAN;

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#define EFIAPI

#define CopyMem(d, s, n) memcpy((d), (s), (n))
#define ZeroMem(d, n) memset((d), 0, (n))

static inline UINT16 SwapBytes16(UINT16 v) { return __builtin_bswap16(v); }
static inline UINT32 SwapBytes32(UINT32 v) { return __builtin_bswap32(v); }
static inline UINT64 SwapBytes64(UINT64 v) { return __builtin_bswap64(v); }

static inline UINT64 LShiftU64(UINT64 v, UINTN n) { return v << n; }

#ifndef MAX_UINT8
#define MAX_UINT8  ((UINT8)0xff)
#endif
#ifndef MAX_UINT16
#define MAX_UINT16 ((UINT16)0xffff)
#endif
#ifndef MAX_UINT32
#define MAX_UINT32 ((UINT32)0xffffffffu)
#endif

/* Keep DEBUG() calls compile-time silent in SMM. */
#define DEBUG_INFO  0
#define DEBUG_WARN  0
#define DEBUG_ERROR 0
#define DEBUG_LINE_NUMBER __LINE__
#define DEBUG(x) do { } while (0)

#define ASSERT(x) do { } while (0)

#endif
