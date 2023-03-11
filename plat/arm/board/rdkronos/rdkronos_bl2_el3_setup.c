/*
 * Copyright (c) 2023, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <lib/fconf/fconf.h>
#include <lib/fconf/fconf_dyn_cfg_getter.h>
#include <plat/arm/common/plat_arm.h>
#include <plat/common/platform.h>
#include <platform_def.h>

void bl2_el3_plat_arch_setup(void)
{
	const struct dyn_cfg_dtb_info_t *fw_config_info;
	uint32_t fw_config_max_size;
	int err = -1;

	arm_bl2_el3_plat_arch_setup();

	/* Set global DTB info for fixed fw_config information */
	fw_config_max_size = ARM_FW_CONFIG_LIMIT - ARM_FW_CONFIG_BASE;
	set_config_info(ARM_FW_CONFIG_BASE, ~0UL, fw_config_max_size, FW_CONFIG_ID);

	/* Fill the device tree information struct with the info from the config dtb */
	err = fconf_load_config(FW_CONFIG_ID);
	if (err < 0) {
		ERROR("Loading of FW_CONFIG failed %d\n", err);
		plat_error_handler(err);
	}

	/*
	 * FW_CONFIG loaded successfully. If FW_CONFIG device tree parsing
	 * is successful then load TB_FW_CONFIG device tree.
	 */
	fw_config_info = FCONF_GET_PROPERTY(dyn_cfg, dtb, FW_CONFIG_ID);
	if (fw_config_info != NULL) {
		err = fconf_populate_dtb_registry(fw_config_info->config_addr);
		if (err < 0) {
			ERROR("Parsing of FW_CONFIG failed %d\n", err);
			plat_error_handler(err);
		}
		/* load TB_FW_CONFIG */
		err = fconf_load_config(TB_FW_CONFIG_ID);
		if (err < 0) {
			ERROR("Loading of TB_FW_CONFIG failed %d\n", err);
			plat_error_handler(err);
		}
	} else {
		ERROR("Invalid FW_CONFIG address\n");
		plat_error_handler(err);
	}
}
