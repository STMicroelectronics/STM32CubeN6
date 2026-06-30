/**
  ******************************************************************************
  * @file    stm32_boot_lrun.c
  * @author  MCD Application Team
  * @brief   This file manages booting in load and run mode.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_boot_lrun.h"

/** @defgroup BOOT
  * @{
  */

/** @defgroup BOOT_LRUN
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* offset of the vector table from the start of the image. Should be set in extmem_conf.h if needed  */
#ifndef EXTMEM_HEADER_OFFSET
#define EXTMEM_HEADER_OFFSET 0
#endif /* EXTMEM_HEADER_OFFSET */

#if defined(EXTMEM_LRUN_TS_ENABLE_NS) \
    && (!defined(EXTMEM_LRUN_DESTINATION_ADDRESS_NS) || !defined(EXTMEM_LRUN_SOURCE_ADDRESS_NS))
#error "ExtMem user configuration incorrect : undefined parameters for Non-Secure image loading"
#endif /* EXTMEM_LRUN_TS_ENABLE_NS) && (!EXTMEM_LRUN_DESTINATION_ADDRESS_NS || !EXTMEM_LRUN_SOURCE_ADDRESS_NS) */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
BOOTStatus_TypeDef MapMemory(void);
BOOTStatus_TypeDef CopyApplication(void);
BOOTStatus_TypeDef JumpToApplication(void);
BOOTStatus_TypeDef GetBaseAddress(uint32_t MemIndex, uint32_t *BaseAddress);

/**
  *  @addtogroup BOOT_LRUN_Exported_Functions Boot LRUN exported functions
  * @{
  */

/**
  * @brief Boots the application by mapping memories, loading code, and jumping to the application.
  * @retval BOOTStatus_TypeDef Status of the operation.
  */
BOOTStatus_TypeDef BOOT_Application(void)
{
  BOOTStatus_TypeDef retr;

  /* Mount the memory */
  retr = MapMemory();
  if (BOOT_OK == retr)
  {
#ifndef EXTMEM_DEBUG_NO_LOAD_FROM_EXT_FLASH
    retr = CopyApplication();
#else
#warning "Application loading from external Flash has been disabled (SRAM expected to be loaded by the debugger)"
#endif /* EXTMEM_DEBUG_NO_LOAD_FROM_EXT_FLASH */

    if (BOOT_OK == retr)
    {
      /* Jump on the application */
      retr = JumpToApplication();
    }
  }
  return retr;
}

/**
  * @}
  */

/**
  *  @defgroup BOOT_LRUN_Private_Functions Boot LRUN private functions
  * @{
  */

/**
  * @brief  Maps the external memory.
  * @retval BOOTStatus_TypeDef Status of the operation.
  */
BOOTStatus_TypeDef MapMemory(void)
{
  BOOTStatus_TypeDef retr = BOOT_OK;
  uint32_t BaseAddress = 0;

  /* Map all the memory */
  for (uint8_t index = 0; index < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)); index++)
  {
    switch (EXTMEM_GetMapAddress(index, &BaseAddress))
    {
      case EXTMEM_OK :
      {
        if (EXTMEM_MemoryMappedMode(index, EXTMEM_ENABLE) != EXTMEM_OK)
        {
          retr = BOOT_ERROR_MAPPEDMODEFAIL;
        }
        break;
      }
      case EXTMEM_ERROR_NOTSUPPORTED :
      {
        /* Memory doesn't support map mode, nothing to do */
        break;
      }
      default :
      {
        retr = BOOT_ERROR_NOBASEADDRESS;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief  Copies the application data from source to destination.
  * @retval BOOTStatus_TypeDef Status of the operation.
  */
BOOTStatus_TypeDef CopyApplication(void)
{
  BOOTStatus_TypeDef retr = BOOT_OK;
  uint8_t *source;
  uint8_t *destination;
  uint32_t MapAddress;
  uint32_t img_size;

#if defined(EXTMEM_LRUN_DESTINATION_INTERNAL)
  /* This case correspond to copy the SW from external memory into internal memory */
  destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS;
#else
  if (EXTMEM_OK != EXTMEM_GetMapAddress(EXTMEM_LRUN_DESTINATION, &MapAddress))
  {
    return BOOT_ERROR_MAPPEDMODEFAIL;
  }
  destination = (uint8_t *)(MapAddress + EXTMEM_LRUN_DESTINATION_ADDRESS);
#endif /* EXTMEM_LRUN_DESTINATION_INTERNAL */

  /* Get the map address of the source memory */
  switch (EXTMEM_GetMapAddress(EXTMEM_LRUN_SOURCE, &MapAddress))
  {
    case EXTMEM_OK :
    {
      /* Manage the copy in mapped mode */
      source = (uint8_t *)(MapAddress + EXTMEM_LRUN_SOURCE_ADDRESS);
      img_size = BOOT_GetApplicationSize((uint32_t) source);
      /* Copy from source to destination in mapped mode */
      for (uint32_t index = 0; index < img_size; index++)
      {
        destination[index] = source[index];
      }
#if defined(EXTMEM_LRUN_TZ_ENABLE_NS)
      source = (uint8_t *)(MapAddress + EXTMEM_LRUN_SOURCE_ADDRESS_NS);
      img_size = BOOT_GetApplicationSize((uint32_t) source);
      destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS_NS;
      /* Copy Non-Secure from source to destination in mapped mode */
      for (uint32_t index = 0; index < img_size; index++)
      {
        destination[index] = source[index];
      }
#endif /* EXTMEM_LRUN_TZ_ENABLE_NS */
      break;
    }

    case EXTMEM_ERROR_NOTSUPPORTED:
    {
      img_size = BOOT_GetApplicationSize(EXTMEM_LRUN_SOURCE_ADDRESS);
      /* Manage the copy using EXTMEM_Read */
      if (EXTMEM_OK != EXTMEM_Read(EXTMEM_LRUN_SOURCE, EXTMEM_LRUN_SOURCE_ADDRESS, destination, img_size))
      {
        retr = BOOT_ERROR_COPY;
      }
#if defined(EXTMEM_LRUN_TZ_ENABLE_NS)
      img_size = BOOT_GetApplicationSize(EXTMEM_LRUN_SOURCE_ADDRESS_NS);
      destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS_NS;
      /* Copy Non-Secure from source to destination in mapped mode */
      if (EXTMEM_OK != EXTMEM_Read(EXTMEM_LRUN_SOURCE, EXTMEM_LRUN_SOURCE_ADDRESS_NS, destination, img_size))
      {
        retr = BOOT_ERROR_COPY;
      }
#endif /* EXTMEM_LRUN_TZ_ENABLE_NS */
      break;
    }

    default :
    {
      /* Returns an error */
      retr = BOOT_ERROR_MAPPEDMODEFAIL;
      break;
    }
  }
  return retr;
}

/**
  * @brief  Jumps to the application using its vector table.
  * @retval BOOTStatus_TypeDef Status of the operation.
  */
BOOTStatus_TypeDef JumpToApplication(void)
{
  uint32_t primask_bit;
  typedef  void (*pFunction)(void);
  static pFunction JumpToApp;
  uint32_t Application_vector;
  /* Suspend SysTick */
  HAL_SuspendTick();

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
  /* if I-Cache is enabled, disable I-Cache-----------------------------------*/
  if (SCB->CCR & SCB_CCR_IC_Msk)
  {
    SCB_DisableICache();
  }
#endif /* ICACHE_PRESENT */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* if D-Cache is enabled, disable D-Cache-----------------------------------*/
  if (SCB->CCR & SCB_CCR_DC_Msk)
  {
    SCB_DisableDCache();
  }
#endif /* DCACHE_PRESENT */

  /* Initialize user application's Stack Pointer & Jump to user application  */
  primask_bit = __get_PRIMASK();
  __disable_irq();

  Application_vector = BOOT_GetApplicationVectorTable();

  SCB->VTOR = (uint32_t)Application_vector;
  JumpToApp = (pFunction)(*(__IO uint32_t *)(Application_vector + 4));

#if (defined(__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))
  /* on ARM v8m, set MSPLIM before setting MSP to avoid unwanted stack overflow faults */
  __set_MSPLIM(0x00000000);
#elif (defined(__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1))
  /* on ARM v8m, set MSPLIM before setting MSP to avoid unwanted stack overflow faults */
  __set_MSPLIM(0x00000000);
#elif (defined(__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))
  /* on ARM v8m, set MSPLIM before setting MSP to avoid unwanted stack overflow faults */
  __set_MSPLIM(0x00000000);
#endif  /* __ARM_ARCH_8M_MAIN__ or __ARM_ARCH_8_1M_MAIN__ or __ARM_ARCH_8M_BASE__ */

  __set_MSP(*(__IO uint32_t *)Application_vector);

  /* Re-enable the interrupts */
  __set_PRIMASK(primask_bit);

  JumpToApp();
  return BOOT_OK;
}


/**
  * @brief Gets the size of the application image.
  * @param img_addr Address of the application image.
  * @retval Size of the application image in bytes.
  */
__weak uint32_t BOOT_GetApplicationSize(uint32_t img_addr)
{
  UNUSED(img_addr);
  return EXTMEM_LRUN_SOURCE_SIZE;
}

/**
  * @brief Gets the address of the application's vector table.
  * @retval Address of the vector table.
  */
__weak uint32_t BOOT_GetApplicationVectorTable(void)
{
  uint32_t vector_table;
#if defined(EXTMEM_LRUN_DESTINATION_INTERNAL)
  vector_table = EXTMEM_LRUN_DESTINATION_ADDRESS;
#else
  if (EXTMEM_OK != EXTMEM_GetMapAddress(EXTMEM_LRUN_DESTINATION, &vector_table))
  {
    return 0xffffffff;
  }
  vector_table += EXTMEM_LRUN_DESTINATION_ADDRESS;
#endif /* EXTMEM_LRUN_DESTINATION_INTERNAL */
  vector_table += EXTMEM_HEADER_OFFSET;
  return vector_table;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
