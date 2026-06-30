/**
  ******************************************************************************
  * @file    low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot on STM32N6xx
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "bootutil/bootutil_log.h"
#include "target_cfg.h"
#include "bootutil_priv.h"

#include "low_level_rng.h"
#include "low_level_security.h"
#include "low_level_mce.h"
#include "low_level_otp.h"
#include "debug_authentication.h"

#define MPU_ATTRIBUTE_DEVICE      MPU_ATTRIBUTES_NUMBER0
#define MPU_ATTRIBUTE_CODE        MPU_ATTRIBUTES_NUMBER1
#define MPU_ATTRIBUTE_DATA        MPU_ATTRIBUTES_NUMBER2
#define MPU_ATTRIBUTE_DATANOCACHE MPU_ATTRIBUTES_NUMBER3

typedef struct
{
  RISAF_TypeDef *RISAFx;
  uint32_t Region;
  RISAF_BaseRegionConfig_t BaseRegionConfig;
#ifdef FLOW_CONTROL
  uint32_t flow_step_enable;
  uint32_t flow_ctrl_enable;
  uint32_t flow_step_check;
  uint32_t flow_ctrl_check;
#endif
} RISAF_BaseRegionCfg_t;

typedef struct
{
  MPU_Region_InitTypeDef MPU_Region;
#ifdef FLOW_CONTROL
  uint32_t flow_step_enable;
  uint32_t flow_ctrl_enable;
  uint32_t flow_step_check;
  uint32_t flow_ctrl_check;
#endif
} MPU_Region_Config_t;

/** @defgroup OEMUROT_SECURITY_Private_Defines  Private Defines
  * @{
  */

/**************************
  * Initial configuration *
  *************************/

/* SAU configuration
  ================== */
const struct sau_cfg_t region_sau_init_cfg[] = {
  /* Region 0: Allows peripheral access */
  {
    0,
    PERIPH_BASE_NS,
    PERIPH_BASE_NS + 0xFFFFFFF,
    SAU_REGION_NOT_NSC,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R0,
    FLOW_CTRL_SAU_I_EN_R0,
    FLOW_STEP_SAU_I_CH_R0,
    FLOW_CTRL_SAU_I_CH_R0,
#endif /* FLOW_CONTROL */
  },
  /* Region 1: Code S */
  {
    1,
    S_CODE_START,
    S_CODE_LIMIT,
    SAU_REGION_NSC,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R1,
    FLOW_CTRL_SAU_I_EN_R1,
    FLOW_STEP_SAU_I_CH_R1,
    FLOW_CTRL_SAU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Code NS */
  {
    2,
    NS_CODE_START,
    NS_CODE_LIMIT,
    SAU_REGION_NOT_NSC,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R2,
    FLOW_CTRL_SAU_I_EN_R2,
    FLOW_STEP_SAU_I_CH_R2,
    FLOW_CTRL_SAU_I_CH_R2,
#endif /* FLOW_CONTROL */
  },
  /* Region 3: DATA NS */
  {
    3,
    NS_DATA_START,
    NS_DATA_LIMIT,
    SAU_REGION_NOT_NSC,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R3,
    FLOW_CTRL_SAU_I_EN_R3,
    FLOW_STEP_SAU_I_CH_R3,
    FLOW_CTRL_SAU_I_CH_R3,
#endif /* FLOW_CONTROL */
  },
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  /* Region 4: DATA NS */
  {
    4,
    NS_DATA2_START,
    NS_DATA2_LIMIT,
    SAU_REGION_NOT_NSC,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R4,
    FLOW_CTRL_SAU_I_EN_R4,
    FLOW_STEP_SAU_I_CH_R4,
    FLOW_CTRL_SAU_I_CH_R4,
#endif /* FLOW_CONTROL */
  },
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
};

/* RISAF configuration
  ================== */
RISAF_BaseRegionCfg_t risaf_cfg[] =
{
  /* NS Code */
  {
#if (OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
    RISAF12,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
    RISAF2,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    RISAF11,
#endif
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
#if (OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
       NS_CODE_START & RISAF12_LIMIT_ADDRESS_SPACE_SIZE,
       NS_CODE_LIMIT & RISAF12_LIMIT_ADDRESS_SPACE_SIZE,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
       NS_CODE_START & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
       NS_CODE_LIMIT & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
       NS_CODE_START & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
       NS_CODE_LIMIT & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
#endif
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_RIF_EN_R1,
    FLOW_CTRL_RIF_EN_R1,
    FLOW_STEP_RIF_CH_R1,
    FLOW_CTRL_RIF_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* NS Data */
  {
#if (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    RISAF11,
#else
    RISAF2,
#endif
    RISAF_REGION_2,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
#if (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
       NS_DATA_START & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
       NS_DATA_LIMIT & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
#else
       NS_DATA_START & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
       NS_DATA_LIMIT & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
#endif
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_RIF_EN_R2,
    FLOW_CTRL_RIF_EN_R2,
    FLOW_STEP_RIF_CH_R2,
    FLOW_CTRL_RIF_CH_R2,
#endif /* FLOW_CONTROL */
  },
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  /* NS Data */
  {
#if (OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
    RISAF12,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
    RISAF2,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    RISAF11,
#endif
    RISAF_REGION_3,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
#if (OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
       NS_DATA2_START & RISAF12_LIMIT_ADDRESS_SPACE_SIZE,
       NS_DATA2_LIMIT & RISAF12_LIMIT_ADDRESS_SPACE_SIZE,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
       NS_DATA2_START & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
       NS_DATA2_LIMIT & RISAF2_LIMIT_ADDRESS_SPACE_SIZE,
#elif (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
       NS_DATA2_START & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
       NS_DATA2_LIMIT & RISAF11_LIMIT_ADDRESS_SPACE_SIZE,
#endif
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_RIF_EN_R3,
    FLOW_CTRL_RIF_EN_R3,
    FLOW_STEP_RIF_CH_R3,
    FLOW_CTRL_RIF_CH_R3,
#endif /* FLOW_CONTROL */
  },
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
};

/* MPU configuration
  ================== */
const MPU_Attributes_InitTypeDef mpu_attributes[] =
{
  /* Peripherals */
  {
    MPU_ATTRIBUTES_NUMBER0,
    MPU_DEVICE_GRE
  },
  /* CODE */
  {
    MPU_ATTRIBUTES_NUMBER1,
    INNER_OUTER(MPU_WRITE_THROUGH | MPU_NON_TRANSIENT | MPU_NO_ALLOCATE)
  },
  /* DATA */
  {
    MPU_ATTRIBUTES_NUMBER2,
    INNER_OUTER(MPU_WRITE_THROUGH | MPU_NON_TRANSIENT | MPU_RW_ALLOCATE)
  },
  /* DATA NO CACHE */
  {
    MPU_ATTRIBUTES_NUMBER3,
    INNER_OUTER(MPU_NO_ALLOCATE)
  },
};

const MPU_Region_Config_t mpu_region_cfg[] = {
  /* Region 0: Allows RW access to peripherals */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER0,
      .AttributesIndex  = MPU_ATTRIBUTE_DEVICE,
      .BaseAddress      = PERIPH_BASE_S,
      .LimitAddress     = PERIPH_BASE_S + 0xFFFFFFF,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R0,
    FLOW_CTRL_MPU_I_EN_R0,
    FLOW_STEP_MPU_I_CH_R0,
    FLOW_CTRL_MPU_I_CH_R0,
#endif /* FLOW_CONTROL */
  },
  /* Region 1: Allows reading access on revision ID area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER1,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = REVID_BASE_S,
      .LimitAddress     = REVID_BASE_S + 0x1F,
      .AccessPermission = MPU_REGION_PRIV_RO,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R1,
    FLOW_CTRL_MPU_I_EN_R1,
    FLOW_STEP_MPU_I_CH_R1,
    FLOW_CTRL_MPU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Allows RW access to all slots + HASH REF + SCRATCH */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER2,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = FLASH_AREA_BEGIN_ADDRESS,
      .LimitAddress     = FLASH_AREA_END_ADDRESS - 1,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R2,
    FLOW_CTRL_MPU_I_EN_R2,
    FLOW_STEP_MPU_I_CH_R2,
    FLOW_CTRL_MPU_I_CH_R2,
#endif /* FLOW_CONTROL */
  },
  /* Region 3: Allows execution of boot */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER3,
      .AttributesIndex  = MPU_ATTRIBUTE_CODE,
      .BaseAddress      = BL2_CODE_START - BOOTROM_HEADER_SIZE,
      .LimitAddress     = BL2_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RO,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE,
      .DisablePrivExec  = MPU_INSTRUCTION_ACCESS_ENABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R3,
    FLOW_CTRL_MPU_I_EN_R3,
    FLOW_STEP_MPU_I_CH_R3,
    FLOW_CTRL_MPU_I_CH_R3,
#endif /* FLOW_CONTROL */
  },
  /* Region 4: Allows execution of the jumper */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER4,
      .AttributesIndex  = MPU_ATTRIBUTE_CODE,
      .BaseAddress      = BL2_JUMP_CODE_START,
      .LimitAddress     = BL2_JUMP_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RO,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE,
      .DisablePrivExec  = MPU_INSTRUCTION_ACCESS_ENABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R4,
    FLOW_CTRL_MPU_I_EN_R4,
    FLOW_STEP_MPU_I_CH_R4,
    FLOW_CTRL_MPU_I_CH_R4,
#endif /* FLOW_CONTROL */
  },
  /* Region 5: Allows RW access on boot DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER5,
      .AttributesIndex  = MPU_ATTRIBUTE_DATA,
      .BaseAddress      = BL2_DATA_START,
      .LimitAddress     = BL2_DATA_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R5,
    FLOW_CTRL_MPU_I_EN_R5,
    FLOW_STEP_MPU_I_CH_R5,
    FLOW_CTRL_MPU_I_CH_R5,
#endif /* FLOW_CONTROL */
  },
  /* Region 6: Allows RW access on boot DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER6,
      .AttributesIndex  = MPU_ATTRIBUTE_DATA,
      .BaseAddress      = SRAM2_AHB_BASE_S,
      .LimitAddress     = SRAM2_AHB_BASE_S + SRAM2_AHB_SIZE - 1,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R6,
    FLOW_CTRL_MPU_I_EN_R6,
    FLOW_STEP_MPU_I_CH_R6,
    FLOW_CTRL_MPU_I_CH_R6,
#endif /* FLOW_CONTROL */
  },
#if (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
/* Region 7: Allows writing of the secure application in the execution area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER7,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = S_CODE_START,
      .LimitAddress     = S_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R7,
    FLOW_CTRL_MPU_I_EN_R7,
    FLOW_STEP_MPU_I_CH_R7,
    FLOW_CTRL_MPU_I_CH_R7,
#endif /* FLOW_CONTROL */
  },
  /* Region 8: Allows writing of the non secure application in the execution area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER8,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = NS_CODE_START,
      .LimitAddress     = NS_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R8,
    FLOW_CTRL_MPU_I_EN_R8,
    FLOW_STEP_MPU_I_CH_R8,
    FLOW_CTRL_MPU_I_CH_R8,
#endif /* FLOW_CONTROL */
  },
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  /* Region 9: Allows writing in secure DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER9,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = S_DATA2_START,
      .LimitAddress     = S_DATA2_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R9,
    FLOW_CTRL_MPU_I_EN_R9,
    FLOW_STEP_MPU_I_CH_R9,
    FLOW_CTRL_MPU_I_CH_R9,
#endif /* FLOW_CONTROL */
  },
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  /* Region 10: Allows writing in non secure DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER10,
      .AttributesIndex  = MPU_ATTRIBUTE_DATANOCACHE,
      .BaseAddress      = NS_DATA2_START,
      .LimitAddress     = NS_DATA2_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R10,
    FLOW_CTRL_MPU_I_EN_R10,
    FLOW_STEP_MPU_I_CH_R10,
    FLOW_CTRL_MPU_I_CH_R10,
#endif /* FLOW_CONTROL */
  },
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
};

#if defined(__ICCARM__)
#pragma location=".BL2_Jump_Data"
#endif
const MPU_Region_Config_t mpu_region_cfg_appli_s[] __attribute__((section(".BL2_Jump_Data"))) = {
  /* Region 3: Forbid execution of the boot and allow erase */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER3,
      .AttributesIndex  = MPU_ATTRIBUTE_CODE,
      .BaseAddress      = BL2_CODE_START - BOOTROM_HEADER_SIZE,
      .LimitAddress     = BL2_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_S_A_EN_R3,
    FLOW_CTRL_MPU_S_A_EN_R3,
    FLOW_STEP_MPU_S_A_CH_R3,
    FLOW_CTRL_MPU_S_A_CH_R3,
#endif /* FLOW_CONTROL */
  },
  /* Region 2/6: Allows execution of application secure */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
#if (OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
      .Number           = MPU_REGION_NUMBER2,
#else
      .Number           = MPU_REGION_NUMBER7,
#endif /* OEMUROT_LOAD_AND_RUN == NO_LOAD_AND_RUN */
      .AttributesIndex  = MPU_ATTRIBUTE_CODE,
      .BaseAddress      = S_CODE_START,
      .LimitAddress     = S_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RO,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE,
      .DisablePrivExec  = MPU_INSTRUCTION_ACCESS_ENABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_S_A_EN_R,
    FLOW_CTRL_MPU_S_A_EN_R,
    FLOW_STEP_MPU_S_A_CH_R,
    FLOW_CTRL_MPU_S_A_CH_R,
#endif /* FLOW_CONTROL */
  },
  /* Region 8: Allows reading of the non secure application */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER8,
      .AttributesIndex  = MPU_ATTRIBUTE_CODE,
      .BaseAddress      = NS_CODE_START,
      .LimitAddress     = NS_CODE_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RO,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_S_A_EN_R8,
    FLOW_CTRL_MPU_S_A_EN_R8,
    FLOW_STEP_MPU_S_A_CH_R8,
    FLOW_CTRL_MPU_S_A_CH_R8,
#endif /* FLOW_CONTROL */
  },
  /* Region 11: Allows RW access on application secure DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER11,
      .AttributesIndex  = MPU_ATTRIBUTE_DATA,
      .BaseAddress      = S_DATA_START,
      .LimitAddress     = S_DATA_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_S_A_EN_R11,
    FLOW_CTRL_MPU_S_A_EN_R11,
    FLOW_STEP_MPU_S_A_CH_R11,
    FLOW_CTRL_MPU_S_A_CH_R11,
#endif /* FLOW_CONTROL */
  },
  /* Region 12: Allows RW access on application non secure DATA area */
  {
    {
      .Enable           = MPU_REGION_ENABLE,
      .Number           = MPU_REGION_NUMBER12,
      .AttributesIndex  = MPU_ATTRIBUTE_DATA,
      .BaseAddress      = NS_DATA_START,
      .LimitAddress     = NS_DATA_LIMIT,
      .AccessPermission = MPU_REGION_PRIV_RW,
      .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
      .DisablePrivExec  = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE,
      .IsShareable      = MPU_ACCESS_NOT_SHAREABLE,
    },
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_S_A_EN_R12,
    FLOW_CTRL_MPU_S_A_EN_R12,
    FLOW_STEP_MPU_S_A_CH_R12,
    FLOW_CTRL_MPU_S_A_CH_R12,
#endif /* FLOW_CONTROL */
  },
};

#if defined(__ICCARM__)
#pragma location=".BL2_Jump_Data"
#endif

#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
static const RTC_SecureStateTypeDef TamperSecureConf = {
    .rtcSecureFull = RTC_SECURE_FULL_NO,
    .rtcNonSecureFeatures = RTC_NONSECURE_FEATURE_ALL,
    .tampSecureFull = TAMP_SECURE_FULL_YES,
    .MonotonicCounterSecure = TAMP_MONOTONIC_CNT_SECURE_NO,
    .backupRegisterStartZone2 = 0,
    .backupRegisterStartZone3 = 0
};
static const RTC_PrivilegeStateTypeDef TamperPrivConf = {
    .rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO,
    .rtcPrivilegeFeatures = RTC_PRIVILEGE_FEATURE_NONE,
    .tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES,
    .MonotonicCounterPrivilege = TAMP_MONOTONIC_CNT_PRIVILEGE_NO,
    .backupRegisterStartZone2 = 0,
    .backupRegisterStartZone3 = 0
};
static const RTC_InternalTamperTypeDef InternalTamperConf = {
    .IntTamper = RTC_INT_TAMPER_9,
    .TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE,
    .NoErase                  = RTC_TAMPER_ERASE_BACKUP_ENABLE
};
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */

#ifdef OEMUROT_DEV_MODE
extern volatile uint32_t TamperEventCleared;
#endif /* OEMUROT_DEV_MODE */

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup OEMUROT_SECURITY_Private_Functions  Private Functions
  * @{
  */
static void sau_init_cfg(void);
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);

#if MCUBOOT_APP_IMAGE_NUMBER == 2
static void rif_init_cfg(void);
static int8_t RIF_RISAF_CheckRegionConfig(RISAF_TypeDef *RISAFx, uint32_t Region, const RISAF_BaseRegionConfig_t *pConfig);
#endif

#ifdef OEMUROT_MPU_PROTECTION
static int8_t MPU_CheckRegion(MPU_Type *MPUx, const MPU_Region_InitTypeDef *MPU_Init);
static int8_t MPU_CheckMemoryAttributes(MPU_Type *MPUx, const MPU_Attributes_InitTypeDef *pMPU_AttributesInit);
#endif /* OEMUROT_MPU_PROTECTION */
static void active_tamper(void);

/**
  * @}
  */

/** @defgroup OEMUROT_SECURITY_Exported_Functions Exported Functions
  * @{
  */
/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void LL_SECU_ApplyRunTimeProtections(void)
{
  /* Configure and enable SAU */
  sau_init_cfg();

#if MCUBOOT_APP_IMAGE_NUMBER == 2
  /* Configure and enable RIF */
  rif_init_cfg();
#endif

  /* Set MPU to forbid execution outside of immutable code  */
  mpu_init_cfg();

#ifdef MCUBOOT_USE_MCE
  /* Configure and enable MCE */
  mce_init_cfg();
#endif /* MCUBOOT_USE_MCE */

  /* Enable tamper */
  active_tamper();
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Jump_Code"
#else
__attribute__((section(".BL2_Jump_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateRunTimeProtections(void)
{
  /* Update MPU config for application execution */
  mpu_appli_cfg();

#ifdef MCUBOOT_USE_MCE
  /* Lock external flash MCE */
  mce_lock_cfg();
#endif /* MCUBOOT_USE_MCE */

  /* Lock secret stored in OTP */
  OTP_Lock();

  /* Set HDPL to 2 */
  Increase_HDPL();
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

/**
  * @brief  Check if the Static security protections are applied.
  * @param  None
  * @retval None
  */
void LL_SECU_CheckStaticProtections(void)
{
#ifdef OEMUROT_SECURE_BOOT
  uint8_t prov_done = 0;
  uint8_t secure_boot = 0;

  /* Get device lifecycle */
  if (OTP_Get_Lifecycle_Status(&secure_boot, &prov_done) != HAL_OK)
  {
    BOOT_LOG_ERR("Failed to retrieving the device lifecycle status");
    Error_Handler();
  }

  /* Check secure_boot flag */
  if (secure_boot == 0)
  {
    BOOT_LOG_ERR("Unexpected value for secure boot");
    Error_Handler();
  }
#endif /* OEMUROT_SECURE_BOOT */
}

/**
  * @brief  SAU configuration
  * @param  None
  * @retval None
  */
static void sau_init_cfg(void)
{
  uint32_t i = 0U;
  uint32_t rnr = 0xFFFFFFFF;
  uint32_t rbar = 0xFFFFFFFF;
  uint32_t rlar = 0xFFFFFFFF;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Disable SAU */
    SAU->CTRL &= ~(SAU_CTRL_ENABLE_Msk);

    /* Configure SAU regions */
    for (i = 0; i < ARRAY_SIZE(region_sau_init_cfg); i++)
    {
      SAU->RNR = region_sau_init_cfg[i].RNR & SAU_RNR_REGION_Msk;
      SAU->RBAR = region_sau_init_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
      SAU->RLAR = (region_sau_init_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                 ((region_sau_init_cfg[i].nsc << SAU_RLAR_NSC_Pos) & SAU_RLAR_NSC_Msk) |
                       SAU_RLAR_ENABLE_Msk;

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, region_sau_init_cfg[i].flow_step_enable,
                                           region_sau_init_cfg[i].flow_ctrl_enable);
    }

    /* Force memory writes before continuing */
    __DSB();
    /* Flush and refill pipeline with updated permissions */
    __ISB();

    /* Enable SAU */
    SAU->CTRL |= SAU_CTRL_ENABLE_Msk;

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SAU_I_EN, FLOW_CTRL_SAU_I_EN);
  }
  /* Verification stage */
  else
  {
    for (i = 0; i < ARRAY_SIZE(region_sau_init_cfg); i++)
    {
      SAU->RNR = region_sau_init_cfg[i].RNR;

      rnr = region_sau_init_cfg[i].RNR;
      rbar = region_sau_init_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
      rlar = (region_sau_init_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
             (region_sau_init_cfg[i].nsc ? SAU_RLAR_NSC_Msk : 0U) |
                  SAU_RLAR_ENABLE_Msk;

      if ((SAU->RNR != rnr) || (SAU->RBAR != rbar) || (SAU->RLAR != rlar))
      {
        Error_Handler();
      }
      else
      {
        /* Execution stops if flow control fails */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_sau_init_cfg[i].flow_step_check,
                                             region_sau_init_cfg[i].flow_ctrl_check);
      }
    }


    if ((SAU->CTRL & SAU_CTRL_ENABLE_Msk) != SAU_CTRL_ENABLE_Msk)
    {
      Error_Handler();
    }
    else
    {
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SAU_I_CH, FLOW_CTRL_SAU_I_CH);
    }
  }
}


#if MCUBOOT_APP_IMAGE_NUMBER == 2
/**
  * @brief  RIF initialization
  * @param  None
  * @retval None
  */
static void rif_init_cfg(void)
{
  uint32_t i = 0U;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* RISAF Config */
    __HAL_RCC_RISAF_CLK_ENABLE();
    __HAL_RCC_RIFSC_CLK_ENABLE();

    for (i = 0; i < ARRAY_SIZE(risaf_cfg); i++)
    {
      HAL_RIF_RISAF_ConfigBaseRegion(risaf_cfg[i].RISAFx,
                                     risaf_cfg[i].Region,
                                     &risaf_cfg[i].BaseRegionConfig);
      FLOW_CONTROL_STEP(uFlowProtectValue, risaf_cfg[i].flow_step_enable,
                                           risaf_cfg[i].flow_ctrl_enable);
    }
  }
  /* Verification stage */
  else
  {
    for (i = 0; i < ARRAY_SIZE(risaf_cfg); i++)
    {
      if (RIF_RISAF_CheckRegionConfig(risaf_cfg[i].RISAFx,
                                      risaf_cfg[i].Region,
                                     &risaf_cfg[i].BaseRegionConfig) != 0)
      {
        Error_Handler();
      }
      else
      {
        FLOW_CONTROL_STEP(uFlowProtectValue, risaf_cfg[i].flow_step_check,
                                             risaf_cfg[i].flow_ctrl_check);
      }
    }
  }
}
#endif

/**
  * @brief  MPU cleaning
  * @param  None
  * @retval None
  */
void LL_SECU_DisableCleanMpu(void)
{
  uint8_t i;

  MPU->CTRL = 0;

  for(i = MPU_REGION_NUMBER0; i <= MPU_REGION_NUMBER15; i++)
  {
    HAL_MPU_DisableRegion(i);
  }
}

/**
  * @brief  MPU initial configuration
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef OEMUROT_MPU_PROTECTION
  uint32_t i;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg); i++)
    {
      HAL_MPU_ConfigRegion(&mpu_region_cfg[i].MPU_Region);
      FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg[i].flow_step_enable,
                                           mpu_region_cfg[i].flow_ctrl_enable);
    }

    /* Configure MPU memory attributes */
    for (i = 0; i < ARRAY_SIZE(mpu_attributes); i++)
    {
      HAL_MPU_ConfigMemoryAttributes(&mpu_attributes[i]);
    }

    /* Enable secure MPU */
    HAL_MPU_Enable(MPU_HARDFAULT_NMI);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_I_EN, FLOW_CTRL_MPU_S_I_EN);

  }
  /* Verification stage */
  else
  {
    /* Check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg); i++)
    {
      if (MPU_CheckRegion(MPU, &mpu_region_cfg[i].MPU_Region) != 0)
      {
        Error_Handler();
      }
      else
      {
        FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg[i].flow_step_check,
                                             mpu_region_cfg[i].flow_ctrl_check);
      }
    }

    /* Check MPU memory attributes */
    for (i = 0; i < ARRAY_SIZE(mpu_attributes); i++)
    {
      if (MPU_CheckMemoryAttributes(MPU, &mpu_attributes[i]) != 0)
      {
        Error_Handler();
      }
    }

    /* Verify that MPU is enabled */
    if (MPU->CTRL != (MPU_HARDFAULT_NMI | MPU_CTRL_ENABLE_Msk))
    {
      Error_Handler();
    }
    else
    {
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_I_CH, FLOW_CTRL_MPU_S_I_CH);
    }

  }
#endif /* OEMUROT_MPU_PROTECTION */
}

#if MCUBOOT_APP_IMAGE_NUMBER == 2
static int8_t RIF_RISAF_CheckRegionConfig(RISAF_TypeDef *RISAFx, uint32_t Region, const RISAF_BaseRegionConfig_t *pConfig)
{
  uint32_t cidcfgr = 0, cfgr = 0;

  /* Check if region is disabled */
  if (pConfig->Filtering == RISAF_FILTER_DISABLE)
  {
    if ((RISAFx->REG[Region].CFGR & (~(RISAF_FILTER_ENABLE))) == (~(RISAF_FILTER_ENABLE)))
    {
        return 0;
    }
  }
  else
  {
    cidcfgr = (pConfig->ReadWhitelist | (pConfig->WriteWhitelist << RISAF_REGx_CIDCFGR_WRENC0_Pos));
    cfgr = (pConfig->Filtering | (pConfig->Secure << RISAF_REGx_CFGR_SEC_Pos)
                               | (pConfig->PrivWhitelist << RISAF_REGx_CFGR_PRIVC0_Pos));

    /* Compare start and end addresses, filtering mode, security and privilege attributes and whitelists */
    if ((RISAFx->REG[Region].STARTR == pConfig->StartAddress) &&
        (RISAFx->REG[Region].ENDR == pConfig->EndAddress) &&
        (RISAFx->REG[Region].CIDCFGR == cidcfgr) &&
        (RISAFx->REG[Region].CFGR == cfgr))
        {
            return 0;
        }
  }

  return -1;
}
#endif

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Jump_Code"
#else
__attribute__((section(".BL2_Jump_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  MPU application configuration
  * @param  None
  * @retval None
  */
static void mpu_appli_cfg(void)
{
#ifdef OEMUROT_MPU_PROTECTION
  uint32_t i;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg_appli_s); i++)
    {
      HAL_MPU_ConfigRegion(&mpu_region_cfg_appli_s[i].MPU_Region);
      FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg_appli_s[i].flow_step_enable,
                                           mpu_region_cfg_appli_s[i].flow_ctrl_enable);
    }
  }
  /* Verification stage */
  else
  {
    /* Check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg_appli_s); i++)
    {
      if (MPU_CheckRegion(MPU, &mpu_region_cfg_appli_s[i].MPU_Region) != 0)
      {
        Error_Handler();
      }
      else
      {
        FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg_appli_s[i].flow_step_check,
                                             mpu_region_cfg_appli_s[i].flow_ctrl_check);
      }
    }
  }
#endif /* OEMUROT_MPU_PROTECTION */
}

#ifdef OEMUROT_MPU_PROTECTION
/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_Jump_Code")))
#endif /* __GNUC__ */
/**
  * @brief Compare with the current mpu memory attributes configuration
  * @param MPUx Pointer to MPU
  * @param pMPU_AttributesInit Pointer to a MPU_Attributes_InitTypeDef structure that contains
  *                the configuration information to compare with.
  * @retval None
  */
static int8_t MPU_CheckMemoryAttributes(MPU_Type *MPUx, const MPU_Attributes_InitTypeDef *pMPU_AttributesInit)
{
  __IO uint32_t *p_mair;
  uint32_t attr_values;
  uint32_t attr_number;

  if (pMPU_AttributesInit->Number < MPU_ATTRIBUTES_NUMBER4)
  {
    /* Program MPU_MAIR0 */
    p_mair = &(MPUx->MAIR0);
    attr_number = pMPU_AttributesInit->Number;
  }
  else
  {
    /* Program MPU_MAIR1 */
    p_mair = &(MPUx->MAIR1);
    attr_number = (uint32_t)pMPU_AttributesInit->Number - 4U;
  }

  attr_values = *(p_mair);
  attr_values &=  ~(0xFFU << (attr_number * 8U));
  if (*(p_mair) == (attr_values | ((uint32_t)pMPU_AttributesInit->Attributes << (attr_number * 8U))))
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_Jump_Code")))
#endif /* __GNUC__ */
/**
  * @brief Compare with the current mpu configuration
  * @param MPUx Pointer to MPU
  * @param MPU_Init Pointer to a MPU_Region_InitTypeDef structure that contains
  *                the configuration information to compare with.
  * @retval None
  */
static int8_t MPU_CheckRegion(MPU_Type *MPUx, const MPU_Region_InitTypeDef *MPU_Init)
{
  uint32_t base_cfg = 0x0;
  uint32_t limit_cfg = 0xFFFFFFFF;

  /* Set the Region number */
  MPUx->RNR = MPU_Init->Number;

  base_cfg = (((uint32_t)MPU_Init->BaseAddress & 0xFFFFFFE0UL)  |
              ((uint32_t)MPU_Init->IsShareable      << MPU_RBAR_SH_Pos)  |
              ((uint32_t)MPU_Init->AccessPermission << MPU_RBAR_AP_Pos)  |
              ((uint32_t)MPU_Init->DisableExec      << MPU_RBAR_XN_Pos));

  limit_cfg = (((uint32_t)MPU_Init->LimitAddress & 0xFFFFFFE0UL) |
               ((uint32_t)MPU_Init->DisablePrivExec  << MPU_RLAR_PXN_Pos) |
               ((uint32_t)MPU_Init->AttributesIndex  << MPU_RLAR_AttrIndx_Pos) |
               ((uint32_t)MPU_Init->Enable           << MPU_RLAR_EN_Pos));

  if ((MPUx->RBAR == base_cfg) && (MPUx->RLAR == limit_cfg))
  {
    return 0;
  }
  else
  {
    return -1;
  }
}
#endif /* OEMUROT_MPU_PROTECTION */

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

void TAMP_IRQHandler(void)
{
    NVIC_SystemReset();
}


static void active_tamper(void)
{
  RTC_HandleTypeDef RTCHandle = {0};

#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
  RTC_SecureStateTypeDef TamperSecureConfGet = {0};
  RTC_PrivilegeStateTypeDef TamperPrivConfGet = {0};
  fih_int fih_rc = FIH_FAILURE;
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
#if defined(OEMUROT_DEV_MODE) && (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
    if (TamperEventCleared)
    {
      BOOT_LOG_INF("Boot with TAMPER Event Active");
      BOOT_LOG_INF("Build and Flash with commented line #define OEMUROT_INTERNAL_TAMPER_ENABLE\n");
      Error_Handler();
    }
#endif /* defined(OEMUROT_DEV_MODE) && (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY) */

    /* RTC Init */
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
    RTCHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;
    RTCHandle.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;

    if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
      Error_Handler();
    }

#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
    /*  Internal Tamper activation  */
    /*  Enable Cryptographic IPs fault (tamp_itamp9) */
    if (HAL_RTCEx_SetInternalTamper(&RTCHandle,(RTC_InternalTamperTypeDef *)&InternalTamperConf)!=HAL_OK)
    {
        Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_EN, FLOW_CTRL_TAMP_INT_EN);

    /*  Set tamper configuration secure only  */
    if (HAL_RTCEx_SecureModeSet(&RTCHandle, (RTC_SecureStateTypeDef *)&TamperSecureConf) != HAL_OK)
    {
        Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_SEC_EN, FLOW_CTRL_TAMP_SEC_EN);

    /*  Set tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeSet(&RTCHandle,(RTC_PrivilegeStateTypeDef *)&TamperPrivConf) != HAL_OK)
    {
        Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_EN, FLOW_CTRL_TAMP_PRIV_EN);

    /*  Activate Secret Erase */
    HAL_RTCEx_Erase_SecretDev_Conf(&RTCHandle,(uint32_t)TAMP_SECRETDEVICE_ERASE_BKP_SRAM);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_EN, FLOW_CTRL_TAMP_CFG_EN);

    BOOT_LOG_INF("TAMPER Activated");
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */
  }
  /* Verification stage */
  else
  {
#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
    /*  Check Internal Tamper activation */
    if ((READ_BIT(RTC->CR, RTC_CR_TAMPTS) != InternalTamperConf.TimeStampOnTamperDetection) ||
        (READ_REG(TAMP->CR1) != 0x01000000U) ||
        (READ_REG(TAMP->CR3) != 0x00000000U))
    {
        Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_CH, FLOW_CTRL_TAMP_INT_CH);

    /*  Check tamper configuration secure only  */
    if (HAL_RTCEx_SecureModeGet(&RTCHandle, (RTC_SecureStateTypeDef *)&TamperSecureConfGet) != HAL_OK)
    {
        Error_Handler();
    }
    FIH_CALL(boot_fih_memequal, fih_rc,(void *)&TamperSecureConf, (void *)&TamperSecureConfGet, sizeof(TamperSecureConf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
            Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_SEC_CH, FLOW_CTRL_TAMP_SEC_CH);

    /*  Check tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeGet(&RTCHandle,(RTC_PrivilegeStateTypeDef *)&TamperPrivConfGet) != HAL_OK)
    {
        Error_Handler();
    }
    FIH_CALL(boot_fih_memequal, fih_rc,(void *)&TamperPrivConf, (void *)&TamperPrivConfGet, sizeof(TamperPrivConf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
            Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_CH, FLOW_CTRL_TAMP_PRIV_CH);

    /*  Check Secret Erase */
    if (READ_BIT(TAMP->RPCFGR, TAMP_RPCFGR_RPCFG0) != TAMP_RPCFGR_RPCFG0)
    {
        Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_CH, FLOW_CTRL_TAMP_CFG_CH);
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */
  }
}
