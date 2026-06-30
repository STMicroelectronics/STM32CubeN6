/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "app_threadx.h"
#include "stm32n6570_discovery.h"
#include "stm32n6570_discovery_xspi.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define TRACE_MAIN(...) printf(__VA_ARGS__)
/* Private variables ---------------------------------------------------------*/
ETH_DMADescTypeDef  DMARxDscrTab[ETH_DMA_RX_CH_CNT][ETH_RX_DESC_CNT] __NON_CACHEABLE; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_DMA_TX_CH_CNT][ETH_TX_DESC_CNT] __NON_CACHEABLE; /* Ethernet Tx DMA Descriptors */
ETH_TxPacketConfig  TxConfig;
ETH_HandleTypeDef   heth;

/* Private function prototypes -----------------------------------------------*/
static void MX_ETH_Init(void);
static void MPU_Config(void);
static void LOG_Config(void);
static void RISAF_Config(void);
static void XSPI1_Source_Init(void);
static void MX_GPDMA1_Init(void);

__weak void SystemClock_Config(void)
{
  Error_Handler();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MPU Configuration */
  MPU_Config();
  
  /* Enable I-Cache */
  SCB_EnableICache();
  
  /* Enable D-Cache */
  SCB_EnableDCache();

  /* System clock already configured, simply SystemCoreClock init */
  SystemCoreClockUpdate();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
 
  /* Clocks configuration*/
  SystemClock_Config();
 
  /* initialize LEDs to signal processing is ongoing */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  /* Security configuration */
  RISAF_Config();

  /* UART Config */
  LOG_Config();

  /*  External RAM  Init */
  XSPI1_Source_Init();
  BSP_XSPI_RAM_Init(0);
  BSP_XSPI_RAM_EnableMemoryMappedMode(0);
  
  /* Ethernet confi*/
  MX_ETH_Init();

  /* GPDMA init*/
  MX_GPDMA1_Init();

  TRACE_MAIN("VENC_RTSP_Server\n");
  TRACE_MAIN("CPU frequency    : %lu MHz\n", (unsigned long)(HAL_RCC_GetCpuClockFreq() / 1000000UL));
  TRACE_MAIN("sysclk frequency : %lu MHz\n", (unsigned long)(HAL_RCC_GetSysClockFreq() / 1000000UL));

  /* Start OS*/
  MX_ThreadX_Init();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief XSPI clock config
  */
static void XSPI1_Source_Init(void)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInit;
  
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
  PeriphClkInit.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  return;
}

/**
  * @brief UART Initialization Function
  */
static void LOG_Config(void)
{
  COM_InitTypeDef COM_Init;

  /* Initialize COM init structure */
  COM_Init.BaudRate   = 115200;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.StopBits   = COM_STOPBITS_1;
  COM_Init.Parity     = COM_PARITY_NONE;
  COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;

  BSP_COM_Init(COM1, &COM_Init);

  if (BSP_COM_SelectLogPort(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
}

void H264EncTrace(const char *pMsg)
{
  printf("VENC[%s]\n", pMsg);
}

/**
  * @brief GPDMA Initialization Function
  */
static void MX_GPDMA1_Init(void)
{
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
}

/**
  * @brief ETH Initialization Function
  */
static void MX_ETH_Init(void)
{
  static uint8_t MACAddr[6];

  heth.Instance = ETH1;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE0;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x10;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RGMII_MODE;

  for (int ch = 0; ch < ETH_DMA_CH_CNT; ch++)
  {
    heth.Init.TxDesc[ch] = DMATxDscrTab[ch];
    heth.Init.RxDesc[ch] = DMARxDscrTab[ch];
  }

  heth.Init.RxBuffLen = 1536;

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
}


/**
* @brief  RISAF Configuration.
* @retval None
*/
static void RISAF_Config(void)
{
 __HAL_RCC_SYSCFG_CLK_ENABLE();

   /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_ETH1, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1 , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2 , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_VENC  , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D  , &RIMC_master);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_ETH1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_CSI    , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_VENC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);

  /* AXISRAM1 full secure */
  RISAF2_S->REG[0].STARTR  = 0x0;
  RISAF2_S->REG[0].ENDR    = 0x000FFFFF;  /* Al SRAM1 in base region (1Mbytes) */
  RISAF2_S->REG[0].CIDCFGR = 0x00030003;  /* RW by DMA (CID 0) and CPU (CID 1) */
  RISAF2_S->REG[0].CFGR    = 0x101;       /* Base region set with SEC attribute */

  RISAF3_S->REG[0].STARTR  = 0x0;
  RISAF3_S->REG[0].ENDR    = 0x000FFFFF;  /* Al SRAM1 in base region (1Mbytes) */
  RISAF3_S->REG[0].CIDCFGR = 0x00030003;  /* RW by DMA (CID 0) and CPU (CID 1) */
  RISAF3_S->REG[0].CFGR    = 0x101;       /* Base region set with SEC attribute */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
}

/**
  * @brief  Check MPU region setting before configuring
  */
static void MPU_CheckAndConfig( MPU_Region_InitTypeDef * region_config)
{
  /* Check region consistency before config*/
  if (region_config->BaseAddress == 0  || region_config->LimitAddress <= region_config->BaseAddress)
  {
    Error_Handler();
  }
  HAL_MPU_ConfigRegion(region_config);
} 

/* Get regions boundaries (exported from link files)*/
#if defined(__ARMCC_VERSION)
extern uint32_t Image$$ER_IROM1$$Base;
extern uint32_t Image$$RW_IRAM1$$Base;
extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$Base;
  extern uint32_t Image$$NOCACHE_REGION_END_MARKER$$ZI$$Limit;

#define RO_REGION_START       ((uint32_t)&Image$$ER_IROM1$$Base)
  #define RO_REGION_END         ((uint32_t)&Image$$RW_IRAM1$$Base)
#define RW_REGION_START       ((uint32_t)&Image$$RW_IRAM1$$Base)
  #define RW_REGION_END         ((uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Base)
#define NOCACHE_REGION_START  ((uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Base)
  #define NOCACHE_REGION_END    ((uint32_t)&Image$$NOCACHE_REGION_END_MARKER$$ZI$$Limit)
#define PSRAM_REGION_START    (0x90000000U)
#define PSRAM_REGION_END      (0x92000000U)
#else
extern int __ro_region_start__;
extern int __ro_region_end__;

extern int __rw_region_start__;
extern int __rw_region_end__;

extern int __nocache_region_start__;
extern int __nocache_region_end__;

extern int __psram_region_start__;
extern int __psram_region_end__;

#define RO_REGION_START       ((uint32_t)&__ro_region_start__)
#define RO_REGION_END         ((uint32_t)&__ro_region_end__)
#define RW_REGION_START       ((uint32_t)&__rw_region_start__)
#define RW_REGION_END         ((uint32_t)&__rw_region_end__)
#define NOCACHE_REGION_START  ((uint32_t)&__nocache_region_start__)
#define NOCACHE_REGION_END    ((uint32_t)&__nocache_region_end__)
#define PSRAM_REGION_START    ((uint32_t)&__psram_region_start__)
#define PSRAM_REGION_END      ((uint32_t)&__psram_region_end__)
#endif


/**
  * @brief  Configure  MPU regions
  */
static void MPU_Config(void)
{
  uint32_t primask_bit = __get_PRIMASK();
  MPU_Region_InitTypeDef mpu_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t region_number = MPU_REGION_NUMBER0;
  __disable_irq();

  /* disable the MPU */
  HAL_MPU_Disable();

  /* create an attribute configuration for the MPU */
  attr_config.Number = MPU_ATTRIBUTES_NUMBER0;
  attr_config.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  attr_config.Number = MPU_ATTRIBUTES_NUMBER1;
  attr_config.Attributes = INNER_OUTER( MPU_WRITE_THROUGH|MPU_NON_TRANSIENT|MPU_RW_ALLOCATE);
  HAL_MPU_ConfigMemoryAttributes(&attr_config);

#define UNCACHED_ATTRIBUTE MPU_ATTRIBUTES_NUMBER0
#define CACHED_ATTRIBUTE   MPU_ATTRIBUTES_NUMBER1
  
  /* Define RO */
  mpu_config.Enable           = MPU_REGION_ENABLE;
  mpu_config.Number           = region_number++;
  mpu_config.AttributesIndex  = CACHED_ATTRIBUTE; /*Cached*/
  mpu_config.BaseAddress      = RO_REGION_START;
  mpu_config.LimitAddress     = RO_REGION_END - 1;
  mpu_config.AccessPermission = MPU_REGION_ALL_RO;
  mpu_config.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  mpu_config.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_CheckAndConfig(&mpu_config);

  /* Define Not Cacheable area */
  mpu_config.Enable           = MPU_REGION_ENABLE;
  mpu_config.Number           = region_number++;
  mpu_config.AttributesIndex  = UNCACHED_ATTRIBUTE;  /*uncached*/
  mpu_config.BaseAddress      = NOCACHE_REGION_START;
  mpu_config.LimitAddress     = NOCACHE_REGION_END - 1;
  mpu_config.AccessPermission = MPU_REGION_ALL_RW;
  mpu_config.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  mpu_config.IsShareable      = MPU_ACCESS_INNER_SHAREABLE | MPU_ACCESS_OUTER_SHAREABLE;
  MPU_CheckAndConfig(&mpu_config);

  /* Define RW */
  mpu_config.Enable           = MPU_REGION_ENABLE;
  mpu_config.Number           = region_number++;
  mpu_config.AttributesIndex  = CACHED_ATTRIBUTE;  /*Cached*/
  mpu_config.BaseAddress      = RW_REGION_START;
  mpu_config.LimitAddress     = RW_REGION_END - 1;
  mpu_config.AccessPermission = MPU_REGION_ALL_RW;
  mpu_config.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  mpu_config.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_CheckAndConfig(&mpu_config);
 
  /* Define external RAM Cacheable area */
  mpu_config.Enable           = MPU_REGION_ENABLE;
  mpu_config.Number           = region_number++;
  mpu_config.AttributesIndex  = UNCACHED_ATTRIBUTE;  /*Uncached*/
  mpu_config.BaseAddress      = PSRAM_REGION_START;
  mpu_config.LimitAddress     = PSRAM_REGION_END - 1;
  mpu_config.AccessPermission = MPU_REGION_ALL_RW;
  mpu_config.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  mpu_config.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_CheckAndConfig(&mpu_config);
 
  /* enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  __enable_irq();
  /* Exit critical section to lock the system and avoid any issue around MPU mechanism */
  __set_PRIMASK(primask_bit);
}

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  BSP_LED_Off(LED_GREEN);
  while (1)
  {
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(250);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  TRACE_MAIN("assert failed at line %d of file %s\n", line, file);
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */
