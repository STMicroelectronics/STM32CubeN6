/**
  ******************************************************************************
  * @file    stm32_extmem_type.h
  * @author  MCD Application Team
  * @brief   This file contains the external memory type definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EXTMEM_TYPE_H_
#define __STM32_EXTMEM_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup EXTMEM
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_SAL_XSPI == 1
#include "sal/stm32_sal_xspi_type.h"
#endif /* EXTMEM_SAL_XSPI */

#if EXTMEM_SAL_SD == 1
#include "sal/stm32_sal_sd_type.h"
#endif /* EXTMEM_SAL_SD */

#if EXTMEM_SAL_MMC == 1
#include "sal/stm32_sal_mmc_type.h"
#endif /* EXTMEM_SAL_MMC */

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "nor_sfdp/stm32_sfdp_driver_type.h"
#endif /* DRIVER_SFDP_ENABLED */

#if EXTMEM_DRIVER_SDCARD == 1
#include "sdcard/stm32_sdcard_driver_type.h"
#endif /* DRIVER_SDCARD_ENABLED */

#if EXTMEM_DRIVER_PSRAM == 1
#include "psram/stm32_psram_driver_type.h"
#endif /* DRIVER_PSRAM_ENABLED */

#if EXTMEM_DRIVER_CUSTOM == 1
#include "custom/stm32_custom_driver_type.h"
#endif /* DRIVER_USER_ENABLED */

#if EXTMEM_DRIVER_USER == 1
#include "user/stm32_user_driver_type.h"
#endif /* DRIVER_USER_ENABLED */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup EXTMEM_Exported_Macros External Memory Exported Macros
  * @{
  */
/**
  * @brief Macro to get the minimum value
  */
#define EXTMEM_MIN(_A_,_B_)    ((_A_)>(_B_))?(_B_):(_A_);
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup EXTMEM_Exported_Constants External Memory Exported Constants
  * @{
  */

/** @defgroup EXTMEM_Manufacturer_IDs EXTMEM JEDEC Manufacturer IDs
  * @{
  */
#define EXTMEM_MANUFACTURER_MACRONIX    (0xC2U)   /*!< JEDEC ID for Macronix */
#define EXTMEM_MANUFACTURER_GIGADEVICE  (0xC8U)   /*!< JEDEC ID for GigaDevice */
#define EXTMEM_MANUFACTURER_ISSI        (0x9DU)   /*!< JEDEC ID for ISSI */
#define EXTMEM_MANUFACTURER_WINBOND     (0xEFU)   /*!< JEDEC ID for Winbond */
/**
  * @}
  */

/** @defgroup EXTMEM_Common_JEDEC_Commands EXTMEM Common JEDEC commands identifiers
  * @{
  */
#define EXTMEM_READ_JEDEC_ID_SPI_COMMAND                0x9FU
#define EXTMEM_READ_JEDEC_ID_SIZE                       0x04U

#define EXTMEM_READ_SFDP_COMMAND                        0x5AU
#define EXTMEM_READ_SFDP_NB_DUMMY_CYCLES_DEFAULT        8U
#define EXTMEM_READ_SFDP_NB_DUMMY_CYCLES_OPI            20U

#define EXTMEM_FAST_READ_NB_DUMMY_CYCLES_DEFAULT        8U

/**
  * @}
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup EXTMEM_Exported_TypeDefinitions External Memory Exported Type Definitions
  * @{
  */

/**
  * @brief Definition of the memory
  */
typedef struct
{
  EXTMEM_TypeTypeDef MemType;               /*!< Specifies the memory driver type */
  void *Handle;                             /*!< Pointer to HAL handle */
  EXTMEM_LinkConfig_TypeDef ConfigType;     /*!< Specifies the physical link configuration */
  union
  {
#if EXTMEM_DRIVER_NOR_SFDP == 1
    EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef  NorSfdpObject;  /*!< Object for NOR SFDP driver */
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
    EXTMEM_DRIVER_SDCARD_ObjectTypeDef    SdCardObject;   /*!< Object for SD Card driver */
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
    EXTMEM_DRIVER_PSRAM_ObjectTypeDef     PsramObject;    /*!< Object for PSRAM driver */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
    EXTMEM_DRIVER_CUSTOM_ObjectTypeDef    CustomObject;   /*!< Object for Custom driver */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
    EXTMEM_DRIVER_USER_ObjectTypeDef      UserObject;     /*!< Object for User driver */
#endif /* EXTMEM_DRIVER_USER == 1 */
  };
} EXTMEM_DefinitionTypeDef;

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_TYPE_H_ */
