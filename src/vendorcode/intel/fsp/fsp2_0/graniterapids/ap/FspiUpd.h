/** @file

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

  This file is automatically generated. Please do NOT modify !!!

**/

#ifndef __FSPI_UPD_H__
#define __FSPI_UPD_H__

#include <FspUpd.h>

#define FSP_RAS_ACPI_HOB_GUID           { 0x826785ee, 0xa8e0, 0x4d8f, { 0x82, 0x6f, 0x54, 0x29, 0x2c, 0xe7, 0x6f, 0xe6 } };

#pragma pack(1)

typedef struct {

	/**  WHEA Support
  Enable/Disable WHEA support.
  0:Disable, 1:Enable
**/
  UINT8                       WheaSupportEn;

/**  Enable/Disable WHEA Error Injection Support.
  0:Disabled, 1:Enabled
**/
  UINT8                       WheaErrorInjSupportEn;

/**  Whea EINJ ACPI 5.0 support for set error type with address and vendor extensions.
  0:Disabled, 1:Enabled
**/
  UINT8                       WheaACPI50ErrInjEn;

/**  Whea PCIE Error Injection Support
  Enable/Disable Whea PCIE Error Injection Support in FSP.
  0:Disable, 1:Enable
**/
  UINT8                       WheaPcieErrInjEn;

/**  Whea PCIe Error Injection Action Table
  Use Action Table for Whea PCIe Error Injection in FSP.
  0:Disable, 1:Enable
**/
  UINT8                       PcieErrInjActionTable;

/**  IIO eDPC Support
  Enable/Disable IIO eDPC Support.
  0:Disabled, 1:On Fatal Error, 2:On Fatal and Non-Fatal Errors
**/
  UINT8                       EdpcEn;

/** N/A
**/
  UINT8                       ReservedSiliconInitUpd[16];

} FSPI_CONFIG;


typedef struct {

/** N/A
**/
  FSP_UPD_HEADER              FspUpdHeader;
  
/** N/A
**/
  FSPI_CONFIG                 FspiConfig;
  
/** N/A
**/
  UINT16                      UpdTerminator;

} FSPI_UPD;

#pragma pack()

#endif
