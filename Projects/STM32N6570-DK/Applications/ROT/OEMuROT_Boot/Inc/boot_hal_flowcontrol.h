/**
  ******************************************************************************
  * @file    boot_hal_flowcontrol.h
  * @author  MCD Application Team
  * @brief   Header for flow control in boot_hal.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOT_HAL_FLOWCONTROL_H
#define BOOT_HAL_FLOWCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "region_defs.h"
#include "boot_hal_cfg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/**
  * @brief  Flow Control : Initial value
  */
#define FLOW_STAGE_CFG              0xfffffe00U
#define FLOW_STAGE_CHK              0xffffe01eU

/**
  * @brief  Flow Control : Initial value
  */
#define FLOW_CTRL_INIT_VALUE        0x00005776U      /*!< Init value definition */

/**
  * @brief  Flow Control : Steps definition
  */
#define FLOW_STEP_SAU_I_EN_R0       0x00006787U      /*!< Step SAU Region 0 Init enable value */
#define FLOW_STEP_SAU_I_EN_R1       0x00007999U      /*!< Step SAU Region 1 Init enable value */
#define FLOW_STEP_SAU_I_EN_R2       0x0000aab5U      /*!< Step SAU Region 2 Init enable value */
#define FLOW_STEP_SAU_I_EN_R3       0x0000b4abU      /*!< Step SAU Region 3 Init enable value */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_SAU_I_EN_R4       0x0000cccdU      /*!< Step SAU Region 4 Init enable value */
#else
#define FLOW_STEP_SAU_I_EN_R4       0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#define FLOW_STEP_SAU_I_EN          0x0000d2d3U      /*!< Step SAU Init enable value */

#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLOW_STEP_RIF_EN_R1         0x00043d7aU      /*!< Step RIF Region 1 Init enable value */
#define FLOW_STEP_RIF_EN_R2         0x0001cf0eU      /*!< Step RIF Region 2 Init enable value */
#else
#define FLOW_STEP_RIF_EN_R1         0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_RIF_EN_R2         0x00000000U      /*!< No effect on control flow */
#endif

#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_RIF_EN_R3         0x0000e3dcU      /*!< Step RIF Region 3 Init enable value */
#else
#define FLOW_STEP_RIF_EN_R3         0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */

#ifdef OEMUROT_MPU_PROTECTION
#define FLOW_STEP_MPU_I_EN_R0       0x0000fdc2U      /*!< Step MPU Region 0 Init enable value */
#define FLOW_STEP_MPU_I_EN_R1       0x00012cd3U      /*!< Step MPU Region 1 Init enable value */
#define FLOW_STEP_MPU_I_EN_R2       0x000132cdU      /*!< Step MPU Region 2 Init enable value */
#define FLOW_STEP_MPU_I_EN_R3       0x00014aabU      /*!< Step MPU Region 3 Init enable value */
#define FLOW_STEP_MPU_I_EN_R4       0x000156a3U      /*!< Step MPU Region 4 Init enable value */
#define FLOW_STEP_MPU_I_EN_R5       0x00016d1fU      /*!< Step MPU Region 5 Init enable value */
#define FLOW_STEP_MPU_I_EN_R6       0x000174b5U      /*!< Step MPU Region 6 Init enable value */
#if (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#define FLOW_STEP_MPU_I_EN_R7       0x00018799U      /*!< Step MPU Region 7 Init enable value */
#define FLOW_STEP_MPU_I_EN_R8       0x00019987U      /*!< Step MPU Region 8 Init enable value */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_I_EN_R9       0x0001b696U      /*!< Step MPU Region 9 Init enable value */
#else
#define FLOW_STEP_MPU_I_EN_R9       0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_I_EN_R10       0x000673eaU     /*!< Step MPU Region 10 Init enable value */
#else
#define FLOW_STEP_MPU_I_EN_R10       0x00000000U     /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#else
#define FLOW_STEP_MPU_I_EN_R7       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R8       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R9       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R10      0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#define FLOW_STEP_MPU_S_I_EN        0x00067f4dU      /*!< Step MPU Secure Init enable value */
#define FLOW_STEP_MPU_NS_I_EN       0x0006ad9fU      /*!< Step MPU Non Secure Init enable value */
#else /* OEMUROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_I_EN_R0       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R1       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R2       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R3       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R4       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R5       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R6       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R7       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R8       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R9       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R10      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_I_EN        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_EN       0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_MPU_PROTECTION */

#ifdef MCUBOOT_USE_MCE
#define FLOW_STEP_MCE1_EN_R1        0x0006b381U      /*!< Step MCE1 Region 1 Init enable value */
#if defined(MCUBOOT_USE_HASH_REF)
#define FLOW_STEP_MCE1_EN_R2        0x0006bf26U      /*!< Step MCE1 Region 2 Init enable value */
#else
#define FLOW_STEP_MCE1_EN_R2        0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_HASH_REF */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE1_EN_R3        0x0001cef0U      /*!< Step MCE1 Region 3 Init enable value */
#else
#define FLOW_STEP_MCE1_EN_R3        0x00000000U      /*!< No effect on control flow */
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */

#define FLOW_STEP_MCE1_AES_CTX_EN   0x0001d0eeU      /*!< Step MCE1 AES context setting enable value */
#define FLOW_STEP_MCE1_AES_CFG_EN   0x0001d345U      /*!< Step MCE1 AES configuration enable value */
#define FLOW_STEP_MCE1_AES_KEY_LOCK 0x0001e1e1U      /*!< Step MCE1 AES key setting lock value */
#define FLOW_STEP_MCE1_AES_EN       0x0001e509U      /*!< Step MCE1 AES enable value */
#define FLOW_STEP_MCE1_AES_CFG_LOCK 0x0001ffffU      /*!< Step MCE1 AES configuration lock value */
#else
#define FLOW_STEP_MCE1_EN_R1        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_EN_R2        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_EN_R3        0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE1_AES_CTX_EN   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_CFG_EN   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_KEY_LOCK 0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_EN       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_CFG_LOCK 0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_MCE */

#if defined(MCUBOOT_USE_MCE) && (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE2_EN_R1        0x00023bd7U      /*!< Step MCE2 Region 1 Init enable value */

#define FLOW_STEP_MCE2_AES_CTX_EN   0x00025dafU      /*!< Step MCE2 AES context setting enable value */
#define FLOW_STEP_MCE2_AES_CFG_EN   0x00026312U      /*!< Step MCE2 AES configuration enable value */
#define FLOW_STEP_MCE2_AES_KEY_LOCK 0x00027dafU      /*!< Step MCE2 AES key setting lock value */
#define FLOW_STEP_MCE2_AES_EN       0x0002810aU      /*!< Step MCE2 AES enable value */
#define FLOW_STEP_MCE2_AES_CFG_LOCK 0x00031a37U      /*!< Step MCE2 AES configuration lock value */
#define FLOW_STEP_MCE2_AES_LOCK     0x000331e0U      /*!< Step MCE2 AES lock value */
#else
#define FLOW_STEP_MCE2_EN_R1        0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE2_AES_CTX_EN   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_CFG_EN   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_KEY_LOCK 0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_EN       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_CFG_LOCK 0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_LOCK     0x00000000U      /*!< No effect on control flow */
#endif /* defined(MCUBOOT_USE_MCE) && (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */

#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
#define FLOW_STEP_TAMP_INT_EN       0x00035407U      /*!< Step Tamper internal enable value */
#define FLOW_STEP_TAMP_SEC_EN       0x00039371U      /*!< Step Tamper secure mode setting value */
#define FLOW_STEP_TAMP_PRIV_EN      0x0003aa07U      /*!< Step Tamper privilege mode setting value */
#define FLOW_STEP_TAMP_CFG_EN       0x0003f04fU      /*!< Step Tamper secret erase setting value */
#else
#define FLOW_STEP_TAMP_INT_EN       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_SEC_EN       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_PRIV_EN      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_CFG_EN       0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */

#define FLOW_STEP_SAU_I_CH_R0       0x0001fe01U      /*!< Step SAU Region 0 Init check value */
#define FLOW_STEP_SAU_I_CH_R1       0x0002e8fbU      /*!< Step SAU Region 1 Init check value */
#define FLOW_STEP_SAU_I_CH_R2       0x0002f71bU      /*!< Step SAU Region 2 Init check value */
#define FLOW_STEP_SAU_I_CH_R3       0x0002fb70U      /*!< Step SAU Region 3 Init check value */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_SAU_I_CH_R4       0x00031b6eU      /*!< Step SAU Region 4 Init check value */
#else
#define FLOW_STEP_SAU_I_CH_R4       0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#define FLOW_STEP_SAU_I_CH          0x00036e9dU      /*!< Step SAU Init check value */

#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLOW_STEP_RIF_CH_R1         0x0051ebeaU      /*!< Step RIF Region 1 Init check value */
#define FLOW_STEP_RIF_CH_R2         0x0003717dU      /*!< Step RIF Region 2 Init check value */
#else
#define FLOW_STEP_RIF_CH_R1         0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_RIF_CH_R2         0x00000000U      /*!< No effect on control flow */
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_RIF_CH_R3         0x00037d16U      /*!< Step SAU Region 3 Init check value */
#else
#define FLOW_STEP_RIF_CH_R3         0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */

#ifdef OEMUROT_MPU_PROTECTION
#define FLOW_STEP_MPU_I_CH_R0       0x0003b1daU      /*!< Step MPU Region 0 Init check value */
#define FLOW_STEP_MPU_I_CH_R1       0x0003bdb1U      /*!< Step MPU Region 1 Init check value */
#define FLOW_STEP_MPU_I_CH_R2       0x0003c9bcU      /*!< Step MPU Region 2 Init check value */
#define FLOW_STEP_MPU_I_CH_R3       0x0006ca19U      /*!< Step MPU Region 3 Init check value */
#define FLOW_STEP_MPU_I_CH_R4       0x0006d407U      /*!< Step MPU Region 4 Init check value */
#define FLOW_STEP_MPU_I_CH_R5       0x0006d95eU      /*!< Step MPU Region 5 Init check value */
#define FLOW_STEP_MPU_I_CH_R6       0x0006ffd1U      /*!< Step MPU Region 6 Init check value */
#if (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#define FLOW_STEP_MPU_I_CH_R7       0x0003d7a2U      /*!< Step MPU Region 7 Init check value */
#define FLOW_STEP_MPU_I_CH_R8       0x0003dbc9U      /*!< Step MPU Region 8 Init check value */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_I_CH_R9       0x0003eafcU      /*!< Step MPU Region 9 Init check value */
#else
#define FLOW_STEP_MPU_I_CH_R9       0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_I_CH_R10      0x0003ff63U      /*!< Step MPU Region 10 Init check value */
#else
#define FLOW_STEP_MPU_I_CH_R10      0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#else
#define FLOW_STEP_MPU_I_CH_R7       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R8       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R9       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R10      0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#define FLOW_STEP_MPU_S_I_CH        0x00000000U      /*!< Step MPU Secure Init check value */
#define FLOW_STEP_MPU_NS_I_CH       0x00000000U      /*!< Step MPU Non Secure Init check value */
#else /* OEMUROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_I_CH_R0       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R1       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R2       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R3       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R4       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R5       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R6       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R7       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R8       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R9       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R10      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_I_CH        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_CH       0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_MPU_PROTECTION */

#ifdef MCUBOOT_USE_MCE
#define FLOW_STEP_MCE1_CH_R1        0x000404dfU      /*!< Step MCE1 Region 1 Init check value */
#if defined(MCUBOOT_USE_HASH_REF)
#define FLOW_STEP_MCE1_CH_R2        0x000417f3U      /*!< Step MCE1 Region 2 Init check value */
#else
#define FLOW_STEP_MCE1_CH_R2        0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_HASH_REF */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE1_CH_R3        0x00042ce8U      /*!< Step MCE1 Region 3 Init check value */
#else
#define FLOW_STEP_MCE1_CH_R3        0x00000000U      /*!< No effect on control flow */
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */

#define FLOW_STEP_MCE1_AES_CTX_CH        0x000438cbU      /*!< Step MCE1 AES context setting check value */
#define FLOW_STEP_MCE1_AES_CFG_CH        0x000440adU      /*!< Step MCE1 AES configuration check value */

#define FLOW_STEP_MCE1_AES_KEY_LOCK_CH   0x00045e0bU      /*!< Step MCE1 AES key setting lock check value */
#define FLOW_STEP_MCE1_AES_CFG_LOCK_CH   0x000465c3U      /*!< Step MCE1 AES configuration lock check value */
#else
#define FLOW_STEP_MCE1_CH_R1             0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_CH_R2             0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_CH_R3             0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE1_AES_CTX_CH        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_CFG_CH        0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE1_AES_KEY_LOCK_CH   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE1_AES_CFG_LOCK_CH   0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_MCE */

#if defined(MCUBOOT_USE_MCE) && (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE2_CH_R1             0x00049aefU      /*!< Step MCE2 Region 1 Init check value */

#define FLOW_STEP_MCE2_AES_CTX_CH        0x0004a930U      /*!< Step MCE2 AES context setting check value */
#define FLOW_STEP_MCE2_AES_CFG_CH        0x0004c77bU      /*!< Step MCE2 AES configuration check value */

#define FLOW_STEP_MCE2_AES_KEY_LOCK_CH   0x0004db13U      /*!< Step MCE2 AES key setting lock check value */
#define FLOW_STEP_MCE2_AES_CFG_LOCK_CH   0x0004e23fU      /*!< Step MCE2 AES configuration lock check value */
#define FLOW_STEP_MCE2_AES_LOCK_CH       0x0004fc61U      /*!< Step MCE2 AES lock check value */
#else
#define FLOW_STEP_MCE2_CH_R1             0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE2_AES_CTX_CH        0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_CFG_CH        0x00000000U      /*!< No effect on control flow */

#define FLOW_STEP_MCE2_AES_KEY_LOCK_CH   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_CFG_LOCK_CH   0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MCE2_AES_LOCK_CH       0x00000000U      /*!< No effect on control flow */
#endif /* defined(MCUBOOT_USE_MCE) && (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */

#if (OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY)
#define FLOW_STEP_TAMP_INT_CH       0x00051407U      /*!< Step Tamper internal check value */
#define FLOW_STEP_TAMP_SEC_CH       0x00053371U      /*!< Step Tamper secure mode check value */
#define FLOW_STEP_TAMP_PRIV_CH      0x00055a07U      /*!< Step Tamper privilege mode check value */
#define FLOW_STEP_TAMP_CFG_CH       0x0005704fU      /*!< Step Tamper secret erase check value */
#else
#define FLOW_STEP_TAMP_INT_CH       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_SEC_CH       0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_PRIV_CH      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_TAMP_CFG_CH       0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_TAMPER_ENABLE == INTERNAL_TAMPER_ONLY */

#ifdef OEMUROT_MPU_PROTECTION
#define FLOW_STEP_MPU_S_A_EN_R3     0x0005953fU      /*!< Step MPU Secure Region 3 Appli enable value */
#define FLOW_STEP_MPU_S_A_EN_R      0x0005afd6U      /*!< Step MPU Secure Region Appli enable value */
#define FLOW_STEP_MPU_S_A_EN_R8     0x0005bae2U      /*!< Step MPU Secure Region 8 Appli enable value */
#define FLOW_STEP_MPU_S_A_EN_R11    0x0005c37aU      /*!< Step MPU Secure Region 11 Appli enable value */
#define FLOW_STEP_MPU_S_A_EN_R12    0x0005dd64U      /*!< Step MPU Secure Region 12 Appli enable value */
#define FLOW_STEP_MPU_NS_A_EN_R0    0x0005ed59U      /*!< Step MPU Non Secure Region 0 Appli enable value */
#define FLOW_STEP_MPU_NS_A_EN_R1    0x0005f347U      /*!< Step MPU Non Secure Region 1 Appli enable value */
#define FLOW_STEP_MPU_NS_A_EN_R2    0x0006072bU      /*!< Step MPU Non Secure Region 2 Appli enable value */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_NS_A_EN_R3    0x00061935U      /*!< Step MPU Non Secure Region 3 Appli enable value */
#else
#define FLOW_STEP_MPU_NS_A_EN_R3    0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#else /* OEMUROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_S_A_EN_R3     0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_EN_R      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_EN_R8     0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_EN_R11    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_EN_R12    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_EN_R0    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_EN_R1    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_EN_R2    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_EN_R3    0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_MPU_PROTECTION */

#ifdef MCUBOOT_USE_MCE
#define FLOW_STEP_MCE1_AES_LOCK     0x00063fd9U      /*!< Step MCE1 AES lock value */
#else
#define FLOW_STEP_MCE1_AES_LOCK     0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_MCE */

#define FLOW_STEP_OTP_NVCNT_EN      0x00066153U      /*!< Step OTP NVCNT Lock enable value */
#define FLOW_STEP_OTP_DA_EN         0x00069f37U      /*!< Step OTP DA PW HASH Lock enable value */
#define FLOW_STEP_OTP_S_PBK_EN      0x0006b4a2U      /*!< Step OTP Secure Auth key Lock enable value */
#define FLOW_STEP_OTP_NS_PBK_EN     0x0006c2faU      /*!< Step OTP NonSecure Auth key Lock enable value */
#define FLOW_STEP_OTP_ENCK_EN       0x0006d0cdU      /*!< Step OTP Encryption key Lock enable value */
#define FLOW_STEP_HDPL_INC          0x00070d33U      /*!< Step HDPL increase enable value */

#ifdef OEMUROT_MPU_PROTECTION
#define FLOW_STEP_MPU_S_A_CH_R3     0x00071fdcU      /*!< Step MPU Secure Region 3 Appli check value */
#define FLOW_STEP_MPU_S_A_CH_R      0x00072434U      /*!< Step MPU Secure Region Appli check value */
#define FLOW_STEP_MPU_S_A_CH_R8     0x00075778U      /*!< Step MPU Secure Region 8 Appli check value */
#define FLOW_STEP_MPU_S_A_CH_R11    0x000784feU      /*!< Step MPU Secure Region 11 Appli check value */
#define FLOW_STEP_MPU_S_A_CH_R12    0x0007b621U      /*!< Step MPU Secure Region 12 Appli check value */
#define FLOW_STEP_MPU_NS_A_CH_R0    0x0007e74fU      /*!< Step MPU Non Secure Region 0 Appli check value */
#define FLOW_STEP_MPU_NS_A_CH_R1    0x0007f43eU      /*!< Step MPU Non Secure Region 1 Appli check value */
#define FLOW_STEP_MPU_NS_A_CH_R2    0x00080642U      /*!< Step MPU Non Secure Region 2 Appli check value */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLOW_STEP_MPU_NS_A_CH_R3    0x00083643U      /*!< Step MPU Non Secure Region 3 Appli check value */
#else
#define FLOW_STEP_MPU_NS_A_CH_R3    0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#else /* OEMUROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_S_A_CH_R3     0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_CH_R      0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_CH_R8     0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_CH_R11    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_S_A_CH_R12    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_CH_R0    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_CH_R1    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_CH_R2    0x00000000U      /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_A_CH_R3    0x00000000U      /*!< No effect on control flow */
#endif /* OEMUROT_MPU_PROTECTION */

#ifdef MCUBOOT_USE_MCE
#define FLOW_STEP_MCE1_AES_LOCK_CH  0x000840d8U      /*!< Step MCE1 AES lock check value */
#else
#define FLOW_STEP_MCE1_AES_LOCK_CH  0x00000000U      /*!< No effect on control flow */
#endif /* MCUBOOT_USE_MCE */

#define FLOW_STEP_OTP_NVCNT_CH      0x00085f3aU      /*!< Step OTP NVCNT Lock check value */
#define FLOW_STEP_OTP_DA_CH         0x000894eeU      /*!< Step OTP DA PW HASH Lock check value */
#define FLOW_STEP_OTP_S_PBK_CH      0x0008ad07U      /*!< Step OTP Secure Auth key Lock check value */
#define FLOW_STEP_OTP_NS_PBK_CH     0x0008f944U      /*!< Step OTP NonSecure Auth key Lock check value */
#define FLOW_STEP_OTP_ENCK_CH       0x00093f18U      /*!< Step OTP Encryption key Lock check value */

#define FLOW_STEP_HDPL_CH1          0x000995efU      /*!< Step HDPL increase 1 check value */
#define FLOW_STEP_HDPL_CH2          0x0009ec04U      /*!< Step HDPL increase 2 check value */

/**
  * @brief  SFU_BOOT Flow Control : Control values runtime protections
  */
/* Flow control Stage 1 */
#define FLOW_CTRL_SAU_I_EN_R0       (FLOW_CTRL_INIT_VALUE ^ FLOW_STEP_SAU_I_EN_R0)
#define FLOW_CTRL_SAU_I_EN_R1       (FLOW_CTRL_SAU_I_EN_R0 ^ FLOW_STEP_SAU_I_EN_R1)
#define FLOW_CTRL_SAU_I_EN_R2       (FLOW_CTRL_SAU_I_EN_R1 ^ FLOW_STEP_SAU_I_EN_R2)
#define FLOW_CTRL_SAU_I_EN_R3       (FLOW_CTRL_SAU_I_EN_R2 ^ FLOW_STEP_SAU_I_EN_R3)
#define FLOW_CTRL_SAU_I_EN_R4       (FLOW_CTRL_SAU_I_EN_R3 ^ FLOW_STEP_SAU_I_EN_R4)
#define FLOW_CTRL_SAU_I_EN          (FLOW_CTRL_SAU_I_EN_R4 ^ FLOW_STEP_SAU_I_EN)

#define FLOW_CTRL_RIF_EN_R1         (FLOW_CTRL_SAU_I_EN ^ FLOW_STEP_RIF_EN_R1)
#define FLOW_CTRL_RIF_EN_R2         (FLOW_CTRL_RIF_EN_R1 ^ FLOW_STEP_RIF_EN_R2)
#define FLOW_CTRL_RIF_EN_R3         (FLOW_CTRL_RIF_EN_R2 ^ FLOW_STEP_RIF_EN_R3)

#define FLOW_CTRL_MPU_I_EN_R0       (FLOW_CTRL_RIF_EN_R3 ^ FLOW_STEP_MPU_I_EN_R0)
#define FLOW_CTRL_MPU_I_EN_R1       (FLOW_CTRL_MPU_I_EN_R0 ^ FLOW_STEP_MPU_I_EN_R1)
#define FLOW_CTRL_MPU_I_EN_R2       (FLOW_CTRL_MPU_I_EN_R1 ^ FLOW_STEP_MPU_I_EN_R2)
#define FLOW_CTRL_MPU_I_EN_R3       (FLOW_CTRL_MPU_I_EN_R2 ^ FLOW_STEP_MPU_I_EN_R3)
#define FLOW_CTRL_MPU_I_EN_R4       (FLOW_CTRL_MPU_I_EN_R3 ^ FLOW_STEP_MPU_I_EN_R4)
#define FLOW_CTRL_MPU_I_EN_R5       (FLOW_CTRL_MPU_I_EN_R4 ^ FLOW_STEP_MPU_I_EN_R5)
#define FLOW_CTRL_MPU_I_EN_R6       (FLOW_CTRL_MPU_I_EN_R5 ^ FLOW_STEP_MPU_I_EN_R6)
#define FLOW_CTRL_MPU_I_EN_R7       (FLOW_CTRL_MPU_I_EN_R6 ^ FLOW_STEP_MPU_I_EN_R7)
#define FLOW_CTRL_MPU_I_EN_R8       (FLOW_CTRL_MPU_I_EN_R7 ^ FLOW_STEP_MPU_I_EN_R8)
#define FLOW_CTRL_MPU_I_EN_R9       (FLOW_CTRL_MPU_I_EN_R8 ^ FLOW_STEP_MPU_I_EN_R9)
#define FLOW_CTRL_MPU_I_EN_R10      (FLOW_CTRL_MPU_I_EN_R9 ^ FLOW_STEP_MPU_I_EN_R10)
#define FLOW_CTRL_MPU_S_I_EN        (FLOW_CTRL_MPU_I_EN_R10 ^ FLOW_STEP_MPU_S_I_EN)

#define FLOW_CTRL_MCE1_EN_R1        (FLOW_CTRL_MPU_S_I_EN ^ FLOW_STEP_MCE1_EN_R1)
#define FLOW_CTRL_MCE1_EN_R2        (FLOW_CTRL_MCE1_EN_R1 ^ FLOW_STEP_MCE1_EN_R2)
#define FLOW_CTRL_MCE1_EN_R3        (FLOW_CTRL_MCE1_EN_R2 ^ FLOW_STEP_MCE1_EN_R3)

#define FLOW_CTRL_MCE1_AES_CTX_EN   (FLOW_CTRL_MCE1_EN_R3 ^ FLOW_STEP_MCE1_AES_CTX_EN)
#define FLOW_CTRL_MCE1_AES_CFG_EN   (FLOW_CTRL_MCE1_AES_CTX_EN ^ FLOW_STEP_MCE1_AES_CFG_EN)
#define FLOW_CTRL_MCE1_AES_KEY_LOCK (FLOW_CTRL_MCE1_AES_CFG_EN ^ FLOW_STEP_MCE1_AES_KEY_LOCK)
#define FLOW_CTRL_MCE1_AES_EN       (FLOW_CTRL_MCE1_AES_KEY_LOCK ^ FLOW_STEP_MCE1_AES_EN)
#define FLOW_CTRL_MCE1_AES_CFG_LOCK (FLOW_CTRL_MCE1_AES_EN ^ FLOW_STEP_MCE1_AES_CFG_LOCK)

#define FLOW_CTRL_MCE2_EN_R1        (FLOW_CTRL_MCE1_AES_CFG_LOCK ^ FLOW_STEP_MCE2_EN_R1)

#define FLOW_CTRL_MCE2_AES_CTX_EN   (FLOW_CTRL_MCE2_EN_R1 ^ FLOW_STEP_MCE2_AES_CTX_EN)
#define FLOW_CTRL_MCE2_AES_CFG_EN   (FLOW_CTRL_MCE2_AES_CTX_EN ^ FLOW_STEP_MCE2_AES_CFG_EN)
#define FLOW_CTRL_MCE2_AES_KEY_LOCK (FLOW_CTRL_MCE2_AES_CFG_EN ^ FLOW_STEP_MCE2_AES_KEY_LOCK)
#define FLOW_CTRL_MCE2_AES_EN       (FLOW_CTRL_MCE2_AES_KEY_LOCK ^ FLOW_STEP_MCE2_AES_EN)
#define FLOW_CTRL_MCE2_AES_CFG_LOCK (FLOW_CTRL_MCE2_AES_EN ^ FLOW_STEP_MCE2_AES_CFG_LOCK)
#define FLOW_CTRL_MCE2_AES_LOCK     (FLOW_CTRL_MCE2_AES_CFG_LOCK ^ FLOW_STEP_MCE2_AES_LOCK)

#define FLOW_CTRL_TAMP_INT_EN       (FLOW_CTRL_MCE2_AES_LOCK ^ FLOW_STEP_TAMP_INT_EN)
#define FLOW_CTRL_TAMP_SEC_EN       (FLOW_CTRL_TAMP_INT_EN ^ FLOW_STEP_TAMP_SEC_EN)
#define FLOW_CTRL_TAMP_PRIV_EN      (FLOW_CTRL_TAMP_SEC_EN ^ FLOW_STEP_TAMP_PRIV_EN)
#define FLOW_CTRL_TAMP_CFG_EN       (FLOW_CTRL_TAMP_PRIV_EN ^ FLOW_STEP_TAMP_CFG_EN)

#define FLOW_CTRL_STAGE_1           FLOW_CTRL_TAMP_CFG_EN

/* Flow control Stage 2 */
#define FLOW_CTRL_SAU_I_CH_R0       (FLOW_CTRL_STAGE_1 ^ FLOW_STEP_SAU_I_CH_R0)
#define FLOW_CTRL_SAU_I_CH_R1       (FLOW_CTRL_SAU_I_CH_R0 ^ FLOW_STEP_SAU_I_CH_R1)
#define FLOW_CTRL_SAU_I_CH_R2       (FLOW_CTRL_SAU_I_CH_R1 ^ FLOW_STEP_SAU_I_CH_R2)
#define FLOW_CTRL_SAU_I_CH_R3       (FLOW_CTRL_SAU_I_CH_R2 ^ FLOW_STEP_SAU_I_CH_R3)
#define FLOW_CTRL_SAU_I_CH_R4       (FLOW_CTRL_SAU_I_CH_R3 ^ FLOW_STEP_SAU_I_CH_R4)
#define FLOW_CTRL_SAU_I_CH          (FLOW_CTRL_SAU_I_CH_R4 ^ FLOW_STEP_SAU_I_CH)

#define FLOW_CTRL_RIF_CH_R1         (FLOW_CTRL_SAU_I_CH ^ FLOW_STEP_RIF_CH_R1)
#define FLOW_CTRL_RIF_CH_R2         (FLOW_CTRL_RIF_CH_R1 ^ FLOW_STEP_RIF_CH_R2)
#define FLOW_CTRL_RIF_CH_R3         (FLOW_CTRL_RIF_CH_R2 ^ FLOW_STEP_RIF_CH_R3)

#define FLOW_CTRL_MPU_I_CH_R0       (FLOW_CTRL_RIF_CH_R3 ^ FLOW_STEP_MPU_I_CH_R0)
#define FLOW_CTRL_MPU_I_CH_R1       (FLOW_CTRL_MPU_I_CH_R0 ^ FLOW_STEP_MPU_I_CH_R1)
#define FLOW_CTRL_MPU_I_CH_R2       (FLOW_CTRL_MPU_I_CH_R1 ^ FLOW_STEP_MPU_I_CH_R2)
#define FLOW_CTRL_MPU_I_CH_R3       (FLOW_CTRL_MPU_I_CH_R2 ^ FLOW_STEP_MPU_I_CH_R3)
#define FLOW_CTRL_MPU_I_CH_R4       (FLOW_CTRL_MPU_I_CH_R3 ^ FLOW_STEP_MPU_I_CH_R4)
#define FLOW_CTRL_MPU_I_CH_R5       (FLOW_CTRL_MPU_I_CH_R4 ^ FLOW_STEP_MPU_I_CH_R5)
#define FLOW_CTRL_MPU_I_CH_R6       (FLOW_CTRL_MPU_I_CH_R5 ^ FLOW_STEP_MPU_I_CH_R6)
#define FLOW_CTRL_MPU_I_CH_R7       (FLOW_CTRL_MPU_I_CH_R6 ^ FLOW_STEP_MPU_I_CH_R7)
#define FLOW_CTRL_MPU_I_CH_R8       (FLOW_CTRL_MPU_I_CH_R7 ^ FLOW_STEP_MPU_I_CH_R8)
#define FLOW_CTRL_MPU_I_CH_R9       (FLOW_CTRL_MPU_I_CH_R8 ^ FLOW_STEP_MPU_I_CH_R9)
#define FLOW_CTRL_MPU_I_CH_R10      (FLOW_CTRL_MPU_I_CH_R9 ^ FLOW_STEP_MPU_I_CH_R10)
#define FLOW_CTRL_MPU_S_I_CH        (FLOW_CTRL_MPU_I_CH_R10 ^ FLOW_STEP_MPU_S_I_CH)

#define FLOW_CTRL_MCE1_CH_R1             (FLOW_CTRL_MPU_S_I_CH ^ FLOW_STEP_MCE1_CH_R1)
#define FLOW_CTRL_MCE1_CH_R2             (FLOW_CTRL_MCE1_CH_R1 ^ FLOW_STEP_MCE1_CH_R2)
#define FLOW_CTRL_MCE1_CH_R3             (FLOW_CTRL_MCE1_CH_R2 ^ FLOW_STEP_MCE1_CH_R3)

#define FLOW_CTRL_MCE1_AES_CTX_CH        (FLOW_CTRL_MCE1_CH_R3 ^ FLOW_STEP_MCE1_AES_CTX_CH)
#define FLOW_CTRL_MCE1_AES_CFG_CH        (FLOW_CTRL_MCE1_AES_CTX_CH ^ FLOW_STEP_MCE1_AES_CFG_CH)
#define FLOW_CTRL_MCE1_AES_KEY_LOCK_CH   (FLOW_CTRL_MCE1_AES_CFG_CH ^ FLOW_STEP_MCE1_AES_KEY_LOCK_CH)
#define FLOW_CTRL_MCE1_AES_CFG_LOCK_CH   (FLOW_CTRL_MCE1_AES_KEY_LOCK_CH ^ FLOW_STEP_MCE1_AES_CFG_LOCK_CH)

#define FLOW_CTRL_MCE2_CH_R1             (FLOW_CTRL_MCE1_AES_CFG_LOCK_CH ^ FLOW_STEP_MCE2_CH_R1)

#define FLOW_CTRL_MCE2_AES_CTX_CH        (FLOW_CTRL_MCE2_CH_R1 ^ FLOW_STEP_MCE2_AES_CTX_CH)
#define FLOW_CTRL_MCE2_AES_CFG_CH        (FLOW_CTRL_MCE2_AES_CTX_CH ^ FLOW_STEP_MCE2_AES_CFG_CH)

#define FLOW_CTRL_MCE2_AES_KEY_LOCK_CH   (FLOW_CTRL_MCE2_AES_CFG_CH ^ FLOW_STEP_MCE2_AES_KEY_LOCK_CH)
#define FLOW_CTRL_MCE2_AES_CFG_LOCK_CH   (FLOW_CTRL_MCE2_AES_KEY_LOCK_CH ^ FLOW_STEP_MCE2_AES_CFG_LOCK_CH)
#define FLOW_CTRL_MCE2_AES_LOCK_CH       (FLOW_CTRL_MCE2_AES_CFG_LOCK_CH ^ FLOW_STEP_MCE2_AES_LOCK_CH)

#define FLOW_CTRL_TAMP_INT_CH       (FLOW_CTRL_MCE2_AES_LOCK_CH ^ FLOW_STEP_TAMP_INT_CH)
#define FLOW_CTRL_TAMP_SEC_CH       (FLOW_CTRL_TAMP_INT_CH ^ FLOW_STEP_TAMP_SEC_CH)
#define FLOW_CTRL_TAMP_PRIV_CH      (FLOW_CTRL_TAMP_SEC_CH ^ FLOW_STEP_TAMP_PRIV_CH)
#define FLOW_CTRL_TAMP_CFG_CH       (FLOW_CTRL_TAMP_PRIV_CH ^ FLOW_STEP_TAMP_CFG_CH)

#define FLOW_CTRL_STAGE_2            FLOW_CTRL_TAMP_CFG_CH

/* Flow control Stage 3 Appli */
#define FLOW_CTRL_MPU_S_A_EN_R3     (FLOW_CTRL_STAGE_2 ^ FLOW_STEP_MPU_S_A_EN_R3)
#define FLOW_CTRL_MPU_S_A_EN_R      (FLOW_CTRL_MPU_S_A_EN_R3 ^ FLOW_STEP_MPU_S_A_EN_R)
#define FLOW_CTRL_MPU_S_A_EN_R8     (FLOW_CTRL_MPU_S_A_EN_R ^ FLOW_STEP_MPU_S_A_EN_R8)
#define FLOW_CTRL_MPU_S_A_EN_R11    (FLOW_CTRL_MPU_S_A_EN_R8 ^ FLOW_STEP_MPU_S_A_EN_R11)
#define FLOW_CTRL_MPU_S_A_EN_R12    (FLOW_CTRL_MPU_S_A_EN_R11 ^ FLOW_STEP_MPU_S_A_EN_R12)
#define FLOW_CTRL_MPU_NS_A_EN_R0    (FLOW_CTRL_MPU_S_A_EN_R12 ^ FLOW_STEP_MPU_NS_A_EN_R0)
#define FLOW_CTRL_MPU_NS_A_EN_R1    (FLOW_CTRL_MPU_NS_A_EN_R0 ^ FLOW_STEP_MPU_NS_A_EN_R1)
#define FLOW_CTRL_MPU_NS_A_EN_R2    (FLOW_CTRL_MPU_NS_A_EN_R1 ^ FLOW_STEP_MPU_NS_A_EN_R2)
#define FLOW_CTRL_MPU_NS_A_EN_R3    (FLOW_CTRL_MPU_NS_A_EN_R2 ^ FLOW_STEP_MPU_NS_A_EN_R3)

#define FLOW_CTRL_MCE1_AES_LOCK     (FLOW_CTRL_MPU_S_A_EN_R12 ^ FLOW_STEP_MCE1_AES_LOCK)

#define FLOW_CTRL_OTP_NVCNT_EN      (FLOW_CTRL_MCE1_AES_LOCK ^ FLOW_STEP_OTP_NVCNT_EN)
#define FLOW_CTRL_OTP_DA_EN         (FLOW_CTRL_OTP_NVCNT_EN ^ FLOW_STEP_OTP_DA_EN)
#define FLOW_CTRL_OTP_S_PBK_EN      (FLOW_CTRL_OTP_DA_EN ^ FLOW_STEP_OTP_S_PBK_EN)
#define FLOW_CTRL_OTP_NS_PBK_EN     (FLOW_CTRL_OTP_S_PBK_EN ^ FLOW_STEP_OTP_NS_PBK_EN)
#define FLOW_CTRL_OTP_ENCK_EN       (FLOW_CTRL_OTP_NS_PBK_EN ^ FLOW_STEP_OTP_ENCK_EN)

#define FLOW_CTRL_HDPL_INC          (FLOW_CTRL_OTP_ENCK_EN ^ FLOW_STEP_HDPL_INC)

#define FLOW_CTRL_STAGE_3            FLOW_CTRL_HDPL_INC

#define FLOW_CTRL_MPU_S_A_CH_R3     (FLOW_CTRL_STAGE_3 ^ FLOW_STEP_MPU_S_A_CH_R3)
#define FLOW_CTRL_MPU_S_A_CH_R      (FLOW_CTRL_MPU_S_A_CH_R3 ^ FLOW_STEP_MPU_S_A_CH_R)
#define FLOW_CTRL_MPU_S_A_CH_R8     (FLOW_CTRL_MPU_S_A_CH_R ^ FLOW_STEP_MPU_S_A_CH_R8)
#define FLOW_CTRL_MPU_S_A_CH_R11    (FLOW_CTRL_MPU_S_A_CH_R8 ^ FLOW_STEP_MPU_S_A_CH_R11)
#define FLOW_CTRL_MPU_S_A_CH_R12    (FLOW_CTRL_MPU_S_A_CH_R11 ^ FLOW_STEP_MPU_S_A_CH_R12)
#define FLOW_CTRL_MPU_NS_A_CH_R0    (FLOW_CTRL_MPU_S_A_CH_R12 ^ FLOW_STEP_MPU_NS_A_CH_R0)
#define FLOW_CTRL_MPU_NS_A_CH_R1    (FLOW_CTRL_MPU_NS_A_CH_R0 ^ FLOW_STEP_MPU_NS_A_CH_R1)
#define FLOW_CTRL_MPU_NS_A_CH_R2    (FLOW_CTRL_MPU_NS_A_CH_R1 ^ FLOW_STEP_MPU_NS_A_CH_R2)
#define FLOW_CTRL_MPU_NS_A_CH_R3    (FLOW_CTRL_MPU_NS_A_CH_R2 ^ FLOW_STEP_MPU_NS_A_CH_R3)

#define FLOW_CTRL_MCE1_AES_LOCK_CH  (FLOW_CTRL_MPU_S_A_CH_R12 ^ FLOW_STEP_MCE1_AES_LOCK_CH)

#define FLOW_CTRL_OTP_NVCNT_CH      (FLOW_CTRL_MCE1_AES_LOCK_CH ^ FLOW_STEP_OTP_NVCNT_CH)
#define FLOW_CTRL_OTP_DA_CH         (FLOW_CTRL_OTP_NVCNT_CH ^ FLOW_STEP_OTP_DA_CH)
#define FLOW_CTRL_OTP_S_PBK_CH      (FLOW_CTRL_OTP_DA_CH ^ FLOW_STEP_OTP_S_PBK_CH)
#define FLOW_CTRL_OTP_NS_PBK_CH     (FLOW_CTRL_OTP_S_PBK_CH ^ FLOW_STEP_OTP_NS_PBK_CH)
#define FLOW_CTRL_OTP_ENCK_CH       (FLOW_CTRL_OTP_NS_PBK_CH ^ FLOW_STEP_OTP_ENCK_CH)

#define FLOW_CTRL_HDPL_CH1          (FLOW_CTRL_OTP_ENCK_CH ^ FLOW_STEP_HDPL_CH1)
#define FLOW_CTRL_HDPL_CH2          (FLOW_CTRL_HDPL_CH1 ^ FLOW_STEP_HDPL_CH2)

#define FLOW_CTRL_STAGE_4            FLOW_CTRL_HDPL_CH2

/* External variables --------------------------------------------------------*/
/**
  *  Flow control protection values
  *  Flow control stage
  */
extern volatile uint32_t uFlowProtectValue;
extern volatile uint32_t uFlowStage;

/* Exported macros -----------------------------------------------------------*/
/** Control with STEP operation :
  * (uFlowValue XOR STEP_VALUE) should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_STEP(C,B,A) \
  do{ \
    (C) ^= (B);\
    if ((C) != (A))\
    { \
      Error_Handler();\
    } \
  }while(0)
#else
#define FLOW_CONTROL_STEP(C,B,A) ((void)0)
#endif

/** Control without STEP operation :
  * uFlowValue should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_CHECK(B,A) \
  do{ \
    if ((B) != (A))\
    { \
      Error_Handler();\
    } \
  }while(0)
#else
#define FLOW_CONTROL_CHECK(B,A) ((void)0)
#endif

/** Control flow initialization
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_INIT(B,A) \
  do{ \
    (B) = (A);\
  }while(0)
#else
#define FLOW_CONTROL_INIT(B,A) ((void)0)
#endif

/** STEP update only :
  * (uFlowValue XOR STEP_VALUE)
  */
#if defined(FLOW_CONTROL)
#define FLOW_STEP(B,A) \
  do{ \
    (B) ^= (A);\
  }while(0)
#else
#define FLOW_STEP(B,A) ((void)0)
#endif

/* Exported functions ------------------------------------------------------- */

/** @defgroup BOOT_FLOWCONTROL_Exported_Functions Exported Functions
  * @{
  */


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_FLOWCONTROL_H */
