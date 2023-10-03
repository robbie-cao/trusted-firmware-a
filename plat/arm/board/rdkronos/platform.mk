# Copyright (c) 2023, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

RDKRONOS_BASE		=	plat/arm/board/rdkronos

PLAT_INCLUDES		+=	-I${RDKRONOS_BASE}/include/

override ARM_PLAT_MT			:= 1
override ARM_RECOM_STATE_ID_ENC		:= 1
override CSS_LOAD_SCP_IMAGES		:= 0
override CTX_INCLUDE_AARCH32_REGS	:= 0
override ENABLE_AMU			:= 1
override NEED_BL1			:= 0
override NEED_BL2U			:= 0
override PSCI_EXTENDED_STATE_ID		:= 1
override ENABLE_SVE_FOR_SWD		:= 1
override ENABLE_SVE_FOR_NS		:= 1

BL2_AT_EL3				:= 1
CSS_USE_SCMI_SDS_DRIVER			:= 1
ENABLE_FEAT_FGT				:= 1
ENABLE_FEAT_ECV				:= 1
GICV3_SUPPORT_GIC600		        := 1
HW_ASSISTED_COHERENCY			:= 1
NEED_BL32				:= yes
USE_COHERENT_MEM			:= 0

GIC_ENABLE_V4_EXTN	:= 1
include drivers/arm/gic/v3/gicv3.mk
RDKRONOS_GIC_SOURCES	:=	${GICV3_SOURCES}			\
				plat/common/plat_gicv3.c		\
				plat/arm/common/arm_gicv3.c

RDKRONOS_CPU_SOURCES	:=	lib/cpus/aarch64/neoverse_poseidon.S

PLAT_BL_COMMON_SOURCES	+=	${RDKRONOS_BASE}/rdkronos_plat.c	\
				${RDKRONOS_BASE}/include/rdkronos_helpers.S

BL2_SOURCES		+=	${RDKRONOS_BASE}/rdkronos_err.c		\
				lib/utils/mem_region.c			\
				plat/arm/common/arm_nor_psci_mem_protect.c

BL31_SOURCES		+=	${RDKRONOS_CPU_SOURCES}			\
				${RDKRONOS_GIC_SOURCES}			\
				${RDKRONOS_BASE}/rdkronos_bl31_setup.c	\
				${RDKRONOS_BASE}/rdkronos_topology.c	\
				drivers/cfi/v2m/v2m_flash.c		\
				lib/utils/mem_region.c			\
				plat/arm/common/arm_nor_psci_mem_protect.c


ifeq (${BL2_AT_EL3},1)
BL2_SOURCES		+=	${RDKRONOS_CPU_SOURCES} \
				${RDKRONOS_BASE}/rdkronos_bl2_el3_setup.c
endif

# Add the FDT_SOURCES and options for Dynamic Config
FDT_SOURCES		+=	${RDKRONOS_BASE}/fdts/${PLAT}_fw_config.dts \
				${RDKRONOS_BASE}/fdts/${PLAT}_tb_fw_config.dts \
				fdts/${PLAT}.dts
FW_CONFIG		:=	${BUILD_PLAT}/fdts/${PLAT}_fw_config.dtb
TB_FW_CONFIG		:=	${BUILD_PLAT}/fdts/${PLAT}_tb_fw_config.dtb
HW_CONFIG		:=	${BUILD_PLAT}/fdts/${PLAT}.dtb

RDKRONOS_SPMC_MANIFEST_DTS	:=	${RDKRONOS_BASE}/fdts/${PLAT}_optee_spmc_manifest.dts
FDT_SOURCES		+=	${RDKRONOS_SPMC_MANIFEST_DTS}
RDKRONOS_TOS_FW_CONFIG	:=	${BUILD_PLAT}/fdts/${PLAT}_optee_spmc_manifest.dtb

# Add the TOS_FW_CONFIG to FIP and specify the same to certtool
$(eval $(call TOOL_ADD_PAYLOAD,${RDKRONOS_TOS_FW_CONFIG},--tos-fw-config,${RDKRONOS_TOS_FW_CONFIG}))


# Add the FW_CONFIG to FIP and specify the same to certtool
$(eval $(call TOOL_ADD_PAYLOAD,${FW_CONFIG},--fw-config,${FW_CONFIG}))
# Add the TB_FW_CONFIG to FIP and specify the same to certtool
$(eval $(call TOOL_ADD_PAYLOAD,${TB_FW_CONFIG},--tb-fw-config,${TB_FW_CONFIG}))
# Add the HW_CONFIG to FIP and specify the same to certtool
$(eval $(call TOOL_ADD_PAYLOAD,${HW_CONFIG},--hw-config,${HW_CONFIG}))

include plat/arm/common/arm_common.mk
include plat/arm/css/common/css_common.mk
include plat/arm/board/common/board_common.mk
