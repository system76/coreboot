/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CDT_H_
#define _SOC_QUALCOMM_CDT_H_

#include <stddef.h>
#include <sys/types.h>

#define CDT_REGION_NAME "RW_CDT"

ssize_t cdt_read(void *buffer, size_t buffer_size);

#endif /* _SOC_QUALCOMM_CDT_H_ */
