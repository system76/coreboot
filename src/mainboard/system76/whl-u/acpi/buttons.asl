/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (PWRB)
{
    Name (_HID, EisaId ("PNP0C0C"))
    Name (_PRW, Package () { 0x29 /* GPP_D9 */, 3 })
}

Device (SLPB)
{
    Name (_HID, EisaId ("PNP0C0E"))
    Name (_PRW, Package () { 0x29 /* GPP_D9 */, 3 })
}
