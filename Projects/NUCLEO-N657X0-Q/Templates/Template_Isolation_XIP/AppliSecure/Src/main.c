/**
  ******************************************************************************
  * @file    main.c
  * @author  GPM Application Team
  * @brief   Secure main program.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6xx_it.h"

/** @addtogroup STM32N6xx_HAL_Template
  * @{
  */
/** @addtogroup HAL
  * @{
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Non-secure Vector table to jump to                                         */
/* Caution: address must correspond to non-secure address as it is mapped in  */
/*          the non-secure vector table                                       */

/* Memory partitioning on STM32N6xx uses */
/* External NOR flash for Secure and Non-Secure code */
/* Secure code is stored from 0x7010'0000 to 0x7017ffff */
/* Non-Secure code is stored from 0x7018'0000 to 0x701fffff */
/* - AXI SRAM1 for Secure data */
/* - AXI SRAM2 for Non-secure data */

#define VECT_TAB_NS_OFFSET  0x00180400
#define VTOR_TABLE_NS_START_ADDR (XSPI2_BASE|VECT_TAB_NS_OFFSET)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemIsolation_Config(void);
/* Global variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  funcptr_NS NonSecure_ResetHandler;

  /* Enable BusFault and SecureFault handlers (HardFault is default) */
  SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk);

  /* Reset of all peripherals, Initializes the Flash interface and the systick. */
  HAL_Init();

  /* Secure/Non-secure Memory and Peripheral isolation configuration */
  SystemIsolation_Config();

  /* Enable I-Cache-----------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache-----------------------------------------------------------*/
  SCB_EnableDCache();

  /* Suspend secure tick before jump to non secure */
  HAL_SuspendTick();

  /*************** Setup and jump to non-secure *******************************/

  /* Set non-secure vector table location */
  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();

  /* Non-secure software does not return, this code is not executed */
  while (1) {
    __NOP();
  }
}

/**
  * @brief  System Isolation Configuration
  *         This function is responsible for Memory and Peripheral isolation
  *         for secure and non-secure application parts
  *         The configuration for this project is :
  *         RISAF7 (FLEXRAM) Secure
  *         RISAF2 (AXISRAM1) Secure
  *         RISAF3 (AXISRAM2) Non-Secure
  *         RISAF12 (XSPI2) up to 0x7017FFFF Secure
  *         RISAF12 (XSPI2) from 0x70180000 to 0x7380000 Non-Secure
  *
  *         RIFSC for GPIOO : Non-Secure
  *         GPIOO pin 1 set as Non-Secure
  * @retval None
  */
static void SystemIsolation_Config(void)
{
  RISAF_BaseRegionConfig_t risaf_conf;
  /* RISAF Config */
  __HAL_RCC_RISAF_CLK_ENABLE();

  /* set up base region configuration for AXISRAM1 and 2 */
  risaf_conf.StartAddress = 0;

  risaf_conf.Filtering = RISAF_FILTER_ENABLE;
  risaf_conf.PrivWhitelist = RIF_CID_MASK;
  risaf_conf.ReadWhitelist = RIF_CID_MASK;
  risaf_conf.WriteWhitelist = RIF_CID_MASK;

  /* FLEXRAM is secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  risaf_conf.EndAddress = 0x00063FFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF7, RISAF_REGION_1, &risaf_conf);

  /* AXISRAM1 is secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  risaf_conf.EndAddress = 0x0009BFFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF2, RISAF_REGION_1, &risaf_conf);

  /* AXISRAM2 is non-secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_NSEC;
  risaf_conf.EndAddress = 0x000FFFFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF3, RISAF_REGION_1, &risaf_conf);

  /* set up RISAF ROM regions for XSPI2 */
  risaf_conf.StartAddress = 0x00100000;
  risaf_conf.EndAddress = 0x0017FFFF;
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF12, RISAF_REGION_1, &risaf_conf);

  risaf_conf.StartAddress = 0x00180000;
  risaf_conf.EndAddress = 0x001FFFFF;
  risaf_conf.Secure = RIF_ATTRIBUTE_NSEC;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF12, RISAF_REGION_2, &risaf_conf);

  __HAL_RCC_RIFSC_CLK_ENABLE();
  /* Set GPIOG as configurable by non-secure */
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RCC_PERIPH_INDEX_GPIOG, RIF_ATTRIBUTE_NSEC);
  /* Enable GPIOG clock and ensure GPIOG bank is powered on */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  /* Configure PG8 as non-secure to be used for non-secure led toggling */
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_8, GPIO_PIN_NSEC);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(file);
  UNUSED(line);

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */