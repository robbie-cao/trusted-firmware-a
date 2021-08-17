/*
 * Copyright (c) 2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPMC_H
#define SPMC_H

#include <stdint.h>

#include "spm_common.h"

/*
 * 0x1 is used for StandAloneMM Secure Parition ID.
 * Same has been followed in Optee.
 * https://github.com/OP-TEE/optee_os/blob/49dbb9ef65643c4322cf3f848910fa880d1c02f6/core/arch/arm/kernel/stmm_sp.c#L65-L67
 */
#define  STMM_SP_ID	(0x1)

enum runtime_el {
	EL0,
	EL1,
	EL2,
	EL3
};

enum mailbox_state {
	/** There is no message in the mailbox. */
	MAILBOX_STATE_EMPTY,

	/** There is a message that has been populated in the mailbox. */
	MAILBOX_STATE_FULL,
};


struct mailbox {
	enum mailbox_state state;

	/* RX/TX Buffers */
	uintptr_t rx_buffer;
	uintptr_t tx_buffer;

	/*
	 * Size of RX/TX Buffer
	 */
	uint32_t rxtx_page_count;

	/* Lock access to mailbox */
	struct spinlock lock;
};


typedef struct spmc_sp_context {
	/*
	 * Secure partition context
	 */
	sp_context_t sp_ctx;

	/*
	 * ID of the Secure Partition
	 */
	uint16_t sp_id;

	/*
	 * Runtime EL
	 */
	uint16_t runtime_el;

	/*
	 * Mailbox tracking
	 */
	struct mailbox mailbox;

	/*
	 * Partition UUID
	 */
	uint32_t uuid[4];

	/*
	 * Partition Properties
	 */
	uint32_t properties;

	/*
	 * Number of Execution Contexts.
	 */
	uint16_t execution_ctx_count;

	/*
	 * Supported FFA Version
	 */
	uint32_t ffa_version;

	/*
	 * Execution State
	 */
	uint32_t exception_state;


} spmc_sp_context_t;

/**
 * Holds information returned for each partition by the FFA_PARTITION_INFO_GET
 * interface.
 */
struct ffa_partition_info {
	uint16_t vm_id;
	uint16_t execution_ctx_count;
	uint32_t properties;
};

#endif /* SPMC_H */
