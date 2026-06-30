/* USER CODE BEGIN Header */
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t protected_address_start = 0x0;
static uint32_t protected_address_end = 0x64FFFU; /* protected_address_start
                                                   + RISAF2_GRANULARITY - 1 */
uint32_t *protected_buf;

volatile uint32_t risaf_it = 0;

RISAF_BaseRegionConfig_t risaf_base_config;
RISAF_SubRegionConfig_t risaf_sub_config;
RISAF_IllegalAccess_t illegal_access;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemIsolation_Config(void);
/* USER CODE BEGIN PFP */
void MPU_Config(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Place buffer in SRAM1 */
  protected_buf = (uint32_t *)0x34064000U; /* SRAM1 */
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Enable IAC_IRQ */
  HAL_NVIC_SetPriority(IAC_IRQn, 0x0, 0x0);
  HAL_NVIC_EnableIRQ(IAC_IRQn);

  __HAL_RCC_IAC_CLK_ENABLE();
  HAL_RIF_IAC_EnableIT(RIF_AWARE_PERIPH_INDEX_RISAF2);

    /* Enable RISAF clock */
  __HAL_RCC_RISAF_CLK_ENABLE();

  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);


   /* 1- Primary region: secure and privileged CPU access */
  protected_buf[0] = 0xFFFFFFFFU;
  protected_buf[0] = 0x0U;

  HAL_RIF_RISAF_GetIllegalAccess(RISAF2, &illegal_access);

  /* Primary region filtering applies: there should not be an illegal access */
  if (illegal_access.ErrorType != RISAF_ILLEGAL_ACCESS_NONE)
  {
    Error_Handler();
  }

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  SystemIsolation_Config();
  /* USER CODE BEGIN 2 */

  /* 2- Base region: data accessible only by non-secure master */
  protected_buf[0] = 0xFFFFFFFFU;
  protected_buf[0] = 0x0U;

  /* Wait for IAC interrupt */
  while(!risaf_it);
  risaf_it = 0U;

  HAL_RIF_RISAF_GetIllegalAccess(RISAF2, &illegal_access);

  /* Illegal access from CID1 Secure CPU */
  if (illegal_access.ErrorType != RISAF_ILLEGAL_ACCESS)
  {
    Error_Handler();
  }

  if (illegal_access.Data.AccessType != RIF_ACCTYPE_WRITE ||
      illegal_access.Data.Address    != 0x34064000U ||
      illegal_access.Data.CID        != RIF_CID_1 ||
      illegal_access.Data.SecPriv    != (RIF_ATTRIBUTE_SEC |
                                         RIF_ATTRIBUTE_PRIV))
  {
    Error_Handler();
  }

  /* Set base region filtering to secure access only */
  risaf_base_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_base_config.Secure = RIF_ATTRIBUTE_SEC;
  risaf_base_config.PrivWhitelist = RIF_CID_NONE;
  risaf_base_config.WriteWhitelist = RIF_CID_NONE;
  risaf_base_config.ReadWhitelist = RIF_CID_NONE;
  risaf_base_config.StartAddress = protected_address_start;
  risaf_base_config.EndAddress = protected_address_end;

  HAL_RIF_RISAF_ConfigBaseRegion(RISAF2, RISAF_REGION_1, &risaf_base_config);

  /* 3- Subregion:  data accessible only by non-secure master */
  risaf_sub_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_sub_config.CID = RIF_CID_MASK;
  risaf_sub_config.SecPriv = RIF_ATTRIBUTE_NSEC;
  risaf_sub_config.ReadWrite = RISAF_READ_WRITE_MASK;
  risaf_sub_config.Lock = RIF_LOCK_DISABLE;
  risaf_sub_config.StartAddress = protected_address_start;
  risaf_sub_config.EndAddress = protected_address_end;

  HAL_RIF_RISAF_ConfigSubRegion(RISAF2, RISAF_REGION_1, RISAF_SUBREGION_A,
                                   &risaf_sub_config);

  protected_buf[0] = 0xFFFFFFFFU;
  protected_buf[0] = 0x0U;

  /* Wait for IAC interrupt */
  while(!risaf_it);
  risaf_it = 0U;

  HAL_RIF_RISAF_GetIllegalAccess(RISAF2, &illegal_access);

  /* Illegal access from CID1 Secure and Privileged CPU */
  if (illegal_access.ErrorType != RISAF_ILLEGAL_ACCESS)
  {
    Error_Handler();
  }

  if (illegal_access.Data.AccessType != RIF_ACCTYPE_WRITE ||
      illegal_access.Data.Address    != 0x34064000U ||
      illegal_access.Data.CID        != RIF_CID_1 ||
      illegal_access.Data.SecPriv    != (RIF_ATTRIBUTE_SEC |
                                         RIF_ATTRIBUTE_PRIV))
  {
    Error_Handler();
  }

  /* 4- Subregion overlap:  data accessible only by non-secure master   */
  /*                        even if one of the subregion is secure-only */
  risaf_sub_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_sub_config.CID = RIF_CID_MASK;
  risaf_sub_config.SecPriv = RIF_ATTRIBUTE_SEC;
  risaf_sub_config.ReadWrite = RISAF_READ_WRITE_MASK;
  risaf_sub_config.Lock = RIF_LOCK_DISABLE;
  risaf_sub_config.StartAddress = protected_address_start;
  risaf_sub_config.EndAddress = protected_address_end;

  HAL_RIF_RISAF_ConfigSubRegion(RISAF2, RISAF_REGION_1, RISAF_SUBREGION_B,
                                   &risaf_sub_config);

  protected_buf[0] = 0xFFFFFFFFU;
  protected_buf[0] = 0x0U;

  /* Wait for IAC interrupt */
  while(!risaf_it);
  risaf_it = 0U;

  HAL_RIF_RISAF_GetIllegalAccess(RISAF2, &illegal_access);

  /* Illegal access from CID1 Secure CPU */
  if (illegal_access.ErrorType != RISAF_ILLEGAL_ACCESS)
  {
    Error_Handler();
  }

  if (illegal_access.Data.AccessType != RIF_ACCTYPE_WRITE ||
      illegal_access.Data.Address    != 0x34064000U ||
      illegal_access.Data.CID        != RIF_CID_1 ||
      illegal_access.Data.SecPriv    != (RIF_ATTRIBUTE_SEC |
                                         RIF_ATTRIBUTE_PRIV))
  {
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

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            CPU Clock source               = IC1_CK
  *            System bus Clock source        = IC2_IC6_IC11_CK
  *            CPUCLK (sysa_ck) (Hz)          = 600000000
  *            SYSCLK AXI (sysb_ck) (Hz)      = 400000000
  *            SYSCLK NPU (sysc_ck) (Hz)      = 300000000
  *            SYSCLK AXISRAM3/4/5/6 (sysd_ck) (Hz) = 400000000
  *            HCLKx(Hz)                      = 200000000
  *            PCLKx(Hz)                      = 200000000
  *            AHB Prescaler                  = 2
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB4 Prescaler                 = 1
  *            APB5 Prescaler                 = 1
  *            PLL1 State                     = ON
  *            PLL1 clock source              = HSI
  *            PLL1 M                         = 4
  *            PLL1 N                         = 75
  *            PLL1 P1                        = 1
  *            PLL1 P2                        = 1
  *            PLL1 FRACN                     = 0
  *            PLL2 State                     = BYPASS
  *            PLL2 clock source              = HSI
  *            PLL3 State                     = BYPASS
  *            PLL3 clock source              = HSI
  *            PLL4 State                     = BYPASS
  *            PLL4 clock source              = HSI
  * @retval None
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

/**
  * @brief RIF Initialization Function
  * @param None
  * @retval None
  */
  static void SystemIsolation_Config(void)
{

  /* USER CODE BEGIN RIF_Init 0 */

  /* USER CODE END RIF_Init 0 */

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();

  /* RISAF Config */
  RISAF_BaseRegionConfig_t risaf_base_config;
  __HAL_RCC_RISAF_CLK_ENABLE();

  /* set up base region configuration for CPUAXI_RAM0*/
  /* region 1 is non-secure */
  risaf_base_config.EndAddress = 0x64fff;
  risaf_base_config.Filtering = RISAF_FILTER_ENABLE;
  risaf_base_config.ReadWhitelist = RIF_CID_NONE;
  risaf_base_config.WriteWhitelist = RIF_CID_NONE;
  risaf_base_config.Secure = RIF_ATTRIBUTE_NSEC;
  risaf_base_config.PrivWhitelist = RIF_CID_NONE;
  risaf_base_config.StartAddress = 0x0000;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF2, RISAF_REGION_1, &risaf_base_config);

  /* USER CODE BEGIN RIF_Init 1 */

  /* USER CODE END RIF_Init 1 */
  /* USER CODE BEGIN RIF_Init 2 */

  /* USER CODE END RIF_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  This function configures a non cacheable region for
  *         the protected buffer.
  * @retval None
  */
void HAL_RIF_ILA_Callback(uint32_t PeriphId)
{
  risaf_it = 1U;
}

/**
  * @brief  This function configures a non cacheable region for
  *         the protected buffer.
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef default_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();
  /* Disable the MPU */
  HAL_MPU_Disable();
  /* Create an attribute configuration for the MPU */
  attr_config.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  attr_config.Number = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigMemoryAttributes(&attr_config);
  /* Create a non cacheable region */
  /* Normal memory type, code execution allowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER0;
  default_config.BaseAddress = 0x34064000U;
  default_config.LimitAddress = 0x34064FFFU;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigRegion(&default_config);
  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  /* Exit critical section to lock the system and avoid any issue around MPU mechanism */
  __set_PRIMASK(primask_bit);
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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  BSP_LED_On(LED_BLUE);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
