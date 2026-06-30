/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRC/CRC_ReverseModes/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use CRC the STM32N6xx CRC HAL API.
  *          to compute a 36-bit CRC code from a data Words (32-bit data) buffer,
  *          based on a default generating polynomial and initialization
  *          value, with input and output data reversing features enabled.
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

/* User-defined CRC init value */
/* As the CRC is 32-bit long, the init value is 32-bit long as well */
#define CRC_INIT_VALUE      0xFFFFFFFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

/* USER CODE BEGIN PV */

/* Input data buffer */
 static const uint32_t CRC32_inputDATA[] = {0x12345678};

 static const uint32_t CRC32_inputDATA_bit_inversion[]= {0x1e6a2c48}; /* This buffer is derived
                 from the CRC32_inputDATA buffer by a bit-reversal operation carried out on word*/

 static const uint32_t CRC32_inputDATA_BitByByte_inversion[]={0x482c6a1e}; /* This  buffer is derived
                      from the CRC32_inputDATA buffer by a bit-reversal operation carried out on byte*/

 static const uint32_t CRC32_inputDATA_BitByHalfWord_inversion[]={0x2c481e6a}; /* This  buffer is derived
                     from the CRC32_inputDATA buffer by a bit-reversal operation carried out on halfword*/

 static const uint32_t CRC32_inputDATA_bytebyWord_inversion[]={0x78563412}; /* This  buffer is derived
                      from the CRC32_inputDATA buffer by a byte-reversal operation carried out on word*/

 static const uint32_t CRC32_inputDATA_halfWordbyWord_inversion[]={0x56781234}; /* This  buffer is derived
                      from the CRC32_inputDATA buffer by a halfword-reversal operation carried out on word*/


/* Expected CRC value yielded by CRC32_inputDATA[] input buffer
   with input and output data reversal feature disabled (default)*/
static const uint32_t uwExpectedCRCValue = 0xDF8A8A2B;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t Word_Inversion(uint32_t Input);
static uint32_t HalfWord_Inversion_By_Word(uint32_t Input);
static uint32_t Byte_Inversion_By_Word(uint32_t Input);
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

/* Used for storing CRC Value */
 uint32_t uwCRCValue = 0;

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* Configure LED2 and LED3 */
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);


  /******************************************************************************************/
  /*                                                                                        */
  /*    CRC computation with input/output reversal options disabled                         */
  /*                                                                                        */
  /******************************************************************************************/

  /* The 32-bit long CRC of the 1-word buffer "CRC32_inputDATA" is computed. After peripheral
     initialization,the CRC calculator is initialized with the default INIT value that is 0xFFFFFFFF.
    The computed CRC is stored in uint32_t uwCRCValue. */

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /*******************************************************************************************/
  /*                                                                                         */
  /*    CRC computation with input bit-reversal by word enable, output reversal disabled     */
  /*                                                                                         */
  /*******************************************************************************************/

  /* As explained above, CRC32_inputDATA_bit_inversion is obtained from the CRC32_inputDATA buffer
     by a bit-reversal operation carried out on word */

  /* bit-reversal by word option is enable */
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BIT_BYWORD;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA_bit_inversion, 1);

   /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /*********************************************************************************************/
  /*                                                                                           */
  /*      CRC computation with input bit-reversal by byte enable, Output reversal disabled     */
  /*                                                                                           */
  /*********************************************************************************************/

  /* As explained above, CRC32_inputDATA_BitByByte_inversion is obtained from the CRC32_inputDATA buffer
     by a bit-reversal operation carried out on byte */

  /* bit-reversal by byte option is enable */
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BIT_BYBYTE;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA_BitByByte_inversion, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

 /**********************************************************************************************/
  /*                                                                                           */
  /*    CRC computation with input bit-reversal by HalfWord enable, Output reversal disabled   */
  /*                                                                                           */
  /*********************************************************************************************/

    /* As explained above, CRC32_inputDATA_BitByHalfWord_inversion is obtained from the CRC32_inputDATA buffer
     by a bit-reversal operation carried out on halfword */

  /* bit-reversal by halfword option is enable */
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BIT_BYHALFWORD;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA_BitByHalfWord_inversion, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /*********************************************************************************************/
  /*                                                                                           */
  /*     CRC computation with input byte-reversal by word enable, Output reversal disabled     */
  /*                                                                                           */
  /*********************************************************************************************/

  /* As explained above, CRC32_inputDATA_bytebyWord_inversion is obtained from CRC32_inputDATA
     by a byte-reversal operation carried out on word.*/

    /* Reversal Input byte-reversal By word option are enable */
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE_BYWORD;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA_bytebyWord_inversion, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

/**************************************************************************************************/
/*                                                                                                */
/*      CRC computation with input half-word-reversal by word enable, Output reversal disabled    */
/*                                                                                                */
/**************************************************************************************************/

  /* As explained above, CRC32_inputDATA_halfWordbyWord_inversion is obtained from CRC32_inputDATA
     by a half-word operation carried out on word. */


    /* Reversal Input half-word-reversal by word option are enable */
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_HALFWORD_BYWORD;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA_halfWordbyWord_inversion, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue)
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /************************************************************************************/
  /*                                                                                  */
  /*      CRC computation with output bit-reversal enable, Input reversal disabled    */
  /*                                                                                  */
  /************************************************************************************/

   /* Reversal input options are disabled */
   hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;

  /* Reversal output bit-reversal option are enable */
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_BIT;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != Word_Inversion(uwExpectedCRCValue))
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /****************************************************************************************/
  /*                                                                                      */
  /*    CRC computation with output byte-reversal by word enable, Input reversal disabled */
  /*                                                                                      */
  /****************************************************************************************/

   /* Reversal output byte-reversal by word option are enable */
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_BYTE;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != Byte_Inversion_By_Word(uwExpectedCRCValue))
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }

  if (HAL_CRC_DeInit(&hcrc) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

/*********************************************************************************************/
/*                                                                                           */
/*  CRC computation with output half-word-reversal by word enable, Input reversal disabled   */
/*                                                                                           */
/*********************************************************************************************/

   /* Reversal output half-word-reversa by Word option are enable */
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_HALFWORD;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the Default INIT value that is 0xFFFFFFFF. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC32_inputDATA, 1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != HalfWord_Inversion_By_Word(uwExpectedCRCValue))
  {
    /* Wrong CRC value: Turn LED2 on */
   Error_Handler();
  }
  else
  {
    /* Right CRC value: Turn LED3 (green LED) on */
    BSP_LED_On(LED3);
  }
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
/*
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
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  This function bit-wise inverts a 32-bit CRC
  * @param  32-bit CRC
  * @retval 32-bit inverted CRC
  */
static uint32_t Word_Inversion(uint32_t Input)
{
  uint32_t output = 0x0;
  uint32_t i;

  for (i=0;i<32;i++)
  {
    output |= (((Input & (1 << i)) >> i) << (31-i));
  }
  return  output;
}

/**
  * @brief  This function HalfWord inverts a 32-bit CRC
  * @param  32-bit CRC (32-bit input)
  * @retval 32-bit inverted CRC (32-bit output)
  */
static uint32_t HalfWord_Inversion_By_Word(uint32_t Input)
{
  uint32_t output = 0x0;

  output = ((Input & 0xFFFF) << 16) | ((Input & 0xFFFF0000)>>16);

  return  output;
}

/**
  * @brief  This function byte inverts a 32-bit CRC
  * @param  32-bit CRC (32-bit Input)
  * @retval 32-bit inverted CRC (32-bit output)
  */
static uint32_t Byte_Inversion_By_Word(uint32_t Input)
{
  uint32_t output = 0x0;

  output  = ((Input & 0xFF) << 24)|((Input & 0xFF00)<< 8)|((Input & 0xFF0000)>>8)|((Input & 0xFF000000)>> 24);

  return  output ;
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
  while (1)
  {
    BSP_LED_Toggle(LED2);
    HAL_Delay(1000);
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
