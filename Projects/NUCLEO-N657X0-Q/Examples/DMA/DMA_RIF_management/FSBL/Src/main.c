/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DMA/DMA_RIF_management/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to use a DMA channel
  *          to transfer a word data buffer from FLASH memory to embedded
  *          SRAM memory through the STM32N6xx HAL API.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STEP1 /* OK case: SRAM CID = Perceive CID = DMA_CID              */
// #define STEP2 /* KO case: SRAM CID = DMA_CID (Perceive CID is different) */
// #define STEP3 /* KO case: Perceive CID = DMA_CID (SRAM CID is different) */

#define RISAF3_ADDR_SPACE_SIZE         (0x000FFFFFU)  /* AXI SRAM 2 */
#define RISAF2_ADDR_SPACE_SIZE         (0x000FFFFFU)  /* AXI SRAM 1 */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t protected_address_start = 0x0;
static uint32_t protected_address_end_A = RISAF3_ADDR_SPACE_SIZE;
static uint32_t protected_address_end_B = 0x000FFFFF;

uint32_t dma_transfer_cpltd_counter; /* DMA TX transfer completion flag */

DMA_HandleTypeDef handle_HPDMA1_Channel12;

/* Source buffer in SRAM2 */
uint64_t aSRC_Buffer[BUFFER_SIZE_IN_BYTES/8] =
{
  0x0102030405060708, 0x090A0B0C0D0E0F10,
  0x1112131415161718, 0x191A1B1C1D1E1F20,
  0x2122232425262728, 0x292A2B2C2D2E2F30,
  0x3132333435363738, 0x393A3B3C3D3E3F40,
  0x4142434445464748, 0x494A4B4C4D4E4F50,
  0x5152535455565758, 0x595A5B5C5D5E5F60,
  0x6162636465666768, 0x696A6B6C6D6E6F70,
  0x7172737475767778, 0x797A7B7C7D7E7F80
};

/* Intermediate buffer in SRAM1 */
/* At address 0x340E0000        */
uint64_t *aIntermediate_Buffer =  (uint64_t *)0x340E0000;

/* Final destination buffer in SRAM2 */
uint64_t aDST_Buffer[BUFFER_SIZE_IN_BYTES/8] __NON_CACHEABLE = {0};

static __IO uint32_t transferErrorDetected;    /* Set to 1 if an error transfer is detected */
static __IO uint32_t transferCompleteDetected; /* Set to 1 if transfer is correctly completed */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static __INLINE  void __SVC(void);

void MPU_Config(void);
void RISAF_Config(void);

static void HPDMA1_Init(void);
/* Private function prototypes -----------------------------------------------*/
static void TransferComplete(DMA_HandleTypeDef *handle_HPDMA1_Channel12);
static void TransferError(DMA_HandleTypeDef *handle_HPDMA1_Channel12);
static uint32_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if defined ( __CC_ARM   )
__ASM void __SVC(void)
{
  SVC 0x01
  BX R14
}
#elif defined ( __ICCARM__ )
static __INLINE  void __SVC()
{
  __ASM("svc 0x01");
}
#elif defined   (  __GNUC__  )
static __INLINE void __SVC()
{
  __ASM volatile("svc 0x01");
}
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  MPU_Config();

  /* STM32N6xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE BEGIN 1 */
  RISAF_Config();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Initialize LEDs */
  BSP_LED_Init(LED_GREEN);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Configure HPDMA channel 12 */
  /* - Channel:  secure */
  /* - Source and destination: secure/secure */
  /* - CID 4 selected and isolation filtering enable */
  HPDMA1_Init();

  /* Perceive CID set to 4 */
  /* From now on all AHB secure user accesses which arrive at the HPDMA, appear to carry the CID 4 */
  /* (AHB secure user accesses = access of DMA control registers) */
#if defined (STEP1) || defined (STEP3)
  HAL_SYSCFG_SetPerceivedCID(4);
#endif   /* STEP1 || STEP3 */
#if defined (STEP2)
  HAL_SYSCFG_SetPerceivedCID(5);  /* IAC HPDMA error expected */
#endif   /* STEP2 */

#if defined (STEP1) || defined (STEP3)
  if (HAL_SYSCFG_GetPerceivedCID()  != 4)
#endif   /* STEP1 || STEP3 */
#if defined (STEP2)
  if (HAL_SYSCFG_GetPerceivedCID()  != 5)
#endif   /* STEP2 */
  {
    /* Perceived CID not set */
    Error_Handler();
  }

  /* Switch from privileged to unprivileged */
  __set_CONTROL(0x1);
  __ISB();

  /* Reset transferErrorDetected to 0, it will be set to 1 if a transfer error is detected */
  transferErrorDetected = 0;
  /* Reset transferCompleteDetected to 0, it will be set to 1 then 2 if a transfer is correctly completed */
  dma_transfer_cpltd_counter = 0;

  /* Clean destination buffer */
  memset((void *)aDST_Buffer, 0x55U, sizeof(aDST_Buffer) );

  /* Execute the the 1st transfer */
  /* from                         */
  /*       AXI SRAM2              */
  /*       CID 1 & 4              */
  /* To    AXI SRAM1              */
  /*       CID 4                  */
  /* ---------------------------- */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)aIntermediate_Buffer, sizeof(aDST_Buffer)) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

#if defined (STEP2)
  /* IAC HPDMA error expected */

  /* Switch back from unprivileged to privileged -------------------*/
  /* Generate a system call exception, and in the ISR switch back Thread mode
    to privileged */
  __SVC();

  /* Check if privileged */
  if((__get_CONTROL() & 1U) != 0U)
  {
    /* unprivileged detected */
    Error_Handler();
  }

  /* Was HPDMA1 illegal detected */
  if(HAL_RIF_IAC_GetFlag(RIF_AWARE_PERIPH_INDEX_HPDMA1) == 0U)
  {
    Error_Handler();
  }

  /* End of example - STEP2       */
  /* STEP2 ended as expected      */
  BSP_LED_On(LED_GREEN);

  while(1)
  {
  }
#endif /* STEP2 */
#if defined (STEP3)
  /* IAC SRAM1 error expected */

  /* Switch back from unprivileged to privileged -------------------*/
  /* Generate a system call exception, and in the ISR switch back Thread mode
    to privileged */
  __SVC();

  /* Check if privileged */
  if((__get_CONTROL() & 1U) != 0U)
  {
    /* unprivileged detected */
    Error_Handler();
  }

  /* Was SRAM1 illegal detected */
  /* IAC illegal checking       */
  if(HAL_RIF_IAC_GetFlag(RIF_RCC_PERIPH_INDEX_AXISRAM1) == 0U)
  {
    Error_Handler();
  }

  /* RISAL illegal checking     */
  RISAF_IllegalAccess_t illegal_access;
  HAL_RIF_RISAF_GetIllegalAccess(RISAF2, &illegal_access);

 /* Primary region filtering applies: there should not be an illegal access */
  if (illegal_access.ErrorType != RISAF_ILLEGAL_ACCESS)
  {
    Error_Handler();
  }

  /* Illegal access seen at aIntermediate_Buffer */
  if (illegal_access.Data.AccessType != RIF_ACCTYPE_WRITE ||
      illegal_access.Data.Address    != 0x340E0000 ||  /* aIntermediate_Buffer */
      illegal_access.Data.CID        != RIF_CID_4 ||
      illegal_access.Data.SecPriv    != (RIF_ATTRIBUTE_SEC |
                                         RIF_ATTRIBUTE_NPRIV))
  {
    Error_Handler();
  }

  /* End of example - STEP3       */
  /* STEP3 ended as expected      */
  BSP_LED_On(LED_GREEN);

  while(1)
  {
  }
#endif /* STEP2 */

  /* Wait for transfer completion */
  HAL_Delay(2);

  /* Check DMA error */
  if (transferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* 1st transfer done as per HPDMA */
  if (dma_transfer_cpltd_counter != 1)
  {
    Error_Handler();
  }

  /* Execute the the 2nd transfer */
  /* from                         */
  /*       AXI SRAM1              */
  /*       CID 4                  */
  /* To    AXI SRAM2              */
  /*       CID 1 & 4              */
  /* ---------------------------- */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)aIntermediate_Buffer, (uint32_t) (aDST_Buffer), sizeof(aDST_Buffer)) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Wait for transfer completion */
  HAL_Delay(2);

  /* Check DMA error */
  if (transferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* 2nd transfer done as per HPDMA */
  if (dma_transfer_cpltd_counter != 2)
  {
    Error_Handler();
  }

  /* Switch back from unprivileged to privileged -------------------*/
  /* Generate a system call exception, and in the ISR switch back Thread mode
    to privileged */
  __SVC();

  /* Check if privileged */
  if((__get_CONTROL() & 1U) != 0U)
  {
    /* unprivileged detected */
    Error_Handler();
  }

  /* Perceived CID is set back to 1 (Cortex CID) */
  HAL_SYSCFG_SetPerceivedCID(1);

  if (HAL_SYSCFG_GetPerceivedCID()  != 1)
  {
    Error_Handler();
  }

  /* Check that data are properly received in aDST_Buffer */
  if (Buffercmp((uint8_t *)aSRC_Buffer, (uint8_t *)aDST_Buffer, BUFFER_SIZE_IN_BYTES) != 0U)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Further check should the source buffer is not initialized (IDE may behave differently) */
  if (aDST_Buffer[0U] != 0x102030405060708U)
  {
    /* Transfer Error */
    Error_Handler();
  }
    
  /* End of example */
  BSP_LED_On(LED_GREEN);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
/* USER CODE BEGIN CLK 1 */

/*         The system Clock is configured as follows :
              CPU Clock source               = IC1_CK
              System bus Clock source        = IC2_IC6_IC11_CK
              CPUCLK (sysa_ck) (Hz)          = 600000000
              SYSCLK AXI (sysb_ck) (Hz)      = 400000000
              SYSCLK NPU (sysc_ck) (Hz)      = 300000000
              SYSCLK AXISRAM3/4/5/6 (sysd_ck) (Hz) = 400000000
              HCLKx(Hz)                      = 200000000
              PCLKx(Hz)                      = 200000000
              AHB Prescaler                  = 2
              APB1 Prescaler                 = 1
              APB2 Prescaler                 = 1
              APB4 Prescaler                 = 1
              APB5 Prescaler                 = 1
              PLL1 State                     = ON
              PLL1 clock source              = HSI
              PLL1 M                         = 4
              PLL1 N                         = 75
              PLL1 P1                        = 1
              PLL1 P2                        = 1
              PLL1 FRACN                     = 0
              PLL2 State                     = BYPASS
              PLL2 clock source              = HSI
              PLL3 State                     = BYPASS
              PLL3 clock source              = HSI
              PLL4 State                     = BYPASS
              PLL4 clock source              = HSI
*/
/* USER CODE END CLK 1 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the System Power Supply
  */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable HSI */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Get current CPU/System buses clocks configuration and if necessary switch
 to intermediate HSI clock to ensure target clock can be set
  */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
  if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
     (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
  {
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 4;
  RCC_OscInitStruct.PLL1.PLLN = 75;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_CPUCLK|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2|RCC_CLOCKTYPE_PCLK5
                              |RCC_CLOCKTYPE_PCLK4;
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 2;
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 3;
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 4;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 3;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  MPU configuration
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef default_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();

  /* disable the MPU */
  HAL_MPU_Disable();

  /* create an attribute configuration for the MPU */
  attr_config.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  attr_config.Number = MPU_ATTRIBUTES_NUMBER0;

  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  /* Create a non cacheable region */
  /* Normal memory type, code execution allowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER0;
  default_config.BaseAddress = 0x34180000;
  default_config.LimitAddress = 0x341FFFFF;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigRegion(&default_config);

  /* create an attribute configuration for the MPU */
  attr_config.Attributes = MPU_DEVICE_NGNRE;
  attr_config.Number = MPU_ATTRIBUTES_NUMBER1;
  /* Create a non cacheable region */
  /* Peripheal type, unprivileged code allowed */

  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER1;
  default_config.BaseAddress = (uint32_t) HPDMA1_BASE_S;
  default_config.LimitAddress = 0x5802FFFF;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  HAL_MPU_ConfigRegion(&default_config);

  /* enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  /* Exit critical section to lock the system and avoid any issue around MPU mechanisme */
  __set_PRIMASK(primask_bit);
}

/**
  * @brief RISAF configuration
  * @param None
  * @retval None
  */
void RISAF_Config()
{
  RISAF_BaseRegionConfig_t risaf_base_config;

  /* Base region: data accessible only by secure master */
  risaf_base_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_base_config.Secure = RIF_ATTRIBUTE_SEC;
  risaf_base_config.PrivWhitelist = RIF_CID_NONE;
  risaf_base_config.WriteWhitelist = (RIF_CID_1 | RIF_CID_4); /* CID 1 for code execution */
  risaf_base_config.ReadWhitelist =  (RIF_CID_1 | RIF_CID_4); /* CID4 as 2 buffers used in test */

  risaf_base_config.StartAddress = protected_address_start;
  risaf_base_config.EndAddress = protected_address_end_A;

  HAL_RIF_RISAF_ConfigBaseRegion(RISAF3, RISAF_REGION_1, &risaf_base_config);

    /* Another region with different */
  risaf_base_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_base_config.Secure = RIF_ATTRIBUTE_SEC;
  risaf_base_config.PrivWhitelist = RIF_CID_NONE;
#if defined (STEP1) || defined (STEP2)
  risaf_base_config.WriteWhitelist = (RIF_CID_4);  /* CID4 as 1 intermediate buffer used in test */
#endif   /* STEP1 || STEP2 */
#if defined (STEP3)
  risaf_base_config.WriteWhitelist = (RIF_CID_5);  /* KO case - RISAF ERROR shall be raised as per AXISRAM1 */
#endif   /* STEP3 */

  risaf_base_config.StartAddress = protected_address_start;
  risaf_base_config.EndAddress = protected_address_end_B;

  HAL_RIF_RISAF_ConfigBaseRegion(RISAF2, RISAF_REGION_2, &risaf_base_config);
}

/**
  * @brief HPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void HPDMA1_Init(void)
{
  DMA_IsolationConfigTypeDef IsolationConfiginput;

  /* --------------- CLK EMABLING ---------------- */
  LL_MEM_EnableClock(LL_MEM_AXISRAM1);
  LL_MEM_EnableClock(LL_MEM_AXISRAM2);

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* --------------------------------------------- */

  /* Peripheral clock enable */
  __HAL_RCC_HPDMA1_CLK_ENABLE();

  /* HPDMA1 interrupt Init */
  HAL_NVIC_SetPriority(HPDMA1_Channel12_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HPDMA1_Channel12_IRQn);

  handle_HPDMA1_Channel12.Instance = HPDMA1_Channel12;
  handle_HPDMA1_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA1_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.Priority = DMA_HIGH_PRIORITY;
  handle_HPDMA1_Channel12.Init.SrcBurstLength = 2;
  handle_HPDMA1_Channel12.Init.DestBurstLength = 2;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_DMA_ConfigChannelAttributes(&handle_HPDMA1_Channel12, DMA_CHANNEL_NPRIV|DMA_CHANNEL_SEC
                              |DMA_CHANNEL_SRC_SEC|DMA_CHANNEL_DEST_SEC) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* DMA channel CID set to 4 */
  IsolationConfiginput.CidFiltering =  DMA_ISOLATION_ON;
  IsolationConfiginput.StaticCid = DMA_CHANNEL_STATIC_CID_4;

  if(HAL_DMA_SetIsolationAttributes(&handle_HPDMA1_Channel12 , &IsolationConfiginput) != HAL_OK)
  {
    while(1);
  };
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *handle_HPDMA1_Channel12)
{
  dma_transfer_cpltd_counter ++;
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *handle_HPDMA1_Channel12)
{
  transferErrorDetected = 1;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0   : pBuffer1 identical to pBuffer2
  *         > 0 : pBuffer1 differs from pBuffer2
  */
static uint32_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
