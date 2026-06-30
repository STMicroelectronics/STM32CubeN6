/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : app_netxduo.h
  * @brief          : Header for app_netxduo.c file.
  *                   NetXDuo applicative header file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* Private includes ----------------------------------------------------------*/
#include "nx_stm32_eth_driver.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern ULONG IpAddress;
/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define PRINT_IP_ADDRESS(addr)             do { \
                                                printf("STM32 %s: %lu.%lu.%lu.%lu \n", #addr, \
                                                (addr >> 24) & 0xff, \
                                                (addr >> 16) & 0xff, \
                                                (addr >> 8) & 0xff, \
                                                addr& 0xff);\
                                           }while(0)

#define PRINT_DATA(addr, port, data)       do { \
                                                printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \n", \
                                                (addr >> 24) & 0xff, \
                                                (addr >> 16) & 0xff, \
                                                (addr >> 8) & 0xff,  \
                                                (addr & 0xff), port, data); \
                                           } while(0)

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_NetXDuo_Init(VOID *memory_ptr);
VOID MX_NetXDuo_Store_Camera_Data(VOID);
VOID MX_NetXDuo_Notify_Video_Frame_Ready(VOID);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PAYLOAD_SIZE             1536
/* Increase packet count for high-throughput RTP: was 16, now 64 */
#define NX_PACKET_POOL_SIZE      (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 64)
#define RTP_PACKET_POOL_SIZE     (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 64)
#define QUEUE_MAX_SIZE           512

#define DEFAULT_MEMORY_SIZE      1024
#define IP_THREAD_PRIORITY       1
#define DEFAULT_PRIORITY         10
#define LINK_PRIORITY            11

#define NULL_ADDRESS             0

#define DEFAULT_PORT             6000

#define NX_APP_CABLE_CONNECTION_CHECK_PERIOD  (6 * NX_IP_PERIODIC_RATE)
/* USER CODE END PD */

/* USER CODE BEGIN 1 */
#define NX_APP_DEFAULT_TIMEOUT               (10 * NX_IP_PERIODIC_RATE)

#define NX_APP_DEFAULT_IP_ADDRESS           IP_ADDRESS(192,168,0,2)

#define NX_APP_DEFAULT_NET_MASK             IP_ADDRESS(255,255,255,0)
/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_NETXDUO_H__ */
