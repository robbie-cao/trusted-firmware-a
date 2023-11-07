/*
 * Copyright (c) 2023, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/arm/sbsa.h>
#include <lib/fconf/fconf.h>
#include <lib/fconf/fconf_dyn_cfg_getter.h>
#include <plat/arm/common/plat_arm.h>
#include <plat/common/platform.h>

const mmap_region_t plat_arm_mmap[] = {
	ARM_MAP_SHARED_RAM,
	KRONOS_MAP_DEVICE,
	KRONOS_EXTERNAL_FLASH,
	SOC_PLATFORM_PERIPH_MAP_DEVICE,
	SOC_SYSTEM_PERIPH_MAP_DEVICE,
#if IMAGE_BL2
	KRONOS_MAP_NS_DRAM1,
#endif
	KRONOS_MAP_S_DRAM1,
	{0}
};

void plat_arm_secure_wdt_start(void)
{
	sbsa_wdog_start(SBSA_SECURE_WDOG_BASE, SBSA_SECURE_WDOG_TIMEOUT);
}

void plat_arm_secure_wdt_stop(void)
{
	sbsa_wdog_stop(SBSA_SECURE_WDOG_BASE);
}

/*
 * For Kronos we do not do anything in these interface functions.
 */
void plat_arm_security_setup(void)
{
}

void __init plat_arm_interconnect_init(void)
{
}

void plat_arm_interconnect_enter_coherency(void)
{
}

void plat_arm_interconnect_exit_coherency(void)
{
}

void soc_css_init_nic400(void)
{
}

void soc_css_init_pcie(void)
{
}