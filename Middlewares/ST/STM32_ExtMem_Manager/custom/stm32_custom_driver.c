/**
  ******************************************************************************
  * @file    stm32_custom_driver.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for custom support
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"

#if defined(EXTMEM_DRIVER_CUSTOM) && (EXTMEM_DRIVER_CUSTOM == 1)
#include "stm32_custom_driver_api.h"
#include "stm32_custom_driver_type.h"

#if EXTMEM_DRIVER_CUSTOM_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /*EXTMEM_DRIVER_CUSTOM_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)*/

/** @defgroup CUSTOM CUSTOM driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup CUSTOM_Private_Macro Private Macro
  * @{
  */

#if EXTMEM_DRIVER_CUSTOM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
/**
  * @brief Debug macro for a string
  */
#define DEBUG_STR(_STR_)                            \
  {                                                 \
    EXTMEM_MACRO_DEBUG(_STR_);                      \
    EXTMEM_MACRO_DEBUG("\n\r");                     \
  }
/**
  * @brief Debug macro for a string and a value (hex)
  */
#define DEBUG_STR_HEX(_STR_,_HEX_ )                 \
  {                                                 \
    char str[10];                                   \
    (void)sprintf(str, "0x%x", (unsigned int)_HEX_);\
    EXTMEM_MACRO_DEBUG("\t ");                      \
    EXTMEM_MACRO_DEBUG(_STR_);                      \
    EXTMEM_MACRO_DEBUG(str);                        \
    EXTMEM_MACRO_DEBUG("\n\r");                     \
  }
/**
  * @brief Debug macro for a string and a value (int)
  */
#define DEBUG_STR_INT(_STR_,_INT_ )                 \
  {                                                 \
    char str[10];                                   \
    (void)sprintf(str, "%u", (unsigned int)_INT_);  \
    EXTMEM_MACRO_DEBUG("\t ");                      \
    EXTMEM_MACRO_DEBUG(_STR_);                      \
    EXTMEM_MACRO_DEBUG(str);                        \
    EXTMEM_MACRO_DEBUG("\n\r");                     \
  }

#define DEBUG_API()       EXTMEM_MACRO_DEBUG("\n\r CUSTOM API: ");
#define DEBUG_INTERNAL_BLOCK()   \
  EXTMEM_MACRO_DEBUG("\t =======================================\n\r ");
#else

/**
  * @brief Debug macro
  */
#define DEBUG_STR_HEX(_STR_,_HEX_ )
#define DEBUG_STR_INT(_STR_,_INT_ )
#define DEBUG_STR(_STR_)
#define DEBUG_API()
#define DEBUG_INTERNAL_BLOCK()

#endif /* EXTMEM_DRIVER_CUSTOM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG) */

/**
  * @brief MIN macro
  */
#define MIN(_A_,_B_)   ((_A_) > (_B_))?(_B_):(_A_);

/**
  * @}
  */


/* Private typedefs ---------------------------------------------------------*/

/* Status values returned by internal Custom XSPI driver functions.
   Indicates success (OK) or failure (KO) of an operation.  */
typedef enum
{
  CUSTOM_XSPI_OK  =  0,   /*!< Operation successful */
  CUSTOM_XSPI_KO  = -1,   /*!< Operation failed */
} CUSTOM_XSPI_StatusTypeDef;

/* Selection of Read Register command from configuration
 * (depending on sequence, i.e. if Read Register is done prior or after Write operation)
 */
typedef enum
{
  CUSTOM_XSPI_READ_TYPE_INITIAL = 0,
  CUSTOM_XSPI_READ_TYPE_AFTER_WRITE,
} CUSTOM_XSPI_ReadTypeTypeDef;

/* Definition of RESET Instructions */
#define CUSTOM_RESET_INS_66_8BITS           0x66U
#define CUSTOM_RESET_INS_99_8BITS           0x99U
#define CUSTOM_RESET_INS_FF_8BITS           0xFFU

#define CUSTOM_RESET_INS_6699_16BITS        0x6699U
#define CUSTOM_RESET_INS_9966_16BITS        0x9966U
#define CUSTOM_RESET_INS_6666_16BITS        0x6666U
#define CUSTOM_RESET_INS_9999_16BITS        0x9999U

/* Definition of the clock value above which rounding is performed */
#define CUSTOM_CLOCK_VALUE_FOR_ROUNDING     50000000U
#define CUSTOM_CLOCK_VALUE_INCREMENT        1000000U

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_Init(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_DeInit(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetClock(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                      uint32_t ClockIn, uint32_t ClockRequested);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_ReadRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                          OptionalRegisterConfigTypeDef *RegisterConfig,
                                                          CUSTOM_XSPI_ReadTypeTypeDef ReadType);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_WriteRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                           OptionalRegisterConfigTypeDef *RegisterConfig);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_Send_Instruction(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                              uint32_t Instruction);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_ResetMemory(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);

/* Prepare XSPI Command depending on memory configuration */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetCommand(MemoryBaseConfigTypeDef *ConfigObject,
                                                        XSPI_RegularCmdTypeDef *sCommand);
/* Save, restore current memory configuration command */
static void CUSTOM_XSPI_SaveCommand(XSPI_RegularCmdTypeDef *sCommand, uint32_t *CommandToSave);
static void CUSTOM_XSPI_RestoreCommand(XSPI_RegularCmdTypeDef *sCommand, uint32_t *CommandSaved);

static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetWriteCommand(MemoryBaseConfigTypeDef *ConfigObject,
                                                             XSPI_RegularCmdTypeDef *sCommand);
/* Used for NorFlash only */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetWriteEnable(XSPI_HandleTypeDef *hxspi, XSPI_RegularCmdTypeDef *sCommand,
                                                            NorFlashBasConfigTypeDef *NorConfig);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_GetWriteInProgress(XSPI_HandleTypeDef *hxspi,
                                                                XSPI_RegularCmdTypeDef *sCommand,
                                                                NorFlashBasConfigTypeDef *NorConfig,
                                                                uint32_t Timeout);

static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_GetJedecID(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);

static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_SetCommand(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_ReadRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                   OptionalRegisterConfigTypeDef *RegisterConfig);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_WriteRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                    OptionalRegisterConfigTypeDef *RegisterConfig);
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_GetDeviceID(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Initializes the CUSTOM driver, configures the XSPI peripheral, and sets up the memory device
  *        according to data provided in Custom driver object (configuration data).
  * @note
  * The initialization sequence as described in Custom driver object could include the following steps:
  *   - Send first command with basic configuration (e.g. 1S1S1S)
  *   - Send (n) configuration register commands and read back each one:
  *     - Check Write In Progress (WIP)
  *     - Write Enable (WE)
  *     - Check Write Enable Latch (WEL)
  *     - Write configuration register
  *     - Read back register to verify
  *   - Initialize clock frequency to maximum
  *   - Send second command with normal configuration (e.g. 4D4D4D)
  *   - At this stage, full configuration and frequency are applied
  * @param hxspi Pointer to the XSPI peripheral handle.
  * @param ClockInput Input clock frequency for the XSPI peripheral.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Init(XSPI_HandleTypeDef *hxspi,  uint32_t ClockInput,
                                                             EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  uint32_t iConfig = 0;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_Init");

  /* Handle */
  CustomObject->Private.Handle = (XSPI_HandleTypeDef *)hxspi;

  /* Init XSPI peripheral */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_Init(CustomObject))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_INIT;
    goto error;
  }

  /* Initialize memory clock frequency to its startup (default) configuration */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetClock(CustomObject, ClockInput, CustomObject->StartupConfig.Frequency))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_CLOCK_INIT;
    goto error;
  }

  if (IS_NOT_MEMORY_HYPERBUS(CustomObject->MemType))
  {
    /* Init Command details, based on Startup configuration */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }

    /* Implementation of reset procedure if specified in configuration file */
    if (CustomObject->ResetMethod != EXTMEM_CUSTOM_RESET_METHOD_UNDEFINED)
    {
      /* Execute Reset procedure */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_ResetMemory(CustomObject))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_RESET_MEMORY;
        goto error;
      }
    }

    /* Initialize Current NorFlash config in private structure */
    if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
    {
      CustomObject->Private.CurrentFlashConfig = &CustomObject->NorFlashConfig.Startup;

      /* Get JedecID in case of NorFlash memory */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetJedecID(CustomObject))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_JEDEC_ID;
        goto error;
      }
    }
  }
  else
  {
    if (IS_MEMORY_PSRAM(CustomObject->MemType))
    {
      /* Init Command details, based on Startup configuration */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_SetCommand(CustomObject))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        goto error;
      }

      /* Get Device ID in case of HyperRam memory */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_GetDeviceID(CustomObject))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_DEVICEC_ID;
        goto error;
      }
    }
  }

  /* Apply optional register configuration steps if described in configuration file */
  while (iConfig < (CustomObject->NbRegisterConfig))
  {
    DEBUG_INTERNAL_BLOCK();
    DEBUG_STR_INT("[CONFIG STEP] ", iConfig);

    /* Send Read/Write configuration commands to registers inside memory */
    if (IS_CFGSTEP_REG_ACCESS_TYPE(CustomObject->RegisterConfig[iConfig].ConfigStepType))
    {
      /* Send Read Register command to memory */
      if (IS_CFGSTEP_REG_R_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_ReadRegister(CustomObject, &CustomObject->RegisterConfig[iConfig],
                                                       CUSTOM_XSPI_READ_TYPE_INITIAL))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER;
          goto error;
        }
      }

      /* Send Write Register command to memory */
      if (IS_CFGSTEP_REG_W_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        /* In case of Nor Flash device, check WIP flag in memory status register
           and send Write Enable command */
        if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
        {
          /* WIP Command*/
          if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(CustomObject->Private.Handle,
                                                               &CustomObject->Private.Commandbase,
                                                               CustomObject->Private.CurrentFlashConfig,
                                                               CUSTOM_XSPI_TIMEOUT_VALUE))
          {
            retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
            goto error;
          }

          /* WE Command*/
          if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteEnable(CustomObject->Private.Handle,
                                                           &CustomObject->Private.Commandbase,
                                                           CustomObject->Private.CurrentFlashConfig))
          {
            retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE;
            goto error;
          }
        }

        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_WriteRegister(CustomObject, &CustomObject->RegisterConfig[iConfig]))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_REGISTER;
          goto error;
        }
      }

      /* Send Read Register command to memory for verification */
      if (IS_CFGSTEP_REG_RAW_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
        {
          /* WIP Command : ensure previous command has been handled */
          if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(CustomObject->Private.Handle,
                                                               &CustomObject->Private.Commandbase,
                                                               CustomObject->Private.CurrentFlashConfig,
                                                               CUSTOM_XSPI_TIMEOUT_VALUE))
          {
            retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
            goto error;
          }
        }

        /* Read after Write operation (to check value) : only if config type is RW or RWR */
        if (IS_CFGSTEP_REG_RAW_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
        {
          if (CUSTOM_XSPI_OK != CUSTOM_XSPI_ReadRegister(CustomObject, &CustomObject->RegisterConfig[iConfig],
                                                         CUSTOM_XSPI_READ_TYPE_AFTER_WRITE))
          {
            retr = EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER;
            goto error;
          }

          /* Write verification */
          /*
          if(tmpReg != CustomObject->Private.ReadRegValue)
          {
            while(1);
          }
          */
        }
      }
    }
    /* Send single instruction to memory */
    else if (IS_CFGSTEP_INST_TYPE(CustomObject->RegisterConfig[iConfig].ConfigStepType))
    {
      /* Basic command case */
      if (IS_CFGSTEP_INS_BASIC(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CUSTOM_XSPI_OK !=
            CUSTOM_XSPI_Send_Instruction(CustomObject,
                                         CustomObject->RegisterConfig[iConfig].INSConfigStep.Instruction))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SEND_INSTRUCTION;
          goto error;
        }
      }
    }
    /* Execute user callback if required */
    else if (IS_CFGSTEP_USER_CALLBACK_TYPE(CustomObject->RegisterConfig[iConfig].ConfigStepType))
    {
      if (CustomObject->RegisterConfig[iConfig].CBConfigStep.Callback == NULL)
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_USER_CALLBACK;
        goto error;
      }

      retr = (EXTMEM_DRIVER_CUSTOM_StatusTypeDef)CustomObject->RegisterConfig[iConfig].CBConfigStep.Callback(
               CustomObject->RegisterConfig[iConfig].CBConfigStep.UserCtx,
               CustomObject->RegisterConfig[iConfig].CBConfigStep.Arg0,
               CustomObject->RegisterConfig[iConfig].CBConfigStep.Arg1);
      if (retr != EXTMEM_DRV_CUSTOM_OK)
      {
        goto error;
      }
    }
    /* HyperBus configuration register step */
    else if (IS_CFGSTEP_HYPERBUS_TYPE(CustomObject->RegisterConfig[iConfig].ConfigStepType))
    {
      /* Send Read Register command to memory */
      if (IS_CFGSTEP_REG_R_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_ReadRegister(CustomObject, &CustomObject->RegisterConfig[iConfig]))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER;
          goto error;
        }
      }

      /* Send Write Register command to memory */
      if (IS_CFGSTEP_REG_W_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_WriteRegister(CustomObject, &CustomObject->RegisterConfig[iConfig]))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_REGISTER;
          goto error;
        }
      }

      /* Send Read Register command to memory for verification */
      if (IS_CFGSTEP_REG_RAW_ACCESS(CustomObject->RegisterConfig[iConfig].ConfigStepType))
      {
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_ReadRegister(CustomObject, &CustomObject->RegisterConfig[iConfig]))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER;
          goto error;
        }
      }
    }
    /* Execute new Optional configuration if required */
    else if (IS_CFGSTEP_EXEC_OPT_CFG_TYPE(CustomObject->RegisterConfig[iConfig].ConfigStepType))
    {
      /* Initialize memory clock frequency to its final (optional) configuration */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetClock(CustomObject, ClockInput, CustomObject->OptionalConfig.Frequency))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_CLOCK_INIT;
        goto error;
      }

      if (IS_NOT_MEMORY_HYPERBUS(CustomObject->MemType))
      {
        /* Init Command with optional config */
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig,
                                                     &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
          goto error;
        }

        if (CustomObject->NorFlashConfig.OptionalConfigEnable)
        {
          /* Initialize Current NorFlash config */
          CustomObject->Private.CurrentFlashConfig = &CustomObject->NorFlashConfig.Optional;
        }
      }
      else
      {
        /* Init Command details, based on Startup configuration */
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_HYPERBUS_SetCommand(CustomObject))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
          goto error;
        }
      }
    }
    else
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_CONFIG_STEP;
      goto error;
    }

    /* Go to next configuration Step */
    iConfig++;
  }

  DEBUG_INTERNAL_BLOCK();

error:
  return retr;
}

/**
  * @brief Deinitializes the XSPI peripheral and the associated CUSTOM driver.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_DeInit(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_DeInit");

  /* Deinitializes the XSPI peripheral and associated CUSTOM driver */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_DeInit(CustomObject))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_DEINIT;
  }

  return retr;
}

/**
  * @brief Reads data from the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param Address Address to read from in the memory.
  * @param Data Pointer to the data buffer to store the read data.
  * @param Size Size of data to read (in bytes).
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Read(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                             uint32_t Address, uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_RegularCmdTypeDef *sCommand   = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi          = CustomObject->Private.Handle;
  MemoryBaseConfigTypeDef *MemConfig;
  NorFlashBasConfigTypeDef *NorConfig;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_Read");

  /* Verify if Memory is a NorFlash */
  if (IS_NOT_MEMORY_FLASH(CustomObject->MemType))
  {
    return EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE;
  }

  /* Check which memory configuration is to be used */
  if (CustomObject->OptionalConfig.Frequency != 0U)
  {
    MemConfig = &CustomObject->OptionalConfig;
  }
  else
  {
    MemConfig = &CustomObject->StartupConfig;
  }

  /* Set specific NorFlash memory configuration and check WIP */
  if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
  {
    /* Init NorFlash configuration */
    if (CustomObject->NorFlashConfig.OptionalConfigEnable)
    {
      NorConfig = &CustomObject->NorFlashConfig.Optional;

      /* Init Command */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(MemConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }
    else
    {
      NorConfig = &CustomObject->NorFlashConfig.Startup;

      /* Init Command */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(MemConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }

    /* WIP Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, CUSTOM_XSPI_TIMEOUT_VALUE))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
      goto error;
    }
  }
  else
  {
    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(MemConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
    }
  }

  /* Read the data */
  sCommand->OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->Instruction        = MemConfig->CommandRead;
  sCommand->DummyCycles        = MemConfig->DummyCycleRead;
  sCommand->Address            = Address;
  sCommand->DataLength         = Size;

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
    goto error;
  }

  if (HAL_XSPI_Receive(hxspi, Data, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
    goto error;
  }
error:
  return retr;
}

/**
  * @brief Writes data to the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param Address Address to write to in the memory.
  * @param Data Pointer to the data buffer to be written.
  * @param Size Size of data to be written (in bytes).
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Write(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                              uint32_t Address, const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_RegularCmdTypeDef *sCommand        = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi               = CustomObject->Private.Handle;
  uint32_t size_write;
  uint32_t local_size                     = Size;
  uint32_t local_Address                  = Address;
  uint32_t local_Data                     = (uint32_t)Data;
  uint32_t misalignment                   = 0u;
  MemoryBaseConfigTypeDef *MemConfig;
  NorFlashBasConfigTypeDef *NorConfig;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_Write");

  /*
   *  Here need to separate in 2 different functions for write operation
   *  - NorFlash_Write
   *  - HyperFlash_Write
   */

  /* Verify if Memory is a NorFlash */
  if (IS_NOT_MEMORY_FLASH(CustomObject->MemType))
  {
    return EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE;
  }

  /* Check which memory configuration is to be used */
  if (CustomObject->OptionalConfig.Frequency != 0U)
  {
    MemConfig = &CustomObject->OptionalConfig;
  }
  else
  {
    MemConfig = &CustomObject->StartupConfig;
  }

  /* Check if write address is on a page frontier */
  if (0u != (local_Address % CustomObject->NorFlashConfig.PageSize))
  {
    misalignment = 1u;
  }

  /* Loop until write operation is completed */
  while (local_size != 0u)
  {
    /* Compute size to be written according to alignment status */
    if (misalignment == 1u)
    {
      /* PAGE_SIZE or Boundary */
      size_write = CustomObject->NorFlashConfig.PageSize - (local_Address % CustomObject->NorFlashConfig.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      /* PAGE_SIZE or Boundary */
      size_write = MIN(local_size, CustomObject->NorFlashConfig.PageSize);
    }

    if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
    {
      /* Check which memory configuration is to be used */
      if (CustomObject->NorFlashConfig.OptionalConfigEnable)
      {
        NorConfig = &CustomObject->NorFlashConfig.Optional;

        /* Init Command */
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }
      else
      {
        NorConfig = &CustomObject->NorFlashConfig.Startup;

        /* Init Command */
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }

      /* WIP Command */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, CUSTOM_XSPI_TIMEOUT_VALUE))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
        goto error;
      }

      /* WE Command */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteEnable(hxspi, sCommand,  NorConfig))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE;
        goto error;
      }
    }

    /* Init write Command */
    if (IS_ACCESS_MODE_TYPE(MemConfig->OptWriteAccessMode))
    {
      /* Use optional write access mode to build command if needed */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteCommand(MemConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }
    else
    {
      /* Set global memory configuration */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(MemConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }

    /* Write the data */
    sCommand->OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand->Instruction        = MemConfig->CommandWrite;
    sCommand->DummyCycles        = MemConfig->DummyCycleWrite;
    sCommand->Address            = local_Address;
    sCommand->DataLength         = size_write;

    if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
      goto error;
    }

    if (HAL_XSPI_Transmit(hxspi, (uint8_t *)local_Data, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
      goto error;
    }

    /* Update counters */
    local_size = local_size - size_write;
    local_Address = local_Address + size_write;
    local_Data = local_Data + size_write;
  }

  /* In case of Nor Flash, check for Write operation completion before exiting function */
  if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
  {
    /* Check which memory configuration is to be used */
    if (CustomObject->NorFlashConfig.OptionalConfigEnable)
    {
      /* Init Command */
      NorConfig = &CustomObject->NorFlashConfig.Optional;
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }
    else
    {
      /* Init Command */
      NorConfig = &CustomObject->NorFlashConfig.Startup;
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }

    /* WIP Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(CustomObject->Private.Handle, sCommand,  NorConfig,
                                                         CUSTOM_XSPI_TIMEOUT_VALUE))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
      goto error;
    }
  }

error:
  return retr;
}

/**
  * @brief Writes data to the memory in mapped mode.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param Address Memory address to write in mapped mode.
  * @param Data Pointer to the data buffer to be written.
  * @param Size Size of data to be written (in bytes).
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_WriteInMappedMode(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef
                                                                          *CustomObject, uint32_t Address,
                                                                          const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_RegularCmdTypeDef *sCommand        = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi               = CustomObject->Private.Handle;
  uint32_t size_write;
  uint32_t local_size                     = Size;
  uint32_t local_Address                  = Address;
  const uint8_t *local_Data               = Data;
  uint32_t misalignment                   = 0u;
  uint32_t size;
  NorFlashBasConfigTypeDef *NorConfig;
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  uint8_t dcache_status = 1;
#endif /* __DCACHE_PRESENT */
  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_WriteInMappedMode");

  /* Verify if Memory is a NorFlash */
  if (IS_NOT_MEMORY_FLASH(CustomObject->MemType))
  {
    return EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE;
  }

  /* Check if the input address is 32bit aligned */
  if (0u != (local_Address % 4u))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_ADDRESS_ALIGNMENT;
    goto error;
  }

  /* Check if write address is on a page frontier */
  if (0u != (local_Address % CustomObject->NorFlashConfig.PageSize))
  {
    misalignment = 1u;
  }

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* Check if DCache is already enabled */
  if (SCB->CCR & SCB_CCR_DC_Msk)
  {
    dcache_status = 1;
  }
  else
  {
    dcache_status = 0;
    /* Enable DCache */
    SCB_EnableDCache();
  }
#endif /* __DCACHE_PRESENT */

  while (local_size != 0u)
  {

    if (misalignment == 1u)
    {
      size_write = CustomObject->NorFlashConfig.PageSize - (local_Address % CustomObject->NorFlashConfig.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      if (local_size > CustomObject->NorFlashConfig.PageSize)
      {
        size_write = CustomObject->NorFlashConfig.PageSize;
      }
      else
      {
        size_write = local_size;
        if (0u != (local_size % sizeof(uint32_t)))
        {
          size_write = size_write + sizeof(uint32_t) - (local_size % sizeof(uint32_t));
        }
      }
    }

    if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
    {
      /* Check which memory configuration is to be used */
      if (CustomObject->NorFlashConfig.OptionalConfigEnable)
      {
        NorConfig = &CustomObject->NorFlashConfig.Optional;

        /* Init Command*/
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }
      else
      {
        NorConfig = &CustomObject->NorFlashConfig.Startup;

        /* Init Command*/
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }

      /* WIP Command*/
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
        goto error;
      }

      /* WE Command*/
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteEnable(hxspi, sCommand,  NorConfig))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE;
        goto error;
      }
    }

    /* Enter the mapped mode */
    retr = EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode(CustomObject);
    if (EXTMEM_DRV_CUSTOM_OK != retr)
    {
      goto error;
    }

    size = size_write;
    /* Execute the copy */
    EXTMEM_DRIVER_CUSTOM_MemCopy((uint32_t *)local_Address, local_Data, size_write);

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* Clean DCache starting from a 32 byte aligned address in 32 byte granularity */
    SCB_CleanDCache_by_Addr((void *)(local_Address), size_write);
#endif /* __DCACHE_PRESENT */

    /* Enter the mapped mode */
    retr = EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode(CustomObject);
    if (EXTMEM_DRV_CUSTOM_OK != retr)
    {
      goto error;
    }

    /* In case of Nor Flash, check for Write operation completion before exiting function */
    if (CustomObject->MemType == EXTMEM_CUSTOM_NOR_FLASH)
    {
      /* Check which memory configuration is to be used */
      if (CustomObject->NorFlashConfig.OptionalConfigEnable)
      {
        NorConfig = &CustomObject->NorFlashConfig.Optional;

        /* Init Command*/
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }
      else
      {
        NorConfig = &CustomObject->NorFlashConfig.Startup;

        /* Init Command*/
        if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
        {
          retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
        }
      }

      /* WIP Command*/
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(CustomObject->Private.Handle, sCommand,  NorConfig,
                                                           HAL_XSPI_TIMEOUT_DEFAULT_VALUE))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
        goto error;
      }
    }

    /* Decrement the transfer size */
    if (local_size > size)
    {
      local_size = local_size - size;
      local_Data = &local_Data[size];
      local_Address = local_Address + size;
    }
    else
    {
      local_size = 0;
    }
  }

error:

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* Disable DCache if deactivated previously */
  if (!dcache_status)
  {
    SCB_DisableDCache();
  }
#endif /* __DCACHE_PRESENT */

  return retr;
}

/**
  * @brief Erases sectors in the specified memory area (Address + Size).
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param Address Address of the first sector to erase.
  * @param Size Size of the area to erase (in bytes).
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_EraseSector(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                    uint32_t Address, uint32_t Size)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_RegularCmdTypeDef *sCommand        = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi               = CustomObject->Private.Handle;
  uint32_t local_address                  = Address;
  uint32_t local_size                     = Size;
  uint32_t sector_size                    = CustomObject->NorFlashConfig.SectorSize;
  uint32_t SavedDataMode;
  uint32_t SavedDummyCycles;
  uint32_t max_timing;
  NorFlashBasConfigTypeDef *NorConfig;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_EraseSector");

  /* Verify if Memory is a NorFlash */
  if (IS_NOT_MEMORY_FLASH(CustomObject->MemType))
  {
    return EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE;
  }

  /* Check if address is a modulo of sector size */
  if ((local_address % sector_size) != 0u)
  {
    return EXTMEM_DRV_CUSTOM_ERR_SECTOR_SIZE;
  }

  /* Save Xspi command */
  SavedDataMode                = sCommand->DataMode;
  SavedDummyCycles             = sCommand->DummyCycles;

  /* Check which memory configuration is to be used */
  if (CustomObject->NorFlashConfig.OptionalConfigEnable)
  {
    NorConfig = &CustomObject->NorFlashConfig.Optional;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }
  else
  {
    NorConfig = &CustomObject->NorFlashConfig.Startup;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }

  /* Loop until enough sectors have been erased */
  while (local_size != 0u)
  {
    /* WIP Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, CUSTOM_XSPI_TIMEOUT_VALUE))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
      goto error;
    }

    /* WE Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteEnable(hxspi, sCommand,  NorConfig))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE;
      goto error;
    }

    /* Save Xspi command */
    SavedDataMode                = sCommand->DataMode;
    SavedDummyCycles             = sCommand->DummyCycles;

    /* Prepare Sector Erase Command */
    sCommand->Instruction        = NorConfig->Cmd_EraseSector;
    sCommand->Address            = local_address;
    sCommand->DataMode           = HAL_XSPI_DATA_NONE;
    sCommand->DummyCycles        = 0;

    if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE)  != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
      goto error;
    }

    /* Restore Xspi command */
    sCommand->DataMode    = SavedDataMode;
    sCommand->DummyCycles = SavedDummyCycles;

    /* Update Size and address to erase next sector */
    local_address = local_address + sector_size;
    if (sector_size > local_size)
    {
      local_size = 0u;
    }
    else
    {
      local_size = local_size - sector_size;
    }
  }

  /* Check default value of Max duration allowed for Eras operation.
     If 0, then use HAL_XSPI default timeout. */
  if (CustomObject->NorFlashConfig.MaxSectorEraseTime == 0)
  {
    max_timing = CUSTOM_XSPI_TIMEOUT_VALUE;
  }
  else
  {
    max_timing = CustomObject->NorFlashConfig.MaxSectorEraseTime;
  }

  /* WIP Command */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, max_timing))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
    goto error;
  }

error:
  if (retr != EXTMEM_DRV_CUSTOM_OK)
  {
    DEBUG_STR("[ERROR] XSPI Abort");
    /* Abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(hxspi);
  }

  /* Restore Xspi command */
  sCommand->DataMode    = SavedDataMode;
  sCommand->DummyCycles = SavedDummyCycles;
  return retr;
}

/**
  * @brief Performs a mass erase of the entire memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_MassErase(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  uint32_t SavedCommand            = 0;
  uint32_t max_timing;
  NorFlashBasConfigTypeDef *NorConfig;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_MassErase");

  /* Verify if Memory is a NorFlash */
  if (IS_NOT_MEMORY_FLASH(CustomObject->MemType))
  {
    return EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE;
  }

  /* Check which memory configuration is to be used */
  if (CustomObject->NorFlashConfig.OptionalConfigEnable)
  {
    NorConfig = &CustomObject->NorFlashConfig.Optional;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }
  else
  {
    NorConfig = &CustomObject->NorFlashConfig.Startup;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }

  /* WIP Command */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, CUSTOM_XSPI_TIMEOUT_VALUE))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
    goto error;
  }

  /* WE Command */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteEnable(hxspi, sCommand,  NorConfig))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE;
    goto error;
  }

  /* Prepare Mass Erase Command */
  sCommand->Instruction        = NorConfig->Cmd_MassErase;
  sCommand->AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand->DataMode           = HAL_XSPI_DATA_NONE;
  sCommand->DummyCycles        = 0;

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE)  != HAL_OK)
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
    goto error;
  }

  /* Check default value of Max duration allowed for Erase operation.
     If 0, then use HAL_XSPI default timeout. */
  if (CustomObject->NorFlashConfig.MaxChipEraseTime == 0)
  {
    max_timing = CUSTOM_XSPI_TIMEOUT_VALUE;
  }
  else
  {
    max_timing = CustomObject->NorFlashConfig.MaxChipEraseTime;
  }

  /* Check which memory configuration is to be used */
  if (CustomObject->NorFlashConfig.OptionalConfigEnable)
  {
    NorConfig = &CustomObject->NorFlashConfig.Optional;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->OptionalConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }
  else
  {
    NorConfig = &CustomObject->NorFlashConfig.Startup;

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(&CustomObject->StartupConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      goto error;
    }
  }

  /* WIP Command */
  if (CUSTOM_XSPI_OK != CUSTOM_XSPI_GetWriteInProgress(hxspi, sCommand,  NorConfig, max_timing))
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY;
    goto error;
  }

error:
  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  if (retr != EXTMEM_DRV_CUSTOM_OK)
  {
    DEBUG_STR("[ERROR] XSPI Abort");
    /* Abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(hxspi);
  }

  return retr;
}

/**
  * @brief Enables memory-mapped mode for the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef
                                                                                *CustomObject)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_HandleTypeDef *hxspi               = CustomObject->Private.Handle;
  XSPI_MemoryMappedTypeDef sMemMappedCfg  = {0};

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode");

  /* NorFlash and Psram use case */
  if (IS_NOT_MEMORY_HYPERBUS(CustomObject->MemType))
  {
    XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
    MemoryBaseConfigTypeDef *MemConfig;

    /* Check which memory configuration is to be used */
    if (CustomObject->OptionalConfig.Frequency != 0U)
    {
      MemConfig = &CustomObject->OptionalConfig;
    }
    else
    {
      MemConfig = &CustomObject->StartupConfig;
    }

    /* Init Command */
    if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetCommand(MemConfig, &CustomObject->Private.Commandbase))
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
    }

    /* Initialize the read ID command */
    sCommand->OperationType = HAL_XSPI_OPTYPE_READ_CFG;
    sCommand->Instruction   = MemConfig->CommandRead;
    sCommand->DummyCycles   = MemConfig->DummyCycleRead;

    /* Configure the read command */
    if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
    }

    /* Init optional write Command */
    if (IS_ACCESS_MODE_TYPE(MemConfig->OptWriteAccessMode))
    {
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_SetWriteCommand(MemConfig, &CustomObject->Private.Commandbase))
      {
        retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
      }
    }

    /* Initialize the read ID command */
    sCommand->OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
    sCommand->Instruction   = MemConfig->CommandWrite;
    sCommand->DummyCycles   = MemConfig->DummyCycleWrite;
    /* Configure the read command */
    if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT;
    }
  }
  else   /* HyperBus use case */
  {
    XSPI_HyperbusCmdTypeDef *sCommand = &CustomObject->Private.CommandbaseHyperbus;

    /* Initialize the transmit command */
    sCommand->AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;   /* CA[46] = 0 */
    sCommand->AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    sCommand->Address      = 0;
    sCommand->DQSMode      = CustomObject->StartupConfig.DqsMode;
    sCommand->DataMode     = ((CustomObject->StartupConfig.AccessMode) & DATA_MASK);
    sCommand->DataLength   = 0;

    if (HAL_XSPI_HyperbusCmd(hxspi, sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      retr = EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND;
    }
  }

  /* Activation of memory-mapped mode */
  sMemMappedCfg.TimeOutActivation  = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeoutPeriodClock = 0x50;
  if (HAL_XSPI_MemoryMapped(hxspi, &sMemMappedCfg) != HAL_OK)
  {
    retr = EXTMEM_DRV_CUSTOM_ERR_ENABLE_MEM_MAP;
  }

  if (retr != EXTMEM_DRV_CUSTOM_OK)
  {
    DEBUG_STR("[ERROR] XSPI Abort");
    /* Abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(hxspi);
  }

  /* return status */
  return retr;
}

/**
  * @brief Disables memory-mapped mode for the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef
    *CustomObject)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_HandleTypeDef *hxspi               = CustomObject->Private.Handle;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode");

  /* Abort any ongoing transaction for the next action */
  if (HAL_XSPI_Abort(hxspi) != HAL_OK)
  {
    DEBUG_STR("[ERROR] XSPI Abort");
    retr = EXTMEM_DRV_CUSTOM_ERR_DISABLE_MEM_MAP;
  }

  return retr;
}

/**
  * @brief Returns the base address of the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param BaseAddress Pointer to store the base address.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetMapAddress(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                      uint32_t *BaseAddress)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_ERR;
  XSPI_HandleTypeDef *hxspi = CustomObject->Private.Handle;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_GetMapAddress");

  /* Retrieve XSPI1 base address from CMSIS file */
#if defined(XSPI1)
  if (hxspi->Instance == XSPI1)
  {
    *BaseAddress = XSPI1_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* XSPI1 */
  /* Retrieve XSPI2 base address from CMSIS file */
#if defined(XSPI2)
  if (hxspi->Instance == XSPI2)
  {
    *BaseAddress = XSPI2_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* XSPI2 */
  /* Retrieve XSPI3 base address from CMSIS file */
#if defined(XSPI3)
  if (hxspi->Instance == XSPI3)
  {
    *BaseAddress = XSPI3_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* XSPI3 */
  /* Retrieve OCTOSPI1 base address from CMSIS file */
#if defined(OCTOSPI1)
  if (hxspi->Instance == OCTOSPI1)
  {
    *BaseAddress = OCTOSPI1_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* OCTOSPI1 */
  /* Retrieve OCTOSPI2 base address from CMSIS file */
#if defined(OCTOSPI2)
  if (hxspi->Instance == OCTOSPI2)
  {
    *BaseAddress = OCTOSPI2_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* OCTOSPI2 */
  /* Retrieve HSPI1 base address from CMSIS file */
#if defined(HSPI1)
  if (hxspi->Instance == HSPI1)
  {
    *BaseAddress = HSPI1_BASE;
    retr = EXTMEM_DRV_CUSTOM_OK;
  }
#endif /* HSPI1 */

  return retr;
}

/**
  * @brief Returns information about the memory device.
  * @param CustomObject Pointer to the CUSTOM driver object.
  * @param MemInfo Pointer to the memory information structure to be filled.
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetInfo(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef *MemInfo)
{
  EXTMEM_DRIVER_CUSTOM_StatusTypeDef retr = EXTMEM_DRV_CUSTOM_OK;
  XSPI_HandleTypeDef *hxspi = CustomObject->Private.Handle;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_GetInfo");

  /* Fill MemInfo structure with memory information */
  MemInfo->MemType       = CustomObject->MemType;
  MemInfo->MemSize       =
    (EXTMEM_CUSTOM_MemSizeTypeDef)((hxspi->Instance->DCR1 & XSPI_DCR1_DEVSIZE) >> XSPI_DCR1_DEVSIZE_Pos);
  MemInfo->MemStandard   = (EXTMEM_CUSTOM_MemStdTypeDef)(hxspi->Instance->DCR1 & XSPI_DCR1_MTYP);
  MemInfo->MemAccessMode =
    (EXTMEM_CUSTOM_AccessModeTypeDef)(ACCESS_MODE_TYPE | (hxspi->Instance->CCR & (ACCESS_MODE_MASK)));
  MemInfo->MemRealFreq   = CustomObject->Private.RealClock;
  MemInfo->JedecID.ManufacturerID    = CustomObject->Private.JedecID.ManufacturerID;
  MemInfo->JedecID.MemoryType        = CustomObject->Private.JedecID.MemoryType;
  MemInfo->JedecID.MemoryDensity     = CustomObject->Private.JedecID.MemoryDensity;

  return retr;
}

/**
  * @brief Default implementation of memory copy functionality.
  * @param Destination_Address Destination address where data is to be copied.
  * @param ptrData Pointer to source data to be copied.
  * @param DataSize Size of data to be copied (in bytes).
  */
__weak void EXTMEM_DRIVER_CUSTOM_MemCopy(uint32_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize)
{
  uint32_t *ptrDest = Destination_Address;
  /* Write the data */
  for (uint32_t index = 0u; index < DataSize; index = index + 4u)
  {
    *ptrDest = ((uint32_t)ptrData[index] | ((uint32_t)ptrData[index + 1u] << 8u) |
                ((uint32_t)ptrData[index + 2u] << 16u) | ((uint32_t)ptrData[index + 3u] << 24u));
    ptrDest++;
  }
}

/**
  * @brief Default weak implementation for EXTMEM_CUSTOM_CFGSTEP_USER_CALLBACK steps.
  * @param CustomObjectCtx User context forwarded to the callback (CBConfigStep.UserCtx).
  * @param UserArg0 User argument 0 (CBConfigStep.Arg0).
  * @param UserArg1 User argument 1 (CBConfigStep.Arg1).
  * @retval EXTMEM_DRIVER_CUSTOM_StatusTypeDef: EXTMEM_DRV_CUSTOM_OK if successful, error code otherwise.
  */
__weak EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_UserCallback(void *CustomObjectCtx,
                                                                            uint32_t UserArg0,
                                                                            uint32_t UserArg1)
{
  (void)UserArg0;
  (void)UserArg1;

  /* Example: if CustomObjectCtx is set to (void*)CustomObject, retrieve it here. */
  EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *obj = (EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *)CustomObjectCtx;
  XSPI_HandleTypeDef *hxspi = NULL;
  if (obj != NULL)
  {
    hxspi = obj->Private.Handle;
  }
  (void)hxspi;

  DEBUG_API();
  DEBUG_STR("EXTMEM_DRIVER_CUSTOM_UserCallback");

  /* User can override this weak function in application code. */
  return EXTMEM_DRV_CUSTOM_OK;
}

/**
  * @}
  */

/** @addtogroup CUSTOM_Private_Functions
  * @{
  */

/**
  * @brief This function initializes the XSPI peripheral associated to Custom Driver object
  * @param CustomObject Custom driver object
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_Init(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  CUSTOM_XSPI_StatusTypeDef retr = CUSTOM_XSPI_OK;
  /*
   * Some of the XSPI memory size initialization parameters, as MemorySize, MemoryType, ChipSelectHighTime, ...
   * are set according to values contained in configuration items present in CustomObject structure.
   */

  CustomObject->Private.Handle->Init.FifoThresholdByte       = 2;
  CustomObject->Private.Handle->Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  CustomObject->Private.Handle->Init.MemoryType              = CustomObject->MemStandard;
  CustomObject->Private.Handle->Init.MemorySize              = CustomObject->MemSize;
  CustomObject->Private.Handle->Init.ChipSelectHighTimeCycle = CustomObject->MemChipSelectHighTimeCycle;
  CustomObject->Private.Handle->Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  CustomObject->Private.Handle->Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;

  /* Verify if Memory is a Psram Type (PSRAM XSPI or HyperRam) */
  if (IS_MEMORY_PSRAM(CustomObject->MemType))
  {
    if (IS_MEMORY_HYPERBUS(CustomObject->MemType))
    {
      /* HyperRam memory */
      CustomObject->Private.Handle->Init.WrapSize            = CustomObject->HyperRamConfig.RamConfig.WrapSize;
      CustomObject->Private.Handle->Init.ChipSelectBoundary  = CustomObject->HyperRamConfig.RamConfig.CsBoundarySize;
      CustomObject->Private.Handle->Init.Refresh             = CustomObject->HyperRamConfig.RamConfig.Refresh;
    }
    else
    {
      /* PSRAM Memory */
      CustomObject->Private.Handle->Init.WrapSize            = CustomObject->PsramConfig.WrapSize;
      CustomObject->Private.Handle->Init.ChipSelectBoundary  = CustomObject->PsramConfig.CsBoundarySize;
      CustomObject->Private.Handle->Init.Refresh             = CustomObject->PsramConfig.Refresh;
    }
  }
  else
  {
    /* Flash memory. Field not used. Keep default value */
    CustomObject->Private.Handle->Init.WrapSize              = HAL_XSPI_WRAP_NOT_SUPPORTED;
    CustomObject->Private.Handle->Init.ChipSelectBoundary    = HAL_XSPI_BONDARYOF_NONE;
    CustomObject->Private.Handle->Init.Refresh               = 0;
  }

  /* Additional settings that could be provided in configuration file.
     If not provided, XPSI configuration as done by CubeMX tool will apply.
     If provided, setting value in configuration file will apply */
  /* Sample Shift setting */
  if (IS_CUSTOM_SETTING_CFG_DEFINED(CustomObject->SampleShiftCfg))
  {
    CustomObject->Private.Handle->Init.SampleShifting        = CUSTOM_SETTING_CFG_VALUE(CustomObject->SampleShiftCfg);
  }
  else
  {
    CustomObject->Private.Handle->Init.SampleShifting        = HAL_XSPI_SAMPLE_SHIFT_NONE;
  }

  /* Other settings (that could be applied to older XSPI versions for instance, for debug purposes
     CustomObject->Private.Handle->Init.DelayHoldQuarterCycle     = HAL_XSPI_DHQC_DISABLE;
  */
  /* Selected by user. Linked to IO Manager and PCB */
  /*
  CustomObject->Private.Handle->Init.MaxTran = 0;
  CustomObject->Private.Handle->Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  */

  /* DeInit the XSPI */
  if (HAL_XSPI_GetState(CustomObject->Private.Handle) != HAL_XSPI_STATE_RESET)
  {
    if (HAL_XSPI_DeInit(CustomObject->Private.Handle) != HAL_OK)
    {
      DEBUG_STR("[ERROR] XSPI DeInit (before Init)");
      retr = CUSTOM_XSPI_KO;
    }
  }

  /* Init the XSPI */
  if (HAL_XSPI_Init(CustomObject->Private.Handle) != HAL_OK)
  {
    DEBUG_STR("[ERROR] XSPI Init");
    retr = CUSTOM_XSPI_KO;
  }

  return retr;
}

/**
  * @brief This function de-initializes the XSPI peripheral associated to Custom Driver object
  *        and resets the associated Command descriptor.
  * @param CustomObject Custom driver object
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_DeInit(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  CUSTOM_XSPI_StatusTypeDef retr = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;

  /* Check if XSPI is not already in reset state */
  if (CustomObject->Private.Handle->State != HAL_XSPI_STATE_RESET)
  {
    /* Implementation of reset procedure if specified in configuration file (stored in CustomObject structure) */
    if (CustomObject->ResetMethod != EXTMEM_CUSTOM_RESET_METHOD_UNDEFINED)
    {
      /* Execute Reset procedure */
      if (CUSTOM_XSPI_OK != CUSTOM_XSPI_ResetMemory(CustomObject))
      {
        retr = CUSTOM_XSPI_KO;
      }
    }

    /* De-init the XSPI */
    if (HAL_XSPI_DeInit(CustomObject->Private.Handle) != HAL_OK)
    {
      DEBUG_STR("[ERROR] XSPI DeInit");
      retr = CUSTOM_XSPI_KO;
    }
    else
    {
      /* Reset Command Base */
      sCommand->OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
      sCommand->IOSelect              = HAL_XSPI_SELECT_IO_3_0;
      sCommand->Instruction           = 0;

      /* Instruction part */
      sCommand->InstructionMode       = HAL_XSPI_INSTRUCTION_NONE;
      sCommand->InstructionWidth      = HAL_XSPI_INSTRUCTION_8_BITS;
      sCommand->InstructionDTRMode    = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

      /* Address part */
      sCommand->Address               = 0;
      sCommand->AddressMode           = HAL_XSPI_ADDRESS_NONE;
      sCommand->AddressWidth          = HAL_XSPI_ADDRESS_8_BITS;
      sCommand->AddressDTRMode        = HAL_XSPI_ADDRESS_DTR_DISABLE;

      /* Alternate Byte mode not yet supported */
      sCommand->AlternateBytes        = 0;
      sCommand->AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
      sCommand->AlternateBytesWidth   = HAL_XSPI_ALT_BYTES_8_BITS;
      sCommand->AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;

      /* Data part */
      sCommand->DataMode              = HAL_XSPI_DATA_NONE;
      sCommand->DataLength            = 0;
      sCommand->DataDTRMode           = HAL_XSPI_DATA_DTR_DISABLE;

      /* Others settings */
      sCommand->DummyCycles           = 0;
      sCommand->DQSMode               = HAL_XSPI_DQS_DISABLE;
    }
  }

  return retr;
}

/**
  * @brief This function sets the clock provided to memory according to input clock and request
  * @param CustomObject Custom driver object
  * @param ClockIn Input clock
  * @param ClockRequested Expected clock frequency value
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetClock(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                      uint32_t ClockIn, uint32_t ClockRequested)
{
  CUSTOM_XSPI_StatusTypeDef retr = CUSTOM_XSPI_OK;
  uint32_t divider   = 0;
  uint32_t ClockReal = 0;

  /* As requested clock value will be checked against all possible divisions of input clock, it could happen that
     division result exceeds clock value provided in configuration file.
     For example, if input clock is 400Mhz and requested clock is 133 Mhz, input clock/3 will exceed
     configuration file value.
     In order to allow such use case, requested clock value is slightly increased (for values > 50 Mhz) */
  if (ClockRequested > CUSTOM_CLOCK_VALUE_FOR_ROUNDING)
  {
    ClockRequested += CUSTOM_CLOCK_VALUE_INCREMENT;
  }

  if (ClockRequested == 0u)
  {
    retr = CUSTOM_XSPI_KO;
  }
  else
  {
    divider = (ClockIn / ClockRequested);
    if (divider >= 1u)
    {
      ClockReal = ClockIn / divider;
      if (ClockReal <= ClockRequested)
      {
        divider--;
      }
    }

    /* Real clock calculation */
    ClockReal = ClockIn / (divider + 1u);
    CustomObject->Private.RealClock = ClockReal;

    MODIFY_REG(CustomObject->Private.Handle->Instance->DCR2, XSPI_DCR2_PRESCALER,
               (uint32_t)divider << XSPI_DCR2_PRESCALER_Pos);
  }

  DEBUG_STR_INT("[CLOCK] New clock: ", CustomObject->Private.RealClock);

  return retr;
}

/**
  * @brief This function performs a Read Register operation
  * @param CustomObject Custom driver object
  * @param RegisterConfig Register access details
  * @param ReadType Read type
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_ReadRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                          OptionalRegisterConfigTypeDef *RegisterConfig,
                                                          CUSTOM_XSPI_ReadTypeTypeDef ReadType)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  uint32_t SavedCommand            = 0;
  uint8_t reg[4]                   = {0};

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  /* Initialize command instruction to be sent */
  sCommand->OperationType = HAL_XSPI_OPTYPE_COMMON_CFG;

  /* Check if read operation */
  if (IS_CFGSTEP_REG_ACCESS_TYPE(RegisterConfig->ConfigStepType))
  {
    /* Check if Read operation is the initial one or after the Write operation */
    if (ReadType == CUSTOM_XSPI_READ_TYPE_INITIAL)
    {
      sCommand->Instruction = RegisterConfig->RWConfigStep.CommandRegisterRead;
      DEBUG_STR_HEX("[READ REG INITIAL] Command: ", sCommand->Instruction);
    }
    else if (ReadType == CUSTOM_XSPI_READ_TYPE_AFTER_WRITE)
    {
      sCommand->Instruction = RegisterConfig->RWConfigStep.CommandRegisterReadAW;
      DEBUG_STR_HEX("[READ AFTER WRITE] Command: ", sCommand->Instruction);
    }
    else
    {
      /* Unexpected type of Read operation */
      retr = CUSTOM_XSPI_KO;
      goto error;
    }

    /* Check if Read register use an address */
    if (IS_CFGSTEP_ADDR(RegisterConfig->ConfigStepType))
    {
      sCommand->Address = RegisterConfig->RWConfigStep.RegisterAddress;
      DEBUG_STR_HEX("[READ WITH ADDR  ] Address: ", sCommand->Address);
    }
    else
    {
      sCommand->AddressMode = HAL_XSPI_ADDRESS_NONE;
    }

  }
  else
  {
    /* Unexpected type of Register config element */
    retr = CUSTOM_XSPI_KO;
    goto error;
  }

  /* Set nb of expected data */
  if (IS_CFGSTEP_DATASIZE_24BIT(RegisterConfig->ConfigStepType))
  {
    sCommand->DataLength    = 3;
  }
  else if (IS_CFGSTEP_DATASIZE_16BIT(RegisterConfig->ConfigStepType))
  {
    sCommand->DataLength    = 2;
  }
  else
  {
    sCommand->DataLength    = 1;
  }

  /* Check if command use DTR mode and length is odd : ensure nb of received bytes is even */
  if ((sCommand->DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
      && (sCommand->DataLength & 1))
  {
    sCommand->DataLength    += 1;
  }

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Read Register Command:  ", sCommand->Instruction);
    retr = CUSTOM_XSPI_KO;
    goto error;
  }

  if (HAL_XSPI_Receive(hxspi, reg, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Read Register Value:  ", reg[0]);
    retr = CUSTOM_XSPI_KO;
    goto error;
  }

  /* Can be used for debug */
  if (IS_CFGSTEP_DATASIZE_24BIT(RegisterConfig->ConfigStepType))
  {
    CustomObject->Private.ReadRegValue = (reg[0] << 16) | (reg[1] << 8) | reg[2];
  }
  else if (IS_CFGSTEP_DATASIZE_16BIT(RegisterConfig->ConfigStepType))
  {
    CustomObject->Private.ReadRegValue = (reg[0] << 8) | reg[1];
  }
  else
  {
    CustomObject->Private.ReadRegValue = reg[0];
  }
  DEBUG_STR_HEX("[READ REG        ] Value:   ", CustomObject->Private.ReadRegValue);

error:
  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief This function performs a Write Register operation
  * @param CustomObject Custom driver object
  * @param RegisterConfig Register access details
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_WriteRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                           OptionalRegisterConfigTypeDef *RegisterConfig)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  uint32_t SavedCommand            = 0;
  uint8_t reg[4]                   = {0};
  uint32_t reg_value               = 0;
  uint32_t mask                    = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  DEBUG_STR_HEX("[WRITE REG] Command:  ", RegisterConfig->RWConfigStep.CommandRegisterWrite);

  /* Initialize mask and register reg */
  if (IS_CFGSTEP_DATASIZE_24BIT(RegisterConfig->ConfigStepType))
  {
    /* 24 bits register content */
    mask = (0xFFFFFFU - ((RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFFFU));
    reg_value = ((RegisterConfig->RWConfigStep.RegisterValue & RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFFFU)
                | (CustomObject->Private.ReadRegValue & mask);
    reg[0] = (uint8_t)(reg_value >> 16U);
    reg[1] = (uint8_t)((reg_value >> 8U) & 0xFFU);
    reg[2] = (uint8_t)(reg_value & 0xFFU);

    sCommand->DataLength    = 3;

    DEBUG_STR_HEX("[WRITE REG]  Value1:  ", reg[0]);
    DEBUG_STR_HEX("[WRITE REG]  Value2:  ", reg[1]);
    DEBUG_STR_HEX("[WRITE REG]  Value3:  ", reg[2]);
  }
  else if (IS_CFGSTEP_DATASIZE_16BIT(RegisterConfig->ConfigStepType))
  {
    /* 16 bits register content */
    mask = (0xFFFFU - ((RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFU));
    reg_value = ((RegisterConfig->RWConfigStep.RegisterValue & RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFU)
                | (CustomObject->Private.ReadRegValue & mask);
    reg[0] = (uint8_t)(reg_value >> 8U);
    reg[1] = (uint8_t)(reg_value & 0xFFU);

    sCommand->DataLength    = 2;

    DEBUG_STR_HEX("[WRITE REG]  Value1:  ", reg[0]);
    DEBUG_STR_HEX("[WRITE REG]  Value2:  ", reg[1]);
  }
  else
  {
    /* 8 bits register content */
    mask = (0xFFU - ((RegisterConfig->RWConfigStep.RegisterMask) & 0xFFU));
    reg[0] = ((RegisterConfig->RWConfigStep.RegisterValue & RegisterConfig->RWConfigStep.RegisterMask) & 0xFFU)
             | (CustomObject->Private.ReadRegValue & mask);

    /* Check if command use DTR mode */
    if (sCommand->DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
    {
      sCommand->DataLength    = 2;
      reg[1] = reg[0];
    }
    else
    {
      sCommand->DataLength    = 1;
    }
    DEBUG_STR_HEX("[WRITE REG]   Value:  ", reg[0]);
  }

  /* Initialize command instruction to be sent */
  sCommand->OperationType   = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->Instruction     = RegisterConfig->RWConfigStep.CommandRegisterWrite;
  sCommand->DummyCycles     = 0;

  /* Check if Write register operation use an address */
  if (IS_CFGSTEP_ADDR(RegisterConfig->ConfigStepType))
  {
    sCommand->Address = RegisterConfig->RWConfigStep.RegisterAddress;
    DEBUG_STR_HEX("[WRITE REG] Address: ", sCommand->Address);
  }
  else
  {
    sCommand->AddressMode = HAL_XSPI_ADDRESS_NONE;
  }

  /* Send Command */
  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Write Register Command:  ", sCommand->Instruction);
    retr = CUSTOM_XSPI_KO;
    goto error;
  }

  if (HAL_XSPI_Transmit(hxspi, reg, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Write Register Value:  ", reg[0]);
    retr = CUSTOM_XSPI_KO;
    goto error;
  }

error:
  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief This function sends a specific Instruction to memory
  * @param CustomObject Custom driver object
  * @param Instruction Instruction code
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_Send_Instruction(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                              uint32_t Instruction)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  uint32_t SavedCommand            = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  /* Initialize command instruction to be sent */
  sCommand->OperationType    = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->Instruction      = Instruction;
  sCommand->AddressMode      = HAL_XSPI_ADDRESS_NONE;
  sCommand->DataMode         = HAL_XSPI_DATA_NONE;

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
  }

  DEBUG_STR_HEX("[INSTRUCTION] Command:  ", sCommand->Instruction);

  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief This function performs a Memory Reset operation
  * @param CustomObject Custom driver object
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_ResetMemory(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  MemoryBaseConfigTypeDef *memCfg;
  uint32_t nbConfig = 1;
  uint32_t idx = 0;
  uint32_t SavedCommand = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  sCommand->OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand->DataMode           = HAL_XSPI_DATA_NONE;
  sCommand->DummyCycles        = 0U;

  /* If there's an optional config then execute reset procedure twice :
  *    First execute Reset command corresponding to Optional config settings,
  *    Second using the startup config settings
  *  Otherwise use directly startup config settings
  */
  if (CustomObject->OptionalConfig.Frequency != 0)
  {
    memCfg = &CustomObject->OptionalConfig;

    /* If Reset Instruction exist in 16bits and 8 bits, execute both reset instructions */
    if (memCfg->InstructionSize != CustomObject->StartupConfig.InstructionSize)
    {
      nbConfig = 2;
    }
  }
  else
  {
    memCfg = &CustomObject->StartupConfig;
  }

  for (idx = 0; idx < nbConfig; idx++)
  {
    /* Check selected type of Reset procedure */
    switch (CustomObject->ResetMethod)
    {
      /* Reset Method is based on 0x66 and 0x99 instructions */
      case EXTMEM_CUSTOM_RESET_METHOD_66_99 :
      case EXTMEM_CUSTOM_RESET_METHOD_6699_9966 :
      case EXTMEM_CUSTOM_RESET_METHOD_6666_9999 :
      {
        /* Configure Instruction according to Optional configuration description */
        sCommand->InstructionMode       = ((memCfg->AccessMode) & INSTRUCTION_MASK);
        sCommand->InstructionWidth      = memCfg->InstructionSize;
        sCommand->InstructionDTRMode    = ((memCfg->AccessMode) & HAL_XSPI_INSTRUCTION_DTR_ENABLE);

        if (memCfg->InstructionSize == EXTMEM_CUSTOM_INSTRUCTION_8_BITS)
        {
          sCommand->Instruction      = CUSTOM_RESET_INS_66_8BITS;
          if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
          {
            DEBUG_STR_HEX("[ERROR] Reset Command:  ", sCommand->Instruction);
            retr = CUSTOM_XSPI_KO;
          }

          DEBUG_STR_HEX("[RESET] Command:  ", sCommand->Instruction);

          /* Initialize command instruction to be sent */
          sCommand->Instruction      = CUSTOM_RESET_INS_99_8BITS;
          if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
          {
            DEBUG_STR_HEX("[ERROR] Reset Command:  ", sCommand->Instruction);
            retr = CUSTOM_XSPI_KO;
          }
          DEBUG_STR_HEX("[RESET] Command:  ", sCommand->Instruction);
        }
        else if (memCfg->InstructionSize == EXTMEM_CUSTOM_INSTRUCTION_16_BITS)
        {
          if (CustomObject->ResetMethod == EXTMEM_CUSTOM_RESET_METHOD_6699_9966)
          {
            sCommand->Instruction      = CUSTOM_RESET_INS_6699_16BITS;
          }
          else
          {
            sCommand->Instruction      = CUSTOM_RESET_INS_6666_16BITS;
          }
          if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
          {
            DEBUG_STR_HEX("[ERROR] Reset Command:  ", sCommand->Instruction);
            retr = CUSTOM_XSPI_KO;
          }
          DEBUG_STR_HEX("[RESET] Command:  ", sCommand->Instruction);

          /* Initialize command instruction to be sent */
          if (CustomObject->ResetMethod == EXTMEM_CUSTOM_RESET_METHOD_6699_9966)
          {
            sCommand->Instruction      = CUSTOM_RESET_INS_9966_16BITS;
          }
          else
          {
            sCommand->Instruction      = CUSTOM_RESET_INS_9999_16BITS;
          }
          if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
          {
            DEBUG_STR_HEX("[ERROR] Reset Command:  ", sCommand->Instruction);
            retr = CUSTOM_XSPI_KO;
          }
          DEBUG_STR_HEX("[RESET] Command:  ", sCommand->Instruction);
        }
        else
        {
          DEBUG_STR("[ERROR] Reset Command: NOT SUPPORTED ");
          /* Other types of Instructions sizes not handled */
          retr = CUSTOM_XSPI_KO;
        }
      }
      break;

      case EXTMEM_CUSTOM_RESET_METHOD_FF:
      {
        /* Configure Instruction according to Optional configuration description */
        sCommand->InstructionMode       = ((memCfg->AccessMode) & INSTRUCTION_MASK);
        sCommand->InstructionWidth      = memCfg->InstructionSize;
        sCommand->InstructionDTRMode    = ((memCfg->AccessMode) & HAL_XSPI_INSTRUCTION_DTR_ENABLE);

        /* Used Address to generate 4 clocks CS low after instruction (APMemory) */
        sCommand->AddressMode           = HAL_XSPI_ADDRESS_8_LINES;
        sCommand->AddressWidth          = HAL_XSPI_ADDRESS_32_BITS;
        sCommand->AddressDTRMode        = HAL_XSPI_ADDRESS_DTR_DISABLE;

        /* Set Instruction 0xFF in 8bits */
        sCommand->Instruction            = CUSTOM_RESET_INS_FF_8BITS;

        if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
        {
          DEBUG_STR_HEX("[ERROR] Reset Command:  ", sCommand->Instruction);
          retr = CUSTOM_XSPI_KO;
        }
        DEBUG_STR_HEX("[RESET] Command:  ", sCommand->Instruction);
      }
      break;


      default :
        /* Unexpected reset type */
        retr = CUSTOM_XSPI_KO;
        break;
    }

    /* Ensure minimal delay is repseted after Reset, if any */
    if (CustomObject->ResetDelay != 0)
    {
      HAL_Delay(CustomObject->ResetDelay);
    }

    memCfg = &CustomObject->StartupConfig;
  }

  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief This function configures a Command descriptor according to Configuration object
  * @param ConfigObject Configuration object
  * @param sCommand Command descriptor to be initialised
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetCommand(MemoryBaseConfigTypeDef *ConfigObject,
                                                        XSPI_RegularCmdTypeDef *sCommand)
{
  CUSTOM_XSPI_StatusTypeDef retr  = CUSTOM_XSPI_OK;

  sCommand->OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->IOSelect              = HAL_XSPI_SELECT_IO_3_0;
  sCommand->Instruction           = 0;

  /* Instruction part */
  sCommand->InstructionMode       = ((ConfigObject->AccessMode) & INSTRUCTION_MASK);
  sCommand->InstructionWidth      = ConfigObject->InstructionSize;
  sCommand->InstructionDTRMode    = ((ConfigObject->AccessMode) & HAL_XSPI_INSTRUCTION_DTR_ENABLE);

  /* Address part */
  sCommand->Address               = 0;
  sCommand->AddressMode           = ((ConfigObject->AccessMode) & ADDRESS_MASK);
  sCommand->AddressWidth          = ConfigObject->AddressSize;
  sCommand->AddressDTRMode        = ((ConfigObject->AccessMode) & HAL_XSPI_ADDRESS_DTR_ENABLE);

  /* Alternate Byte mode not yet supported */
  sCommand->AlternateBytes        = 0;
  sCommand->AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand->AlternateBytesWidth   = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand->AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;

  /* Data part */
  sCommand->DataMode              = ((ConfigObject->AccessMode) & DATA_MASK);;
  sCommand->DataLength            = 0;
  sCommand->DataDTRMode           = ((ConfigObject->AccessMode) & HAL_XSPI_DATA_DTR_ENABLE);;

  /* Others settings */
  sCommand->DummyCycles           = ConfigObject->DummyRegisterRead;
  sCommand->DQSMode               = ConfigObject->DqsMode;

  return retr;
}

/**
  * @brief Saves the command descriptor to the specified location.
  * @param sCommand Pointer to the command to save.
  * @param CommandToSave Pointer to the location where the command will be saved.
  */
static void CUSTOM_XSPI_SaveCommand(XSPI_RegularCmdTypeDef *sCommand, uint32_t *CommandToSave)
{
  /* Save global command */
  *CommandToSave = ((sCommand->InstructionMode & XSPI_CCR_IMODE_Msk) |
                    (sCommand->InstructionWidth & XSPI_CCR_ISIZE_Msk) |
                    (sCommand->InstructionDTRMode & XSPI_CCR_IDTR_Msk) |
                    (sCommand->AddressMode & XSPI_CCR_ADMODE_Msk) |
                    (sCommand->AddressWidth & XSPI_CCR_ADSIZE_Msk) |
                    (sCommand->AddressDTRMode & XSPI_CCR_ADDTR_Msk) |
                    (sCommand->DataMode & XSPI_CCR_DMODE_Msk) |
                    (sCommand->DataDTRMode & XSPI_CCR_DDTR_Msk) |
                    (sCommand->DQSMode & XSPI_CCR_DQSE_Msk) |
                    ((sCommand->DummyCycles & XSPI_TCR_DCYC_Msk) << XSPI_CCR_ABMODE_Pos));
}

/**
  * @brief Restores the command descriptor from the specified location.
  * @param sCommand Pointer to the command to restore.
  * @param CommandSaved Pointer to the saved command used for restoration.
  */
static void CUSTOM_XSPI_RestoreCommand(XSPI_RegularCmdTypeDef *sCommand, uint32_t *CommandSaved)
{
  /* Restore instruction command part */
  sCommand->InstructionMode       = ((*CommandSaved) & XSPI_CCR_IMODE_Msk);
  sCommand->InstructionWidth      = ((*CommandSaved) & XSPI_CCR_ISIZE_Msk);
  sCommand->InstructionDTRMode    = ((*CommandSaved) & XSPI_CCR_IDTR_Msk);

  /* Restore Address command part */
  sCommand->AddressMode           = ((*CommandSaved) & XSPI_CCR_ADMODE_Msk);
  sCommand->AddressWidth          = ((*CommandSaved) & XSPI_CCR_ADSIZE_Msk);
  sCommand->AddressDTRMode        = ((*CommandSaved) & XSPI_CCR_ADDTR_Msk);

  /* Alternate Byte mode not yet supported */
  sCommand->AlternateBytes        = 0;
  sCommand->AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand->AlternateBytesWidth   = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand->AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;

  /* Restore Data command part */
  sCommand->DataMode              = ((*CommandSaved) & XSPI_CCR_DMODE_Msk);
  sCommand->DataDTRMode           = ((*CommandSaved) & XSPI_CCR_DDTR_Msk);

  /* Restore DQS and Dummycycles settings of the command */
  sCommand->DummyCycles           = (((*CommandSaved) >> XSPI_CCR_ABMODE_Pos) & XSPI_TCR_DCYC_Msk);
  sCommand->DQSMode               = ((*CommandSaved) & XSPI_CCR_DQSE_Msk);
}

/**
  * @brief Configures a Write Command descriptor according to the configuration object.
  * @param ConfigObject Pointer to the configuration object.
  * @param sCommand Pointer to the command descriptor to initialize.
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetWriteCommand(MemoryBaseConfigTypeDef *ConfigObject,
                                                             XSPI_RegularCmdTypeDef *sCommand)
{
  CUSTOM_XSPI_StatusTypeDef retr  = CUSTOM_XSPI_OK;

  sCommand->OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->IOSelect              = HAL_XSPI_SELECT_IO_3_0;
  sCommand->Instruction           = 0;

  /* Instruction part */
  sCommand->InstructionMode       = ((ConfigObject->OptWriteAccessMode) & INSTRUCTION_MASK);
  sCommand->InstructionWidth      = ConfigObject->InstructionSize;
  sCommand->InstructionDTRMode    = ((ConfigObject->OptWriteAccessMode) & HAL_XSPI_INSTRUCTION_DTR_ENABLE);

  /* Address part */
  sCommand->Address               = 0;
  sCommand->AddressMode           = ((ConfigObject->OptWriteAccessMode) & ADDRESS_MASK);
  sCommand->AddressWidth          = ConfigObject->AddressSize;
  sCommand->AddressDTRMode        = ((ConfigObject->OptWriteAccessMode) & HAL_XSPI_ADDRESS_DTR_ENABLE);

  /* Alternate Byte mode not yet supported */
  sCommand->AlternateBytes        = 0;
  sCommand->AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand->AlternateBytesWidth   = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand->AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;

  /* Data part */
  sCommand->DataMode              = ((ConfigObject->OptWriteAccessMode) & DATA_MASK);
  sCommand->DataLength            = 0;
  sCommand->DataDTRMode           = ((ConfigObject->OptWriteAccessMode) & HAL_XSPI_DATA_DTR_ENABLE);

  /* Others settings */
  sCommand->DummyCycles           = ConfigObject->DummyCycleWrite;
  sCommand->DQSMode               = ConfigObject->DqsMode;

  return retr;
}

/**
  * @brief Sets the Write Enable (WE) bit in the memory device.
  * @param hxspi Pointer to the HAL XSPI handle.
  * @param sCommand Pointer to the command descriptor to use.
  * @param NorConfig Pointer to the Nor Flash configuration descriptor.
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_SetWriteEnable(XSPI_HandleTypeDef *hxspi, XSPI_RegularCmdTypeDef *sCommand,
                                                            NorFlashBasConfigTypeDef *NorConfig)
{
  CUSTOM_XSPI_StatusTypeDef retr  = CUSTOM_XSPI_OK;
  XSPI_AutoPollingTypeDef sConfig = {0};
  uint32_t SavedCommand           = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  /* Initialize command instruction to be sent */
  sCommand->OperationType   = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->Instruction     = NorConfig->Cmd_WE;
  sCommand->AddressMode     = HAL_XSPI_ADDRESS_NONE;
  sCommand->DataMode        = HAL_XSPI_DATA_NONE;
  sCommand->DummyCycles     = 0;

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
    DEBUG_STR_HEX("[ERROR] WEL Command:  ", sCommand->Instruction);
    goto error;
  }
  DEBUG_STR_HEX("\t[WEL] Command:  ", sCommand->Instruction);

  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  /* Reconfigure XSPI to automatic polling mode to wait for write enabling */
  sCommand->Instruction     = NorConfig->Cmd_RDSR;

  if (sCommand->DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
  {
    sCommand->DataLength    = 2;
  }
  else
  {
    sCommand->DataLength    = 1;
  }

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
    DEBUG_STR_HEX("[ERROR] RDSR Command:  ", sCommand->Instruction);
    goto error;
  }

  /* Reconfigure XSPI to automatic polling mode to wait for write enabling */
  sConfig.MatchMode         = HAL_XSPI_MATCH_MODE_AND;
  sConfig.AutomaticStop     = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  sConfig.IntervalTime      = 0x10;
  sConfig.MatchMask         = NorConfig->MatchMask_WEL;
  sConfig.MatchValue        = NorConfig->MatchValue_WEL;

  if (HAL_XSPI_AutoPolling(hxspi, &sConfig, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
    DEBUG_STR("[ERROR] WEL Autopolling: ");
    DEBUG_STR_HEX("[ERROR] WEL MatchMask:  ", NorConfig->MatchMask_WEL);
    DEBUG_STR_HEX("[ERROR] WEL MatchValue: ", NorConfig->MatchValue_WEL);
    DEBUG_STR_HEX("[ERROR] Real Value: ", hxspi->Instance->DR);
    goto error;
  }

error:
  /* Restore XSPI command in case of error */
  if (retr != CUSTOM_XSPI_OK)
  {
    CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);
  }

  return retr;
}

/**
  * @brief Gets the Write In Progress (WIP) bit from the memory device.
  * @param hxspi Pointer to the HAL XSPI handle.
  * @param sCommand Pointer to the command descriptor to use.
  * @param NorConfig Pointer to the Nor Flash configuration descriptor.
  * @param Timeout Maximum duration allowed for the WIP check to succeed (no Write In Progress).
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_GetWriteInProgress(XSPI_HandleTypeDef *hxspi,
                                                                XSPI_RegularCmdTypeDef *sCommand,
                                                                NorFlashBasConfigTypeDef *NorConfig,
                                                                uint32_t Timeout)
{
  CUSTOM_XSPI_StatusTypeDef retr  = CUSTOM_XSPI_OK;
  XSPI_AutoPollingTypeDef sConfig = {0};
  uint32_t SavedCommand           = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  /* Initialize command instruction to be sent */
  sCommand->Instruction     = NorConfig->Cmd_RDSR;

  /* Check if Address is to be provided for access Status Register :
     If not provided, no address field to be sent in command.
     If provided, address value present in configuration file will apply */
  if (IS_CUSTOM_SETTING_CFG_DEFINED(NorConfig->SRAccessDetails))
  {
    sCommand->Address       = CUSTOM_SETTING_CFG_VALUE(NorConfig->SRAccessDetails);
  }
  else
  {
    sCommand->AddressMode   = HAL_XSPI_ADDRESS_NONE;
  }

  if (sCommand->DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
  {
    sCommand->DataLength    = 2;
  }
  else
  {
    sCommand->DataLength    = 1;
  }

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] RDSR Command:  ", sCommand->Instruction);
    retr = CUSTOM_XSPI_KO;
  }

  /* Reconfigure XSPI to automatic polling mode to wait for write enabling */
  sConfig.MatchMode         = HAL_XSPI_MATCH_MODE_AND;
  sConfig.AutomaticStop     = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  sConfig.IntervalTime      = 0x10;
  sConfig.MatchMask         = NorConfig->MatchMask_WIP;
  sConfig.MatchValue        = NorConfig->MatchValue_WIP;

  if (HAL_XSPI_AutoPolling(hxspi, &sConfig, Timeout) != HAL_OK)
  {
    DEBUG_STR("[ERROR] WIP Autopolling: ");
    DEBUG_STR_HEX("[ERROR] WIP MatchMask:  ", NorConfig->MatchMask_WIP);
    DEBUG_STR_HEX("[ERROR] WIP MatchValue: ", NorConfig->MatchValue_WIP);
    DEBUG_STR_HEX("[ERROR] Real Value: ", hxspi->Instance->DR);
    retr = CUSTOM_XSPI_KO;
  }

  DEBUG_STR_HEX("\t[WIP] Command:  ", sCommand->Instruction);

  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief Retrieves the JEDEC ID from the memory device.
  * @param CustomObject Pointer to the custom driver object.
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_GetJedecID(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_RegularCmdTypeDef *sCommand = &CustomObject->Private.Commandbase;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  uint32_t SavedCommand            = 0;

  /* Save current XSPI command */
  CUSTOM_XSPI_SaveCommand(sCommand, &SavedCommand);

  /* Initialize command instruction to be sent */
  sCommand->OperationType     = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand->Instruction       = EXTMEM_CUSTOM_JEDECID_CMD;
  sCommand->AddressMode       = HAL_XSPI_ADDRESS_NONE;
  sCommand->DataLength        = EXTMEM_CUSTOM_JEDECID_SIZE;
  sCommand->DummyCycles       = 0;

  if (HAL_XSPI_Command(hxspi, sCommand, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] JEDEC ID Command:  ", sCommand->Instruction);
    retr = CUSTOM_XSPI_KO;
  }

  if (HAL_XSPI_Receive(hxspi, (uint8_t *) &CustomObject->Private.JedecID, CUSTOM_XSPI_TIMEOUT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
  }

  DEBUG_STR_HEX("[JEDEC ID] Manufacturer ID :", CustomObject->Private.JedecID.ManufacturerID);
  DEBUG_STR_HEX("[JEDEC ID] Memory type     :", CustomObject->Private.JedecID.MemoryType);
  DEBUG_STR_HEX("[JEDEC ID] Memory density  :", CustomObject->Private.JedecID.MemoryDensity);

  /* Restore XSPI command previously saved */
  CUSTOM_XSPI_RestoreCommand(sCommand, &SavedCommand);

  return retr;
}

/**
  * @brief This function configures a Command descriptor according to Configuration object
  * @param ConfigObject Configuration object
  * @param sCommand Command descriptor to be initialised
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_SetCommand(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  XSPI_HyperbusCfgTypeDef sHyperbusCfg;

  /* Configure the Hyperbus to transmit */
  sHyperbusCfg.RWRecoveryTimeCycle = CustomObject->HyperRamConfig.HyperbusConfig.RwRecoveryTimeCycle;
  sHyperbusCfg.AccessTimeCycle     = CustomObject->HyperRamConfig.HyperbusConfig.AccessTimeCycle;
  sHyperbusCfg.WriteZeroLatency    = CustomObject->HyperRamConfig.HyperbusConfig.WriteZeroLatency;
  sHyperbusCfg.LatencyMode         = CustomObject->HyperRamConfig.HyperbusConfig.LatencyMode;

  if (HAL_XSPI_HyperbusCfg(hxspi, &sHyperbusCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return CUSTOM_XSPI_KO;
  }

  return CUSTOM_XSPI_OK;
}

/**
  * @brief This function configures a Command descriptor according to Configuration object
  * @param ConfigObject Configuration object
  * @param sCommand Command descriptor to be initialised
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_ReadRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                   OptionalRegisterConfigTypeDef *RegisterConfig)
{
  CUSTOM_XSPI_StatusTypeDef retr  = CUSTOM_XSPI_OK;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  XSPI_HyperbusCmdTypeDef *sCommand   = &CustomObject->Private.CommandbaseHyperbus;
  uint16_t readData[1] = {0};

  /* Initialize the transmit command */
  sCommand->AddressSpace = HAL_XSPI_REGISTER_ADDRESS_SPACE;   /* CA[46] = 1 */
  sCommand->AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
  sCommand->Address      = RegisterConfig->RWConfigStep.RegisterAddress;
  sCommand->DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand->DataMode     = HAL_XSPI_DATA_8_LINES;
  sCommand->DataLength   = 2U;

  if (HAL_XSPI_HyperbusCmd(hxspi, sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return CUSTOM_XSPI_KO;
  }

  if (HAL_XSPI_Receive(hxspi, (uint8_t *)readData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Read Register Value:  ", readData[0]);
    return CUSTOM_XSPI_KO;
  }

  /* Can be used for debug */
  CustomObject->Private.ReadRegValue = readData[0];
  DEBUG_STR_HEX("[READ REG        ] Value:   ", CustomObject->Private.ReadRegValue);

  return retr;
}

/**
  * @brief This function configures a Command descriptor according to Configuration object
  * @param ConfigObject Configuration object
  * @param sCommand Command descriptor to be initialised
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_WriteRegister(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                    OptionalRegisterConfigTypeDef *RegisterConfig)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  XSPI_HyperbusCmdTypeDef *sCommand   = &CustomObject->Private.CommandbaseHyperbus;
  uint16_t writeData[1] = {0};
  uint16_t mask;

  /* 16 bits register content */
  mask = (0xFFFFU - ((RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFU));
  writeData[0] = ((RegisterConfig->RWConfigStep.RegisterValue & RegisterConfig->RWConfigStep.RegisterMask) & 0xFFFFU)
                 | (CustomObject->Private.ReadRegValue & mask);

  /* Enable the Write Zero Latency bit for Write register */
  SET_BIT(hxspi->Instance->HLCR, XSPI_HLCR_WZL);

  /* Initialize the transmit command */
  sCommand->AddressSpace  = HAL_XSPI_REGISTER_ADDRESS_SPACE;   /* CA[46] = 1 */
  sCommand->AddressWidth  = HAL_XSPI_ADDRESS_32_BITS;
  sCommand->Address       = RegisterConfig->RWConfigStep.RegisterAddress;
  sCommand->DQSMode       = CustomObject->StartupConfig.DqsMode;
  sCommand->DataMode      = HAL_XSPI_DATA_8_LINES;
  sCommand->DataLength    = 2U;

  if (HAL_XSPI_HyperbusCmd(hxspi, sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return CUSTOM_XSPI_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, (uint8_t *) writeData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    DEBUG_STR_HEX("[ERROR] Write Register Value:  ", writeData[0]);
    return CUSTOM_XSPI_KO;
  }

  DEBUG_STR_HEX("[WRITE REG]   Value:  ", writeData[0]);
  return retr;
}


/**
  * @brief Retrieves the JEDEC ID from the memory device.
  * @param CustomObject Pointer to the custom driver object.
  * @retval @ref CUSTOM_XSPI_StatusTypeDef
  */
static CUSTOM_XSPI_StatusTypeDef CUSTOM_XSPI_HYPERBUS_GetDeviceID(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject)
{
  CUSTOM_XSPI_StatusTypeDef retr   = CUSTOM_XSPI_OK;
  XSPI_HandleTypeDef *hxspi        = CustomObject->Private.Handle;
  XSPI_HyperbusCmdTypeDef *sCommand   = &CustomObject->Private.CommandbaseHyperbus;
  uint8_t IdRegister0[2] = {0};

  /* Initialize the transmit command */
  sCommand->AddressSpace = HAL_XSPI_REGISTER_ADDRESS_SPACE;   /* CA[46] = 1 */
  sCommand->AddressWidth = CustomObject->StartupConfig.AddressSize;
  sCommand->Address      = 0x0000;                            /* address configuration register 0 */
  sCommand->DQSMode      = CustomObject->StartupConfig.DqsMode;
  sCommand->DataMode     = HAL_XSPI_DATA_8_LINES;
  sCommand->DataLength   = 2U;

  if (HAL_XSPI_HyperbusCmd(hxspi, sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    retr = CUSTOM_XSPI_KO;
  }

  if (HAL_XSPI_Receive(hxspi, IdRegister0, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return CUSTOM_XSPI_KO;
  }

  CustomObject->Private.DeviceID.RowAddrBitCount    = (IdRegister0[1] & 0x0F);
  CustomObject->Private.DeviceID.ColumnAddrBitCount = (IdRegister0[0] & 0xF0);
  CustomObject->Private.DeviceID.Manufacturer       = (IdRegister0[0] & 0x0F);

  DEBUG_STR_HEX("[DEVICE ID] Row Addr Bit    :", CustomObject->Private.DeviceID.RowAddrBitCount);
  DEBUG_STR_HEX("[DEVICE ID] Column Addr Bit :", CustomObject->Private.DeviceID.ColumnAddrBitCount);
  DEBUG_STR_HEX("[DEVICE ID] Manufacturer    :", CustomObject->Private.DeviceID.Manufacturer);

  return retr;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
