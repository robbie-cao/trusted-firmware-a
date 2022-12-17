/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <drivers/arm/mhuv3.h>
#include <drivers/delay_timer.h>


static void discover_mhu_v3_features(mhu_dev_t *dev,
		mhu_v3_feat_t *mhu_v3_features, size_t feature_list_len,
		mhu_frame_t frame_t)
{
	uint32_t feat_spt0, feat_spt1, feat;
	uint32_t i, val;
	struct mhu3_pbx_ctrl_page *pbx_ctrl_page;
	struct mhu3_mbx_ctrl_page *mbx_ctrl_page;

	dev->mhu_v3_features = mhu_v3_features;
	if (frame_t == MHU_SENDER_FRAME) {
		if (false == dev->is_initialized[MHU_SENDER_FRAME])
			return;
		pbx_ctrl_page = (struct mhu3_pbx_ctrl_page *)
			(dev->pbx + MHU3_PBX_PBX_CTRL_PAGE_OFFSET);
		feat_spt0 = pbx_ctrl_page->PBX_FEAT_SPT0;
		feat_spt1 = pbx_ctrl_page->PBX_FEAT_SPT1;
	} else if (frame_t == MHU_RECEIVER_FRAME) {
		if (false == dev->is_initialized[MHU_RECEIVER_FRAME])
			return;
		mbx_ctrl_page = (struct mhu3_mbx_ctrl_page *)
			(dev->mbx + MHU3_MBX_MBX_CTRL_PAGE_OFFSET);
		feat_spt0 = mbx_ctrl_page->MBX_FEAT_SPT0;
		feat_spt1 = mbx_ctrl_page->MBX_FEAT_SPT1;
	} else {
		WARN("Undefined MHUv3 Frame Type :%u\n", frame_t);
		return;
	}


	INFO("MHUv3-%s: Supported Features\n", frame_t == MHU_SENDER_FRAME ?
			"Postbox" : "Mailbox");

	for (i = 0; i < feature_list_len; i++) {
		feat = (mhu_v3_features[i].feat_reg ? feat_spt1 :
				feat_spt0);
		val = (feat & GENMASK(mhu_v3_features[i].feat_msb,
					mhu_v3_features[i].feat_lsb)) >>
			mhu_v3_features[i].feat_lsb;
		if (val != mhu_v3_features[i].feat_inv_val[frame_t]) {
			mhu_v3_features[i].feat_support[frame_t] = val;
			mhu_v3_features[i].feat_valid[frame_t] = 1;
			INFO("\t%s:%u\n", mhu_v3_features[i].feat_name, val);
		}
	}
}

static void mhu_v3_init_sender_frame_doorbell_ch(mhu_dev_t *dev)
{
	struct mhu3_pdbcw_page *pdbcw_page;
	struct mhu3_pbx_ctrl_page *pbx_ctrl_page;
	uint32_t ch;

	pbx_ctrl_page = (struct mhu3_pbx_ctrl_page *)
		(dev->pbx + MHU3_PBX_PBX_CTRL_PAGE_OFFSET);
	pdbcw_page = (struct mhu3_pdbcw_page *)
		(dev->pbx + MHU3_PBX_PDBCW_PAGE_OFFSET);

	/* Read total number of supported channels */
	dev->mhu_v3_pbx_db.num_dbch = READ_REG(pbx_ctrl_page->PBX_DBCH_CFG0,
			DBCH_CFG0_NUM_DBCH_SHIFT, DBCH_CFG0_NUM_DBCH_MASK) + 1;
	dev->mhu_v3_pbx_db.num_free_dbch = dev->mhu_v3_pbx_db.num_dbch;

	for (ch = 0 ; ch < dev->mhu_v3_pbx_db.num_dbch; ch++) {
		/* Clear the Transfer Acknowledge */
		WRITE_REG(pdbcw_page[ch].PDBCW_INT_CLR,
				PDBCW_INT_CLR_TFR_ACK,
				PDBCW_INT_CLR_TFR_ACK_SHIFT,
				PDBCW_INT_CLR_TFR_ACK_MASK);
		/* Disable Transfer Acknowledge Interrupt */
		WRITE_REG(pdbcw_page[ch].PDBCW_INT_EN,
				~PDBCW_INT_EN_TFR_ACK,
				PDBCW_INT_EN_TFR_ACK_SHIFT,
				PDBCW_INT_EN_TFR_ACK_MASK);
		/* Prevent channel contribution to PBX combined interrupt */
		WRITE_REG(pdbcw_page[ch].PDBCW_CTRL,
				~PDBCW_CTRL_PBX_COMB_EN,
				PDBCW_CTRL_PBX_COMB_EN_SHIFT,
				PDBCW_CTRL_PBX_COMB_EN_MASK);
		dev->mhu_v3_pbx_db.dbch_state[ch] = FREE;
	}

}

static void mhu_v3_init_receiver_frame_doorbell_ch(mhu_dev_t *dev)
{
	struct mhu3_mdbcw_page *mdbcw_page;
	struct mhu3_mbx_ctrl_page *mbx_ctrl_page;
	uint32_t ch;

	mbx_ctrl_page = (struct mhu3_mbx_ctrl_page *)
		(dev->mbx + MHU3_MBX_MBX_CTRL_PAGE_OFFSET);
	mdbcw_page = (struct mhu3_mdbcw_page *)
		(dev->mbx + MHU3_MBX_MDBCW_PAGE_OFFSET);

	/* Read total number of supported channels */
	dev->mhu_v3_mbx_num_dbch = READ_REG(mbx_ctrl_page->MBX_DBCH_CFG0,
			DBCH_CFG0_NUM_DBCH_SHIFT, DBCH_CFG0_NUM_DBCH_MASK) + 1;

	for (ch = 0; ch < dev->mhu_v3_mbx_num_dbch; ch++) {
		/* Mask interrupts for all the flags to stop listening */
		mdbcw_page[ch].MDBCW_MSK_SET = (uint32_t)MHU3_ALL_FLAGS_MASK;
		/* Prevent channel contribution to MBX combined interrupt */
		WRITE_REG(mdbcw_page[ch].MDBCW_CTRL,
				~MDBCW_CTRL_MBX_COMB_EN,
				MDBCW_CTRL_MBX_COMB_EN_SHIFT,
				MDBCW_CTRL_MBX_COMB_EN_MASK);
	}
}

int mhu_v3_sender_frame_init(mhu_dev_t *dev, uintptr_t base,
		mhu_v3_feat_t *mhu_v3_features, size_t feature_list_len)
{
	struct mhu3_pbx_ctrl_page *pbx_ctrl_page;

	if ((void *)base == NULL)
		return -EFAULT;

	dev->pbx = base;
	pbx_ctrl_page = (struct mhu3_pbx_ctrl_page *)(dev->pbx +
			MHU3_PBX_PBX_CTRL_PAGE_OFFSET);

	if (READ_REG(pbx_ctrl_page->PBX_BLK_ID, PBX_BLK_ID_SHIFT,
				PBX_BLK_ID_MASK) != MHU_SENDER_FRAME) {
		ERROR("Not a Sender Frame, please check IO base address\n");
		return -EINVAL;
	}

	WRITE_REG(pbx_ctrl_page->PBX_CTRL, PBX_CTRL_OP_REQ,
			PBX_CTRL_OP_REQ_SHIFT, PBX_CTRL_OP_REQ_MASK);
	WRITE_REG(pbx_ctrl_page->PBX_CTRL, ~PBX_CTRL_CH_OP_MSK_IGNORE,
			PBX_CTRL_CH_OP_MSK_SHIFT, PBX_CTRL_CH_OP_MSK_MASK);

	dev->product_id[MHU_SENDER_FRAME] = READ_REG(pbx_ctrl_page->PBX_IIDR,
			IIDR_PRODUCT_ID_SHIFT, IIDR_PRODUCT_ID_MASK);
	dev->variant[MHU_SENDER_FRAME] = READ_REG(pbx_ctrl_page->PBX_IIDR,
			IIDR_VARIANT_SHIFT, IIDR_VARIANT_MASK);
	dev->version[MHU_SENDER_FRAME].major = READ_REG(pbx_ctrl_page->PBX_AIDR,
			AIDR_ARCH_MAJOR_REV_SHIFT, AIDR_ARCH_MAJOR_REV_MASK);
	dev->version[MHU_SENDER_FRAME].minor = READ_REG(pbx_ctrl_page->PBX_AIDR,
			AIDR_ARCH_MINOR_REV_SHIFT, AIDR_ARCH_MINOR_REV_MASK);

	dev->is_initialized[MHU_SENDER_FRAME] = true;
	INFO("MHUv3-Postbox: Product:%x Variant:%x Version:[%u:%u]\n",
			dev->product_id[MHU_SENDER_FRAME],
			dev->variant[MHU_SENDER_FRAME],
			dev->version[MHU_SENDER_FRAME].major,
			dev->version[MHU_SENDER_FRAME].minor);

	/* Obtain MHUv3 Capabilities */
	discover_mhu_v3_features(dev, mhu_v3_features, feature_list_len,
			MHU_SENDER_FRAME);

	/* Initialize Doorbell Channels */
	if (mhu_v3_features[DBE_SPT].feat_valid[MHU_SENDER_FRAME])
		mhu_v3_init_sender_frame_doorbell_ch(dev);

	dev->global_owner_id_counter = 0;

	return 0;
}

int mhu_v3_receiver_frame_init(mhu_dev_t *dev, uint64_t base,
		mhu_v3_feat_t *mhu_v3_features, size_t feature_list_len)
{
	struct mhu3_mbx_ctrl_page *mbx_ctrl_page;

	if ((void *)base == NULL)
		return -EFAULT;

	dev->mbx = base;
	mbx_ctrl_page = (struct mhu3_mbx_ctrl_page *)(dev->mbx +
			MHU3_MBX_MBX_CTRL_PAGE_OFFSET);

	if (READ_REG(mbx_ctrl_page->MBX_BLK_ID, MBX_BLK_ID_SHIFT,
				MBX_BLK_ID_MASK) != MHU_RECEIVER_FRAME) {
		ERROR("Not a Receiver Frame, please check IO base address\n");
		return -EINVAL;
	}

	WRITE_REG(mbx_ctrl_page->MBX_CTRL, MBX_CTRL_OP_REQ,
			MBX_CTRL_OP_REQ_SHIFT, MBX_CTRL_OP_REQ_MASK);
	WRITE_REG(mbx_ctrl_page->MBX_CTRL, ~MBX_CTRL_CH_OP_MSK_IGNORE,
			MBX_CTRL_CH_OP_MSK_SHIFT, MBX_CTRL_CH_OP_MSK_MASK);

	dev->product_id[MHU_RECEIVER_FRAME] = READ_REG(mbx_ctrl_page->MBX_IIDR,
			IIDR_PRODUCT_ID_SHIFT, IIDR_PRODUCT_ID_MASK);
	dev->variant[MHU_RECEIVER_FRAME] = READ_REG(mbx_ctrl_page->MBX_IIDR,
			IIDR_VARIANT_SHIFT, IIDR_VARIANT_MASK);
	dev->version[MHU_RECEIVER_FRAME].major = READ_REG(mbx_ctrl_page->MBX_AIDR,
			AIDR_ARCH_MAJOR_REV_SHIFT, AIDR_ARCH_MAJOR_REV_MASK);
	dev->version[MHU_RECEIVER_FRAME].minor = READ_REG(mbx_ctrl_page->MBX_AIDR,
			AIDR_ARCH_MINOR_REV_SHIFT, AIDR_ARCH_MINOR_REV_MASK);

	dev->is_initialized[MHU_RECEIVER_FRAME] = true;
	INFO("MHUv3-Mailbox: Product:%x Variant:%x Version:[%u:%u]\n",
			dev->product_id[MHU_RECEIVER_FRAME],
			dev->variant[MHU_RECEIVER_FRAME],
			dev->version[MHU_RECEIVER_FRAME].major,
			dev->version[MHU_RECEIVER_FRAME].minor);

	/* Obtain MHUv3 Capabilities */
	discover_mhu_v3_features(dev, mhu_v3_features, feature_list_len,
			MHU_RECEIVER_FRAME);

	/* Initialize Doorbell Channels */
	if (mhu_v3_features[DBE_SPT].feat_valid[MHU_RECEIVER_FRAME])
		mhu_v3_init_receiver_frame_doorbell_ch(dev);

	return 0;
}

/*
 * This function will require num of channels as input and in response will
 * derive a unique Owner ID and allocate channels to that Owner ID and return
 * the same to caller which has to be provided by the caller while requesting
 * for any MHU operation.
 */
int mhu_v3_sender_frame_reserve_doorbell_channels(mhu_dev_t *dev,
		uint64_t *owner_id, uint32_t nums, uint32_t *channel_ids)
{
	mhu_v3_db_t *pbx = &dev->mhu_v3_pbx_db;
	uint32_t ch, j;

	if (!nums) {
		ERROR("Invalid num of sender doorbell channels requested\n");
		return -EINVAL;
	}
	if (nums > (pbx->num_free_dbch)) {
		ERROR("Not enough free sender doorbell channels\n");
		return -ENOSPC;
	}

	/*
	 * TODO:
	 * Use a standard method to generate unique ID everytime this function is
	 * invoked. On multi core system at runtime bakery lock has to be used to
	 * manage channel ownership in this function during while request to use
	 * MHU arises from EL2 or EL1 software.
	 */
	dev->global_owner_id_counter++;
	*owner_id = dev->global_owner_id_counter;

	for (ch = 0, j = 0; (j < nums) && (ch < pbx->num_dbch); ch++) {

		if (pbx->dbch_state[ch] == FREE) {
			pbx->dbch_state[ch] = IN_USE;
			pbx->num_free_dbch -= 1;
			pbx->owner[ch] = *owner_id;
			channel_ids[j] = ch;
			j++;
		}
	}

	return 0;
}

void mhu_v3_sender_frame_release_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id)
{
	mhu_v3_db_t *pbx = &dev->mhu_v3_pbx_db;
	uint32_t ch;

	for (ch = 0; ch < pbx->num_dbch; ch++) {

		if ((pbx->owner[ch] == owner_id) &&
				(pbx->dbch_state[ch] == IN_USE)) {
			pbx->dbch_state[ch] = FREE;
			pbx->num_free_dbch += 1;
		}
	}
}

int mhu_v3_sender_frame_write_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id, uint32_t nums_ch, mhu_v3_db_data_t *data)
{
	uint32_t i, retry = 30;
	struct mhu3_pdbcw_page *pdbcw_page;
	mhu_v3_db_t *pbx = &dev->mhu_v3_pbx_db;

	pdbcw_page = (struct mhu3_pdbcw_page *)
		(dev->pbx + MHU3_PBX_PDBCW_PAGE_OFFSET);

	for (i = 0; i < nums_ch; i++) {
		if (pbx->owner[data[i].ch] != owner_id) {
			ERROR("Operation not permitted: Unknown Owner ID\n");
			return -EPERM;
		}
		/* Wait for the status to reset */
		while ((pdbcw_page[data[i].ch].PDBCW_ST & (data[i].flags)) && retry) {
			mdelay(20);
			retry--;
		}
		if (!retry) {
			ERROR("MHUv3:%p Sender timeout waiting for DBCH:%u\n",
					(void *)dev->pbx, data[i].ch);
			return -EIO;
		}

		pdbcw_page[data[i].ch].PDBCW_SET = data[i].flags;
	}

	return 0;
}

int mhu_v3_sender_frame_read_ack_doorbell_channels(mhu_dev_t *dev,
		uint32_t owner_id, uint32_t nums_ch, mhu_v3_db_data_t *data)
{
	uint32_t i;
	bool status, curr_status = false;
	struct mhu3_pdbcw_page *pdbcw_page;
	mhu_v3_db_t *pbx = &dev->mhu_v3_pbx_db;

	pdbcw_page = (struct mhu3_pdbcw_page *)
		(dev->pbx + MHU3_PBX_PDBCW_PAGE_OFFSET);

	for (i = 0; i < nums_ch; i++) {
		if (pbx->owner[data[i].ch] != owner_id) {
			ERROR("Operation not permitted: Unknown Owner ID\n");
			return -EPERM;
		}
	}
	/* Wait for all the channels */
	while (!curr_status) {
		status = true;
		for (i = 0; i < nums_ch && (status == true); i++) {
			/* Status is still set i.e. not acknowledged yet */
			if (pdbcw_page[data[i].ch].PDBCW_ST & (data[i].flags))
				status = status & false;
		}
		curr_status = status;
		mdelay(20);
	}

	return 0;
}

int mhu_v3_receiver_read_doorbell_ch(mhu_dev_t *dev, uint32_t ch, uint32_t *val)
{
	uint32_t retry = 30;
	struct mhu3_mdbcw_page *mdbcw_page;
	int ret = 0;

	mdbcw_page = (struct mhu3_mdbcw_page *)
		(dev->mbx + MHU3_MBX_MDBCW_PAGE_OFFSET);
	do {
		*val = mdbcw_page[ch].MDBCW_ST;
		if (*val)
			break;
		mdelay(20);
		retry--;
	} while (!retry);

	if (*val == 0) {
		ERROR("Unable to find data on DB ch:%u\n", ch);
		ret = -EIO;
	} else {
		WRITE_REG(mdbcw_page[ch].MDBCW_CLR, *val, 0, MHU3_ALL_FLAGS_MASK);
	}

	return ret;
}

int find_device(uintptr_t base_addr, mhu_v3_device_t *devs, size_t len,
		mhu_frame_t frame_type)
{
	uint32_t idx;
	int dev_index = -1;

	for (idx = 0; idx < len; idx++) {
		if ((devs[idx].base == base_addr) &&
			(devs[idx].frame_type == frame_type))
			dev_index = idx;
	}

	return dev_index;
}
