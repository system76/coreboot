# SPDX-License-Identifier: GPL-2.0-only

romstage-$(CONFIG_DRIVERS_GFX_NVIDIA) += romstage.c

ramstage-$(CONFIG_DRIVERS_GFX_NVIDIA) += nvidia.c
