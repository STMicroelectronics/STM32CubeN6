/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file    app_netxduo.c
* @author  MCD Application Team
* @brief   NetXDuo applicative file
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


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nxd_ptp_client.h"
#include "nx_shaper.h"
#include "app_netxduo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
TX_SEMAPHORE   TAS_Semaphore;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static TX_THREAD           NxAppThread;
static TX_THREAD           NxTASThread;
static TX_THREAD           NxUDPClientThread;
static NX_PACKET_POOL      NxAppPool;
static NX_IP               NetXDuoEthIpInstance;
static UINT                interface_1;
static NX_SHAPER           tas_shaper;

static NX_SHAPER_CONTAINER shaper_container;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID   nx_app_thread_entry (ULONG thread_input);
static VOID   nx_app_TAS_entry (ULONG thread_input);
static VOID   nx_app_TAS_Config (VOID);
static VOID   nx_udp_client_thread_entry (ULONG thread_input);
NX_UDP_SOCKET UDPSocket_Q0;
NX_UDP_SOCKET UDPSocket_Q1;
/* USER CODE END PFP */

/**
* @brief  Application NetXDuo Initialization.
* @param memory_ptr: memory pointer
* @retval int
*/
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */

  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN 0 */
  printf("Nx_TimeAwareShaper_Talker application started..\n");
  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  nx_system_initialize();

  /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
  * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
  */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

  /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */

  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */
  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Allocate the memory for TAS thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxTASThread, "NetXDuo TAS thread", nx_app_TAS_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Allocate the memory for the app UDP thread  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the app UDP thread */
  ret = tx_thread_create(&NxUDPClientThread, "NetXDuo UDP App thread", nx_udp_client_thread_entry, 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY + 1 , NX_APP_THREAD_PRIORITY + 1 , TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* Create TAS Semaphore  */
  tx_semaphore_create(&TAS_Semaphore, "TAS Semaphore", 0);

  /* USER CODE END MX_NetXDuo_Init */
  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
  /* USER CODE END Nx_App_Thread_Entry 0 */
  UINT ret = NX_SUCCESS;
  /* USER CODE BEGIN Nx_App_Thread_Entry 1 */

  /* USER CODE END Nx_App_Thread_Entry 1 */

  /* register the IP address change callback */

  /* USER CODE BEGIN Nx_App_Thread_Entry 2 */
  PRINT_IP_ADDRESS(NX_APP_DEFAULT_IP_ADDRESS);

  /* Create a VLAN Interface */
  ret = nx_link_vlan_interface_create(&NetXDuoEthIpInstance, "NetX IP Interface 0:1", NX_APP_SECOND_IP_ADDRESS
                                      , NX_APP_DEFAULT_NET_MASK, VLAN_TAG_1, 0, &interface_1);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  PRINT_IP_ADDRESS(NX_APP_SECOND_IP_ADDRESS);

  printf("\nTime Aware Shaper Feature is Disabled \n");
  printf("Press User Button to enable it \n");

  /* this thread is not needed any more, we relinquish it */
  tx_thread_relinquish();
}
/* USER CODE BEGIN 1 */
/**
* @brief  Time Aware Shaper thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_TAS_entry(ULONG thread_input)
{
  UINT TAS_Enabled = 0;
  while(1)
  {
    if (tx_semaphore_get(&TAS_Semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
    {
      if(TAS_Enabled == 0)
      {
        printf("\nTime Aware Shaper Feature is Enabled \n");
        nx_app_TAS_Config();
        TAS_Enabled = 1;
      }
    }
    tx_thread_relinquish();
  }
}

/**
* @brief  Time Aware Shaper configuration function.
* @param none
* @retval none
*/
static VOID nx_app_TAS_Config()
{
  NX_SHAPER_TAS_CONFIG tas_config;
  UINT ret = NX_SUCCESS;
  UCHAR pcp_list[8];
  UCHAR queue_id_list[8];

  /* Init HW Queue Mapping and configure Time Aware Shaper */

  /* Create a TAS instance */
  ret = nx_shaper_create(&(NetXDuoEthIpInstance.nx_ip_interface[0]), &shaper_container, &tas_shaper, NX_SHAPER_TYPE_TAS, nx_driver_shaper_tas_entry);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Get HW Queue mapping */
  ret = nx_shaper_default_mapping_get(&(NetXDuoEthIpInstance.nx_ip_interface[0]), pcp_list, queue_id_list, 8);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Set HW Queue mapping : VLAN ID 2 mapped to Queue 1 */
  ret = nx_shaper_mapping_set(&(NetXDuoEthIpInstance.nx_ip_interface[0]), pcp_list, queue_id_list, 8);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /*
  Set the TAS Configuration :
  Example :
  Cycle time : 1000000 : 1ms
  Q0: |---------------700000(C)-----------|---300000(O)----|
  Q1: |---------------700000(O)-----------|---300000(C)----|
  */
  tas_config.base_time = 0;
  tas_config.auto_fill_status = NX_SHAPER_TAS_IDLE_CYCLE_AUTO_FILL_DISABLED;
  tas_config.cycle_time = TAS_CYCLE_TIME;
  tas_config.traffic_count = 2;

  tas_config.traffic[0].pcp = 2;
  tas_config.traffic[0].time_offset = 0;
  tas_config.traffic[0].duration = QUEUE_1_SLOT_TIME;
  tas_config.traffic[0].traffic_control = NX_SHAPER_TRAFFIC_OPEN;

  tas_config.traffic[1].pcp = 0;
  tas_config.traffic[1].time_offset = QUEUE_0_SLOT_OFFSET;
  tas_config.traffic[1].duration = QUEUE_0_SLOT_TIME;
  tas_config.traffic[1].traffic_control = NX_SHAPER_TRAFFIC_OPEN;

  printf("Configuration Parameters in Nanoseconds : \n");
  printf(" CYCLE TIME            = %d \n QUEUE 0 Slot Interval = %d \n QUEUE 1 Slot Interval = %d\n",
         TAS_CYCLE_TIME, QUEUE_0_SLOT_TIME, QUEUE_1_SLOT_TIME);

  ret = nx_shaper_tas_parameter_set(&(NetXDuoEthIpInstance.nx_ip_interface[0]), &tas_config);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

}

/**
* @brief  UDP CLient thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_udp_client_thread_entry(ULONG thread_input)
{
  UINT ret;
  NX_PACKET *data_packet_1;
  NX_PACKET *data_packet_2;
  UINT Queue = ETH_QUEUE_0;

  /* create the UDP socket */
  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket_Q0, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, MAX_QUEUE_SIZE);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket_Q1, "UDP Client Socket 2", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, MAX_QUEUE_SIZE);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* bind UDP socket to the DEFAULT PORT */
  ret = nx_udp_socket_bind(&UDPSocket_Q0, UDP_PORT_1, TX_WAIT_FOREVER);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* bind UDP socket to the DEFAULT PORT */
  ret = nx_udp_socket_bind(&UDPSocket_Q1, UDP_PORT_2, TX_WAIT_FOREVER);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  printf("\nUDP transmission started...\n");

  /* 2 sec Delay before starting UDP transmissions */
  HAL_Delay(2000);

  while(1)
  {
    if (Queue == ETH_QUEUE_0)
    {
      /* create the packet to send over the UDP socket */
      ret = nx_packet_allocate(&NxAppPool, &data_packet_1, NX_UDP_PACKET, TX_WAIT_FOREVER);

      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }

      ret = nx_packet_data_append(data_packet_1, (VOID *)DEFAULT_MESSAGE_Q0, sizeof(DEFAULT_MESSAGE_Q0), &NxAppPool, TX_WAIT_FOREVER);

      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }

      /* send the message */
      ret = nx_udp_socket_send(&UDPSocket_Q0, data_packet_1, SERVER_IP_ADDRESS, UDP_PORT_1);

      /* Set the next transmission Queue */
      Queue = ETH_QUEUE_1;
    }
    else
    {
      /* create the packet to send over the UDP socket */
      ret = nx_packet_allocate(&NxAppPool, &data_packet_2, NX_UDP_PACKET, TX_WAIT_FOREVER);

      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }

      ret = nx_packet_data_append(data_packet_2, (VOID *)DEFAULT_MESSAGE_Q1, sizeof(DEFAULT_MESSAGE_Q1), &NxAppPool, TX_WAIT_FOREVER);

      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }

      /* send the message */
      ret = nx_udp_socket_send(&UDPSocket_Q1, data_packet_2, SERVER_IP_SECOND_ADDRESS, UDP_PORT_2);

      /* Set the next transmission Queue */
      Queue = ETH_QUEUE_0;
    }
  }
}
/* USER CODE END 1 */