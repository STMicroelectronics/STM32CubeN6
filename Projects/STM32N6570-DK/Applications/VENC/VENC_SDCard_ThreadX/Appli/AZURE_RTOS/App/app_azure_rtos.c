/* USER CODE BEGIN Header */
/**
******************************************************************************
  * @file           : app_azure_rtos.c
  * @brief          : app_azure_rtos application implementation file
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

#include "app_azure_rtos.h"
#include "stm32n6xx.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "venc_app.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __ALIGN_END;
TX_BYTE_POOL tx_app_byte_pool;

TX_THREAD venc_thread;
TX_THREAD sdcard_thread;

/* Private function prototypes -----------------------------------------------*/

void     Error_Handler(void);
__weak void venc_thread_func(ULONG arg)
{
  while(1);
}

__weak void sdcard_thread_func(ULONG arg)
{
  while(1);
}

__weak void monitor_thread_create(void)
{
  return;
}

/**
* @brief  Define the initial system.
* @param  first_unused_memory : Pointer to the first unused memory
* @retval None
*/
VOID tx_application_define(VOID *first_unused_memory)
{
  UINT status = TX_SUCCESS;
  
  if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    Error_Handler();
    
  }
  else
  {
    monitor_thread_create();

    void *thread_stack_pointer;
    if(tx_byte_allocate(&tx_app_byte_pool, &thread_stack_pointer, 8000, TX_NO_WAIT) != TX_SUCCESS){
      Error_Handler();
    }
    /* Start the VENC Thread */
    status = tx_thread_create(&venc_thread, "VENC App Thread", venc_thread_func, 0,
                              thread_stack_pointer, 8000, 12, 12, TX_NO_TIME_SLICE, TX_AUTO_START);
    if(status != TX_SUCCESS)
    {
      Error_Handler();
    }
    
    if(tx_byte_allocate(&tx_app_byte_pool, &thread_stack_pointer, 4000, TX_NO_WAIT) != TX_SUCCESS){
      Error_Handler();
    }

    /* Start the SDCard thread. */
    status = tx_thread_create(&sdcard_thread, "SDCard App Thread", sdcard_thread_func, 0,
                              thread_stack_pointer, 4000, 12, 12, TX_NO_TIME_SLICE, TX_AUTO_START);
    if(status != TX_SUCCESS)
    {
      Error_Handler();
    }

  }  
}
