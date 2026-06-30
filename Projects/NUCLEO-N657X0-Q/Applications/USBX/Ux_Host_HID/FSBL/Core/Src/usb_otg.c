/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usb_otg.c
  * @brief   This file provides code for the configuration
  *          of the USB_OTG instances.
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
#include "usb_otg.h"

/* USER CODE BEGIN 0 */
/* Set hhcd_USB1_OTG_HS at the end of non-cacheable */
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma location = ".UsbHpcdSection"
#else /* GNU and AC6 Compiler */
__attribute__((section(".UsbHpcdSection")))
#endif
/* USER CODE END 0 */

HCD_HandleTypeDef hhcd_USB_OTG_HS1;

/* USB1_OTG_HS init function */

void MX_USB1_OTG_HS_HCD_Init(void)
{

  /* USER CODE BEGIN USB1_OTG_HS_Init 0 */

  /* USER CODE END USB1_OTG_HS_Init 0 */

  /* USER CODE BEGIN USB1_OTG_HS_Init 1 */

  /* USER CODE END USB1_OTG_HS_Init 1 */
  hhcd_USB_OTG_HS1.Instance = USB1_OTG_HS;
  hhcd_USB_OTG_HS1.Init.dev_endpoints = 9;
  hhcd_USB_OTG_HS1.Init.Host_channels = 16;
  hhcd_USB_OTG_HS1.Init.speed = HCD_SPEED_HIGH;
  hhcd_USB_OTG_HS1.Init.dma_enable = DISABLE;
  hhcd_USB_OTG_HS1.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
  hhcd_USB_OTG_HS1.Init.Sof_enable = DISABLE;
  hhcd_USB_OTG_HS1.Init.low_power_enable = DISABLE;
  hhcd_USB_OTG_HS1.Init.vbus_sensing_enable = DISABLE;
  hhcd_USB_OTG_HS1.Init.use_external_vbus = ENABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_HS1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB1_OTG_HS_Init 2 */

  /* USER CODE END USB1_OTG_HS_Init 2 */

}

void HAL_HCD_MspInit(HCD_HandleTypeDef* hcdHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hcdHandle->Instance==USB1_OTG_HS)
  {
  /* USER CODE BEGIN USB1_OTG_HS_MspInit 0 */

  /* USER CODE END USB1_OTG_HS_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USBOTGHS1;
    PeriphClkInitStruct.UsbPhy1ClockSelection = RCC_USBPHY1CLKSOURCE_HSE_DIV2;
    PeriphClkInitStruct.UsbOtgHs1ClockSelection = RCC_USBOTGHS1CLKSOURCE_HSE_DIRECT;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Enable VDDUSB */
    HAL_PWREx_EnableVddUSB();
    /* USB1_OTG_HS clock enable */
    __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();
    __HAL_RCC_USB1_OTG_HS_PHY_CLK_ENABLE();

    /* USB1_OTG_HS interrupt Init */
    HAL_NVIC_SetPriority(USB1_OTG_HS_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USB1_OTG_HS_IRQn);
  /* USER CODE BEGIN USB1_OTG_HS_MspInit 1 */
    /* Reset USB peripherals and configure the HSE clock */
    RESET_USB_MACRO();
    /* Peripheral clock enable */
    __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

    /* Required few clock cycles before accessing USB PHY Controller Registers */
    HAL_Delay(1);

    USB1_HS_PHYC->USBPHYC_CR &= ~(0x7 << 0x4);

    USB1_HS_PHYC->USBPHYC_CR |= (0x1 << 16) |
                                (0x2 << 4)  |
                                (0x1 << 2)  |
                                 0x1U;

    __HAL_RCC_USB1_OTG_HS_PHY_RELEASE_RESET();

    /* Required few clock cycles before Releasing Reset */
    HAL_Delay(1);

    __HAL_RCC_USB1_OTG_HS_RELEASE_RESET();
  /* USER CODE END USB1_OTG_HS_MspInit 1 */
  }
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hcdHandle)
{

  if(hcdHandle->Instance==USB1_OTG_HS)
  {
  /* USER CODE BEGIN USB1_OTG_HS_MspDeInit 0 */

  /* USER CODE END USB1_OTG_HS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB1_OTG_HS_CLK_DISABLE();
    __HAL_RCC_USB1_OTG_HS_PHY_CLK_DISABLE();

    /* Disable VDDUSB */
      HAL_PWREx_DisableVddUSB();

    /* USB1_OTG_HS interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB1_OTG_HS_IRQn);
  /* USER CODE BEGIN USB1_OTG_HS_MspDeInit 1 */

  /* USER CODE END USB1_OTG_HS_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

