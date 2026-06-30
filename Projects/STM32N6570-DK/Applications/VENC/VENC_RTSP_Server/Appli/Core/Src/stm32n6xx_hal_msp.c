/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32n6xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */
/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */
/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief ETH MSP Initialization
* This function configures the hardware resources used in this example
* @param heth: ETH handle pointer
* @retval None
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  GPIO_InitTypeDef GPIO_Init;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(heth);
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Peripheral clock enable */
  __HAL_RCC_ETH1MAC_CLK_ENABLE();
  __HAL_RCC_ETH1TX_CLK_ENABLE();
  __HAL_RCC_ETH1RX_CLK_ENABLE();

  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

    /**ETH GPIO Configuration
    PD1      ------> ETH1_MDC
    PD12     ------> ETH1_MDIO
    PD3      ------> ETH1_PHY_INTN
    PF10     ------> ETH1_RGMII_RX_CTL
    PF7      ------> ETH1_RGMII_RX_CLK
    PF5      ------> ETH1_CLK
    PF15     ------> ETH1_RGMII_RXD1
    PF14     ------> ETH1_RGMII_RXD0
    PF8      ------> ETH1_RGMII_RXD2
    PF2      ------> ETH1_RGMII_CLK125
    PF9      ------> ETH1_RGMII_RXD3
    PG4      ------> ETH1_RGMII_TXD3
    PF11     ------> ETH1_RGMII_TX_CTL
    PG3      ------> ETH1_RGMII_TXD2
    PF13     ------> ETH1_RGMII_TXD1
    PF0      ------> ETH1_RGMII_GTX_CLK
    PF12     ------> ETH1_RGMII_TXD0
    */

  /* Configure PD1/PD3/PD12 as ETH1 AF11. */
  GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_Init.Mode = GPIO_MODE_AF_PP;
  GPIO_Init.Pull = GPIO_NOPULL;
  GPIO_Init.Alternate = GPIO_AF11_ETH1;
  GPIO_Init.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOD, &GPIO_Init);

  /* Configure PF2/PF5/PF7-PF15 as ETH1 AF11. */
  GPIO_Init.Pin = GPIO_PIN_2  | GPIO_PIN_7  | GPIO_PIN_5  |
                  GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &GPIO_Init);

  /* Configure PG3/PG4 as ETH1 AF11. */
  GPIO_Init.Pin = GPIO_PIN_3 | GPIO_PIN_4;
  HAL_GPIO_Init(GPIOG, &GPIO_Init);

  /* Configure PF0 as ETH1 AF12. */
  GPIO_Init.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_Init.Pin =  GPIO_PIN_0;
  GPIO_Init.Alternate = GPIO_AF12_ETH1;
  HAL_GPIO_Init(GPIOF, &GPIO_Init);

  HAL_NVIC_SetPriority(ETH1_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(ETH1_IRQn);
  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
}

/**
* @brief ETH MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param heth: ETH handle pointer
* @retval None
*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(heth);

  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
  __HAL_RCC_ETH1MAC_CLK_DISABLE();
  __HAL_RCC_ETH1TX_CLK_DISABLE();
  __HAL_RCC_ETH1RX_CLK_DISABLE();

    /**ETH GPIO Configuration
    PD1      ------> ETH1_MDC
    PD12     ------> ETH1_MDIO
    PD3      ------> ETH1_PHY_INTN
    PF10     ------> ETH1_RGMII_RX_CTL
    PF7      ------> ETH1_RGMII_RX_CLK
    PF5      ------> ETH1_CLK
    PF15     ------> ETH1_RGMII_RXD1
    PF14     ------> ETH1_RGMII_RXD0
    PF8      ------> ETH1_RGMII_RXD2
    PF2      ------> ETH1_RGMII_CLK125
    PF9      ------> ETH1_RGMII_RXD3
    PG4      ------> ETH1_RGMII_TXD3
    PF11     ------> ETH1_RGMII_TX_CTL
    PG3      ------> ETH1_RGMII_TXD2
    PF13     ------> ETH1_RGMII_TXD1
    PF0      ------> ETH1_RGMII_GTX_CLK
    PF12     ------> ETH1_RGMII_TXD0
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0  | GPIO_PIN_2  | GPIO_PIN_5  | GPIO_PIN_7 |
                           GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_3 | GPIO_PIN_4);

    /* ETH1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(ETH1_IRQn);
  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */
    /* Reset peripherals */
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_6);

    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
