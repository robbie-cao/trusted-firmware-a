/*
 * Copyright (c) 2023, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MHUV3_H
#define MHUV3_H

#include <stdint.h>

#include <drivers/arm/mhu.h>
#include <plat/arm/common/arm_def.h>

/* Read-only register */
#define FWK_R const volatile
/* Write-only register */
#define FWK_W volatile
/* Read/write register */
#define FWK_RW volatile

#define MHU3_MAX_DOORBELL_CHANNELS UINT32_C(128)
#define SCP_MHU3_MBX_INT_EN (4U)
#define PBX_DBCH_FLAG_POS 0

#define MHU3_MASK_GENERATE(BITSTART, LENGTH) \
	(((1 << (LENGTH)) - 1) << ((BITSTART) + 1 - (LENGTH)))

#define MHU3_MASKED_RECOVER(MASKED, BITSTART, LENGTH) \
	((MASKED) >> ((BITSTART) + 1 - (LENGTH)))

#define CLEAR_REG(reg, mask)	\
	{ reg = reg & (~mask); }

#define WRITE_REG(reg, val, shift, mask)	\
	{				\
		CLEAR_REG(reg, mask)	\
		reg |= ((val << shift) & mask);	\
	}
#define	READ_REG(reg, shift, mask)	((reg & mask) >> shift)

/*
 * Postbox CTRL --> PBX.PBX_CTRL_page.PBX_CTRL
 */
#define PBX_CTRL_OP_REQ_MASK		BIT(0)
#define PBX_CTRL_CH_OP_MSK_MASK		BIT(1)
#define PBX_CTRL_OP_REQ_SHIFT		0
#define PBX_CTRL_CH_OP_MSK_SHIFT	1
#define PBX_CTRL_CH_OP_MSK_IGNORE	0x1U
#define PBX_CTRL_OP_REQ			0x1U

/*
 * Postbox Block Identifier --> PBX.PBX_CTRL_page.PBX_BLK_ID
 */
#define PBX_BLK_ID_MASK			GENMASK(3, 0)
#define PBX_BLK_ID_SHIFT		0

/*
 * Mailbox Block Identifier --> MBX.MBX_CTRL_page.MBX_BLK_ID
 */
#define MBX_BLK_ID_MASK			GENMASK(3, 0)
#define MBX_BLK_ID_SHIFT		0

/*
 * Mailbox CTRL --> MBX.MBX_CTRL_page.MBX_CTRL
 */
#define MBX_CTRL_OP_REQ_MASK		BIT(0)
#define MBX_CTRL_CH_OP_MSK_MASK		BIT(1)
#define MBX_CTRL_OP_REQ_SHIFT		0
#define MBX_CTRL_CH_OP_MSK_SHIFT	1
#define MBX_CTRL_CH_OP_MSK_IGNORE	0x1U
#define MBX_CTRL_OP_REQ			0x1U

/*
 * Postbox IIDR --> PBX.PBX_CTRL_page.PBX_IIDR
 * Mailbox IIDR --> MBX.MBX_CTRL_page.MBX_IIDR
 */
#define	IIDR_IMPLEMENTER_MASK		GENMASK(11, 0)
#define	IIDR_REVISION_MASK		GENMASK(15, 12)
#define	IIDR_VARIANT_MASK		GENMASK(19, 16)
#define	IIDR_PRODUCT_ID_MASK		GENMASK(31, 20)
#define	IIDR_IMPLEMENTER_SHIFT		0
#define	IIDR_REVISION_SHIFT		12
#define	IIDR_VARIANT_SHIFT		16
#define	IIDR_PRODUCT_ID_SHIFT		20

/*
 * Postbox AIDR --> PBX.PBX_CTRL_page.PBX_AIDR
 * Mailbox AIDR --> MBX.MBX_CTRL_page.MBX_AIDR
 */
#define AIDR_ARCH_MINOR_REV_MASK	GENMASK(3, 0)
#define AIDR_ARCH_MAJOR_REV_MASK	GENMASK(7, 4)
#define AIDR_ARCH_MINOR_REV_SHIFT	0
#define AIDR_ARCH_MAJOR_REV_SHIFT	4

/*
 * Postbox DBCH_CFG0 --> PBX.PBX_CTRL_page.PBX_DBCH_CFG0
 * Mailbox DBCH_CFG0 --> MBX.MBX_CTRL_page.MBX_DBCH_CFG0
 */
#define	DBCH_CFG0_NUM_DBCH_MASK		GENMASK(7, 0)
#define	DBCH_CFG0_NUM_DBCH_SHIFT	0
#define	MH3_MAX_NUM_DBCH		U(128)
#define	MH3_MAX_NUM_FCH			U(1024)

/*
 * Postbox PDBCW<n>_CTRL --> PBX.PDBCW_page.PDBCW_CTRL
 */
#define PDBCW_CTRL_PBX_COMB_EN_MASK	BIT(0)
#define PDBCW_CTRL_PBX_COMB_EN_SHIFT	0
#define PDBCW_CTRL_PBX_COMB_EN		0x1U

/*
 * Postbox PDBCW<n>_INT_EN --> PBX.PDBCW_page.PDBCW_INT_EN
 */
#define	PDBCW_INT_EN_TFR_ACK_MASK	BIT(0)
#define	PDBCW_INT_EN_TFR_ACK_SHIFT	0
#define	PDBCW_INT_EN_TFR_ACK		0x1U

/*
 * Postbox PDBCW<n>_INT_CLR --> PBX.PDBCW_page.PDBCW_INT_CLR
 */
#define	PDBCW_INT_CLR_TFR_ACK_MASK	BIT(0)
#define	PDBCW_INT_CLR_TFR_ACK_SHIFT	0
#define	PDBCW_INT_CLR_TFR_ACK		0x1U

/*
 * Mailbox MDBCW<n>_CTRL --> MBX.MDBCW_page.MDBCW_CTRL
 */
#define MDBCW_CTRL_MBX_COMB_EN_MASK	BIT(0)
#define MDBCW_CTRL_MBX_COMB_EN_SHIFT	0
#define MDBCW_CTRL_MBX_COMB_EN		0x1U

#define MHU3_NUM_DB_FLAGS		32U
#define MHU3_ALL_FLAGS_MASK		GENMASK(MHU3_NUM_DB_FLAGS - 1, 0)

/*
 * Offset
 */
#define	MHU3_PBX_PBX_CTRL_PAGE_OFFSET		UINT32_C(0x0)
#define	MHU3_PBX_PDBCW_PAGE_OFFSET		UINT32_C(0x00001000)
#define	MHU3_PBX_PFFCW_PAGE_OFFSET		UINT32_C(0x00002000)
#define	MHU3_PBX_PFCW_PAGE_OFFSET		UINT32_C(0x00003000)
#define	MHU3_PBX_PBX_IMPL_DEF_PAGE_OFFSET	UINT32_C(0x0000F000)

#define	MHU3_MBX_MBX_CTRL_PAGE_OFFSET		UINT32_C(0x0)
#define	MHU3_MBX_MDBCW_PAGE_OFFSET		UINT32_C(0x00001000)
#define	MHU3_MBX_MFFCW_PAGE_OFFSET		UINT32_C(0x00002000)
#define	MHU3_MBX_MFCW_PAGE_OFFSET		UINT32_C(0x00003000)
#define	MHU3_MBX_MBX_IMPL_DEF_PAGE_OFFSET	UINT32_C(0x0000F000)

/*!
 * \brief MHU3 Postbox(PBX) Register Definitions
 */
struct mhu3_pbx_ctrl_page {
	/*! Postbox control page */
	FWK_R uint32_t PBX_BLK_ID;
	uint8_t RESERVED1[0x10 - 0x04];
	FWK_R uint32_t PBX_FEAT_SPT0;
	FWK_R uint32_t PBX_FEAT_SPT1;
	uint8_t RESERVED2[0x20 - 0x18];
	FWK_R uint32_t PBX_DBCH_CFG0;
	uint8_t RESERVED3[0x30 - 0x24];
	FWK_R uint32_t PBX_FFCH_CFG0;
	uint8_t RESERVED4[0x40 - 0x34];
	FWK_R uint32_t PBX_FCH_CFG0;
	uint8_t RESERVED5[0x50 - 0x44];
	FWK_R uint32_t PBX_DCH_CFG0;
	uint8_t RESERVED6[0x100 - 0x54];
	FWK_RW uint32_t PBX_CTRL;
	uint8_t RESERVED7[0x150 - 0x104];
	FWK_RW uint32_t PBX_DMA_CTRL;
	FWK_R uint32_t PBX_DMA_ST;
	FWK_RW uint64_t PBX_DMA_CDL_BASE;
	FWK_RW uint32_t PBX_DMA_CDL_PROP;
	uint8_t RESERVED8[0x400 - 0x164];
	FWK_R uint32_t PBX_DBCH_INT_ST[(0x410 - 0x400) >> 2];
	FWK_R uint32_t PBX_FFCH_INT_ST[(0x430 - 0x410) >> 2];
	FWK_R uint32_t PBX_DCH_INT_ST;
	uint8_t RESERVED9[0xFC8 - 0x434];
	FWK_R uint32_t PBX_IIDR;
	FWK_R uint32_t PBX_AIDR;
	FWK_R uint32_t IMPL_DEF_ID[4 * 11];
};

struct mhu3_pdbcw_page {
	FWK_R uint32_t PDBCW_ST;
	uint8_t RESERVED1[0xC - 0x4];
	FWK_W uint32_t PDBCW_SET;
	FWK_R uint32_t PDBCW_INT_ST;
	FWK_W uint32_t PDBCW_INT_CLR;
	FWK_RW uint32_t PDBCW_INT_EN;
	FWK_RW uint32_t PDBCW_CTRL;
};
struct mhu3_mbx_ctrl_page {
	/*! Mailbox control page */
	FWK_R uint32_t MBX_BLK_ID;
	uint8_t RESERVED1[0x10 - 0x04];
	FWK_R uint32_t MBX_FEAT_SPT0;
	FWK_R uint32_t MBX_FEAT_SPT1;
	uint8_t RESERVED2[0x20 - 0x18];
	FWK_R uint32_t MBX_DBCH_CFG0;
	uint8_t RESERVED3[0x30 - 0x24];
	FWK_R uint32_t MBX_FFCH_CFG0;
	uint8_t RESERVED4[0x40 - 0x34];
	FWK_R uint32_t MBX_FCH_CFG0;
	uint8_t RESERVED5[0x50 - 0x44];
	FWK_R uint32_t MBX_DCH_CFG0;
	uint8_t RESERVED6[0x100 - 0x54];
	FWK_RW uint32_t MBX_CTRL;
	uint8_t RESERVED7[0x140 - 0x104];
	FWK_RW uint32_t MBX_FCH_CTRL;
	FWK_RW uint32_t MBX_FCG_INT_EN;
	uint8_t RESERVED8[0x150 - 0x148];
	FWK_RW uint32_t MBX_DMA_CTRL;
	FWK_R uint32_t MBX_DMA_ST;
	FWK_RW uint64_t MBX_DMA_CDL_BASE;
	FWK_RW uint32_t MBX_DMA_CDL_PROP;
	uint8_t RESERVED9[0x400 - 0x164];
	FWK_R uint32_t MBX_DBCH_INT_ST[(0x410 - 0x400) >> 2];
	FWK_R uint32_t MBX_FFCH_INT_ST[(0x420 - 0x410) >> 2];
	FWK_R uint32_t MBX_FCG_INT_ST;
	uint8_t RESERVED10[0x430 - 0x424];
	FWK_R uint32_t MBX_FCH_GRP_INT_ST[(0x4B0 - 0x430) >> 2];
	FWK_R uint32_t MBX_DCH_INT_ST;
	uint8_t RESERVED11[0xFC8 - 0x4B4];
	FWK_R uint32_t MBX_IIDR;
	FWK_R uint32_t MBX_AIDR;
	FWK_R uint32_t IMPL_DEF_ID[4 * 11];
};

struct mhu3_mdbcw_page {
	FWK_R uint32_t MDBCW_ST;
	FWK_R uint32_t MDBCW_ST_MSK;
	FWK_RW uint32_t MDBCW_CLR;
	uint8_t RESERVED1[0x10 - 0x0C];
	FWK_R uint32_t MDBCW_MSK_ST;
	FWK_RW uint32_t MDBCW_MSK_SET;
	FWK_RW uint32_t MDBCW_MSK_CLR;
	FWK_RW uint32_t MDBCW_CTRL;
};

typedef struct mhu_v3_db_data {
	uint32_t ch;
	uint8_t flags;
} mhu_v3_db_data_t;

typedef enum mhu_v3_cap_id {
	DBE_SPT,
	FE_SPT,
	FCE_SPT,
	TZE_SPT,
	RME_SPT,
	RASE_SPT,
	AUTO_OP_SPT
} mhu_v3_cap_id_t;


typedef enum feat_spt {
	FEAT_SPT0,
	FEAT_SPT1
} feat_spt_t;

typedef enum mhu_frame {
	MHU_SENDER_FRAME,
	MHU_RECEIVER_FRAME,
	MHU_MAX_FRAME_TYPES
} mhu_frame_t;

typedef enum mhu_v3_dbch_state {
	INVALID,
	FREE,
	IN_USE
} mhu_v3_dbch_state_t;
/*
 * MHU capability structure
 */
typedef struct mhu_v3_feat {
	char *feat_name;
	bool feat_valid[MHU_MAX_FRAME_TYPES];
	int32_t feat_support[MHU_MAX_FRAME_TYPES];
	int32_t feat_inv_val[MHU_MAX_FRAME_TYPES];
	uint32_t feat_msb;
	uint32_t feat_lsb;
	feat_spt_t feat_reg;
} mhu_v3_feat_t;

typedef struct mhu_version {
	uint8_t major;
	uint8_t minor;
} mhu_version_t;

typedef struct mhu_v3_db {
	uint32_t num_dbch;
	uint32_t num_free_dbch;
	mhu_v3_dbch_state_t dbch_state[MH3_MAX_NUM_DBCH];
	uint64_t owner[MH3_MAX_NUM_DBCH];
} mhu_v3_db_t;

typedef struct mhu_dev {
	uint64_t global_owner_id_counter;
	mhu_version_t version[MHU_MAX_FRAME_TYPES];
	uint32_t product_id[MHU_MAX_FRAME_TYPES];
	uint32_t variant[MHU_MAX_FRAME_TYPES];
	bool is_initialized[MHU_MAX_FRAME_TYPES];
	uintptr_t pbx;
	uintptr_t mbx;
	mhu_v3_feat_t *mhu_v3_features;
	mhu_v3_db_t mhu_v3_pbx_db;
	uint32_t mhu_v3_mbx_num_dbch;
} mhu_dev_t;

typedef struct mhu_v3_device {
	uintptr_t base;
	uintptr_t mem_base;
	mhu_dev_t *dev;
	mhu_v3_feat_t *feat_list;
	size_t feat_list_sz;
	mhu_frame_t frame_type;
} mhu_v3_device_t;

/* Fast Channel Configuration 0 Number of Fast Channels Start Bit */
#define MHU3_FCH_CFG0_NUM_FCH_BITSTART 9
/* Fast Channel Configuration 0 Number of Fast Channels Length */
#define MHU3_FCH_CFG0_NUM_FCH_LEN 10
/* Fast Channel Configuration 0 Number of Fast Channels Mask */
#define MHU3_FCH_CFG0_NUM_FCH_MASK \
	(MHU3_MASK_GENERATE( \
		MHU3_FCH_CFG0_NUM_FCH_BITSTART, MHU3_FCH_CFG0_NUM_FCH_LEN))

/* Fast Channel Configuration 0 Number of Fast Channel Groups Start Bit */
#define MHU3_FCH_CFG0_NUM_FCG_BITSTART 15
/* Fast Channel Configuration 0 Number of Fast Channel Groups Length */
#define MHU3_FCH_CFG0_NUM_FCG_LEN 5
/* Fast Channel Configuration 0 Number of Fast Channel Groups Mask */
#define MHU3_FCH_CFG0_NUM_FCG_MASK \
	(MHU3_MASK_GENERATE( \
		MHU3_FCH_CFG0_NUM_FCG_BITSTART, MHU3_FCH_CFG0_NUM_FCG_LEN))

/*
 * Fast Channel Configuration 0 Number of Fast Channels per Fast Channel Group
 * Start Bit
 */
#define MHU3_FCH_CFG0_NUM_FCH_PER_GRP_BITSTART 20

/*
 * Fast Channel Configuration 0 Number of Fast Channels per Fast Channel Group
 * Length
 */
#define MHU3_FCH_CFG0_NUM_FCH_PER_GRP_LEN 5

/*
 * Fast Channel Configuration 0 Number of Fast Channels per Fast Channel Group
 * Mask
 */
#define MHU3_FCH_CFG0_NUM_FCH_PER_GRP_MASK \
	(MHU3_MASK_GENERATE( \
		MHU3_FCH_CFG0_NUM_FCH_PER_GRP_BITSTART, \
		MHU3_FCH_CFG0_NUM_FCH_PER_GRP_LEN))

/* Fast Channel Configuration 0 Fast Channel Word Size Start Bit */
#define MHU3_FCH_CFG0_FCH_WS_BITSTART 28
/* Fast Channel Configuration 0 Fast Channel Word Size Length */
#define MHU3_FCH_CFG0_FCH_WS_LEN 8
/* Fast Channel Configuration 0 Fast Channel Word Size Mask */
#define MHU3_FCH_CFG0_FCH_WS_MASK \
	(MHU3_MASK_GENERATE( \
		MHU3_FCH_CFG0_FCH_WS_BITSTART, MHU3_FCH_CFG0_FCH_WS_LEN))


/* Doorbell Flags */
#define	MHU3_PBX_DCH_FLAG_SCMI		BIT_32(0)
#define	MHU3_PBX_DCH_FLAG_RSS_COMMS	BIT_32(0)

/* Base addresses of the NS AP<-->SCP MHUv3 */
#define	AP2SCP_NS_PBX		0x2A900000
#define	AP2SCP_NS_MBX		0x2A910000

/* Base addresses of the S AP<-->SCP MHUv3 */
#define	AP2SCP_S_PBX		0x2A920000
#define	AP2SCP_S_MBX		0x2A930000

/* Base addresses of the Root AP<-->SCP MHUv3 */
#define	AP2SCP_ROOT_PBX		0x2A940000
#define	AP2SCP_ROOT_MBX		0x2A950000

/* Base addresses of the NS AP<-->RSS MHUv3 */
#define	AP2RSS_NS_PBX		0x2AB00000
#define	AP2RSS_NS_MBX		0x2AB10000

/* Base addresses of the S AP<-->RSS MHUv3 */
#define	AP2RSS_S_PBX		0x2AB20000
#define	AP2RSS_S_MBX		0x2AB30000

/* Base addresses of the Realm AP<-->RSS MHUv3 */
#define	AP2RSS_REALM_PBX	0x2AB60000
#define	AP2RSS_REALM_MBX	0x2AB70000

/* Base addresses of the Root AP<-->RSS MHUv3 */
#define	AP2RSS_ROOT_PBX		0x2AB40000
#define	AP2RSS_ROOT_MBX		0x2AB50000


extern int find_device(uintptr_t base_addr, mhu_v3_device_t *devs, size_t len,
		mhu_frame_t frame_type);

extern int mhu_v3_sender_frame_init(mhu_dev_t *dev, uintptr_t base,
		mhu_v3_feat_t *mhu_v3_features, size_t feature_list_len);

extern int mhu_v3_receiver_frame_init(mhu_dev_t *dev, uint64_t base,
		mhu_v3_feat_t *mhu_v3_features, size_t feature_list_len);

extern int mhu_v3_sender_frame_reserve_doorbell_channels(mhu_dev_t *dev,
		uint64_t *owner_id, uint32_t nums, uint32_t *channel_ids);

extern void mhu_v3_sender_frame_release_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id);

extern int mhu_v3_sender_frame_write_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id, uint32_t nums_ch, mhu_v3_db_data_t *data);

extern int mhu_v3_sender_frame_read_ack_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id, uint32_t nums_ch, mhu_v3_db_data_t *data);

extern int mhu_v3_receiver_read_doorbell_ch(mhu_dev_t *dev, uint32_t ch,
		uint32_t *val);
#endif /* MHUV3 */
