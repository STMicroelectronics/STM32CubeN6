/**
  ******************************************************************************
  * @file    stm32n6xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
#include "stm32n6xx_hal.h"
/** @addtogroup STM32N6xx_HAL_Examples
  * @{
  */

/** @defgroup RTC_ActiveTamper
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constant ----------------------------------------------------------*/
/** @defgroup HAL_MSP_Private_Constant
  * @{
  */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
void XSPIM_InitPort1(void);
void XSPIM_InitPort2(void);
void XSPIM_DeInitPort1(void);
void XSPIM_DeInitPort2(void);
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief XSPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hxspi: XSPI handle pointer
  * @retval None
  */
void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  if (hxspi->Instance == XSPI1)
  {
    /* Initializes the peripherals clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
    PeriphClkInitStruct.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_CLKP;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      while(1);
    }

    /* Peripheral clock enable */
    __HAL_RCC_XSPIM_CLK_ENABLE();
    __HAL_RCC_XSPI1_CLK_ENABLE();
    __HAL_RCC_XSPI2_CLK_ENABLE();
    __HAL_RCC_XSPI3_CLK_ENABLE();

    /* XSPI power enable */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    HAL_PWREx_EnableVddIO3();
    HAL_PWREx_ConfigVddIORange(PWR_VDDIO3, PWR_VDDIO_RANGE_1V8);

    /* Initialize the XSPIM port 1 */
    XSPIM_InitPort1();
  }
  else if (hxspi->Instance == XSPI2)
  {
    /*  Select IC3 clock from PLL1 at 50MHz (1200/24) as XSPI2 source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
    PeriphClkInitStruct.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_IC3;
    PeriphClkInitStruct.ICSelection[RCC_IC3].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    PeriphClkInitStruct.ICSelection[RCC_IC3].ClockDivider = 24;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      while(1);
    }

    /* Peripheral clock enable */
    __HAL_RCC_XSPIM_CLK_ENABLE();
    __HAL_RCC_XSPI1_CLK_ENABLE();
    __HAL_RCC_XSPI2_CLK_ENABLE();
    __HAL_RCC_XSPI3_CLK_ENABLE();

    /* XSPI power enable */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO3();
    HAL_PWREx_ConfigVddIORange(PWR_VDDIO3, PWR_VDDIO_RANGE_1V8);

    /* Initialize the XSPIM port 2 */
    XSPIM_InitPort2();
  }
}

/**
  * @brief XSPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @retval None
  */
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
  if (hxspi->Instance == XSPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_XSPI1_CLK_DISABLE();

    XSPIM_DeInitPort1();
  }
  else if (hxspi->Instance == XSPI2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_XSPI2_CLK_DISABLE();

    XSPIM_DeInitPort2();
  }
}

/**
  * @brief Initialize the XSPIM Port1 (XSPI-PHY1)
  * @retval None
  */
void XSPIM_InitPort1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPIOP_CLK_ENABLE();

  /* XSPI2 GPIO Configuration
  PO0     ------> XSPIM_P1_NCS
  PO4     ------> XSPIM_P1_CLK
  PO2     ------> XSPIM_P1_DQS0
  PO3     ------> XSPIM_P1_DQS1
  PP0     ------> XSPIM_P1_IO0
  PP1     ------> XSPIM_P1_IO1
  PP2     ------> XSPIM_P1_IO2
  PP3     ------> XSPIM_P1_IO3
  PP4     ------> XSPIM_P1_IO4
  PP5     ------> XSPIM_P1_IO5
  PP6     ------> XSPIM_P1_IO6
  PP7     ------> XSPIM_P1_IO7
  PP8     ------> XSPIM_P1_IO8
  PP9     ------> XSPIM_P1_IO9
  PP10    ------> XSPIM_P1_IO10
  PP11    ------> XSPIM_P1_IO11
  PP12    ------> XSPIM_P1_IO12
  PP13    ------> XSPIM_P1_IO13
  PP14    ------> XSPIM_P1_IO14
  PP15    ------> XSPIM_P1_IO15
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
  HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
  HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_ALL;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
  HAL_GPIO_Init(GPIOP, &GPIO_InitStruct);
}

/**
  * @brief Initialize the XSPIM Port2 (XSPI-PHY2)
  * @retval None
  */
void XSPIM_InitPort2(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPION_CLK_ENABLE();

  /* XSPI2 GPIO Configuration
  PN1     ------> XSPIM_P2_NCS1
  PN3     ------> XSPIM_P2_IO1
  PN0     ------> XSPIM_P2_DQS0
  PN11    ------> XSPIM_P2_IO7
  PN10    ------> XSPIM_P2_IO6
  PN9     ------> XSPIM_P2_IO5
  PN2     ------> XSPIM_P2_IO0
  PN6     ------> XSPIM_P2_CLK
  PN8     ------> XSPIM_P2_IO4
  PN4     ------> XSPIM_P2_IO2
  PN5     ------> XSPIM_P2_IO3
  */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_0 | GPIO_PIN_11
                      | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_2 | GPIO_PIN_6
                      | GPIO_PIN_8 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
  HAL_GPIO_Init(GPION, &GPIO_InitStruct);
}

/**
  * @brief DeInitialize the XSPIM Port2 (XSPI-PHY2)
  * @retval None
  */
void XSPIM_DeInitPort1(void)
{
  HAL_GPIO_DeInit(GPIOO, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_0
                       | GPIO_PIN_4);

  HAL_GPIO_DeInit(GPIOP, GPIO_PIN_ALL);

  /* Disable GPIO clocks */
  __HAL_RCC_GPIOO_CLK_DISABLE();
  __HAL_RCC_GPIOP_CLK_DISABLE();
}

/**
  * @brief DeInitialize the XSPIM Port1 (XSPI-PHY1)
  * @retval None
  */
void XSPIM_DeInitPort2(void)
{
  HAL_GPIO_DeInit(GPION, GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_0 | GPIO_PIN_11
                       | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_2 | GPIO_PIN_6
                       | GPIO_PIN_8 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12);

  /* Disable GPIO clocks */
  __HAL_RCC_GPION_CLK_DISABLE();
}

/**
  * @}
  */
