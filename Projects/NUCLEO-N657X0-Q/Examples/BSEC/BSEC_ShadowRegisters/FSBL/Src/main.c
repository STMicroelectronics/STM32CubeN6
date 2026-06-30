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
#include "stm32n6xx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Use a customer-available shadow OTP */
#define SHADOW_REG_INDEX 124  /* hardware configuration register */
#define SCRATCH_REGISTER_INDEX 0 /* can be between 0 and 3 (included) */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BSEC_HandleTypeDef hbsec;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_BSEC_Init(void);
/* USER CODE BEGIN PFP */

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
  uint32_t otp_value = 0;
  uint32_t reset_number = 0;
  uint32_t bsec_status = 0;
  BSEC_ScratchRegTypeDef scratch_register;

  hbsec.Instance = BSEC;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_BSEC_Init();
  /* USER CODE BEGIN 2 */

  /* Read the scratch register to know whether this is the first or second execution */
  scratch_register.RegType = HAL_BSEC_SCRATCH_REG;
  scratch_register.RegNumber = SCRATCH_REGISTER_INDEX;
  if (HAL_BSEC_ReadScratchValue(&hbsec, &scratch_register, &otp_value) != HAL_OK)
  {
    Error_Handler();
  }
  if(otp_value == 0){
    /* After a power-on reset, the value of a regular scratch register is 0 */

    /* Get the number of resets and store it in a scratch register */
    if(HAL_BSEC_GetNumberOfResets(&hbsec, HAL_BSEC_WARM_RESET, &reset_number) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_BSEC_WriteScratchValue(&hbsec, &scratch_register, reset_number) != HAL_OK)
    {
      Error_Handler();
    }
    /* Read the shadow register value */
    if(HAL_BSEC_OTP_ReadShadow(&hbsec, SHADOW_REG_INDEX, &otp_value) != HAL_OK)
    {
      Error_Handler();
    }

    /* Modify the shadow value. In this case, freeze IWDG in stop and standby mode */
    otp_value |= 0x6;

    /* Write the value to the shadow register */
    if(HAL_BSEC_OTP_WriteShadow(&hbsec, SHADOW_REG_INDEX, otp_value) != HAL_OK)
    {
      Error_Handler();
    }

    /* Lock shadow register write, program and reload */
    if (HAL_BSEC_OTP_Lock(&hbsec, SHADOW_REG_INDEX, HAL_BSEC_FUSE_WRITE_LOCKED
                                                  | HAL_BSEC_FUSE_RELOAD_LOCKED) != HAL_OK)
    {
      Error_Handler();
    }

    /* Try to reload the fuse value */
    if(HAL_BSEC_OTP_Reload(&hbsec, SHADOW_REG_INDEX) != HAL_OK)
    {
      if(HAL_BSEC_GetErrorCode(&hbsec, &bsec_status) != HAL_OK)
      {
        if(bsec_status != HAL_BSEC_ERROR_LOCK)
        {
          Error_Handler();
        }
        /* The expected error occurred, continue the example */
      }
    } else
    {
      Error_Handler();
    }

    /* Try to write the fuse value */
    if(HAL_BSEC_OTP_WriteShadow(&hbsec, SHADOW_REG_INDEX, otp_value) != HAL_OK)
    {
      if(HAL_BSEC_GetErrorCode(&hbsec, &bsec_status) != HAL_OK)
      {
        if(bsec_status != HAL_BSEC_ERROR_LOCK)
        {
          Error_Handler();
        }
        /* The expected error occurred, continue the example */
      }
    } else
    {
      Error_Handler();
    }

    /* Try to reload the fuse value */
    if(HAL_BSEC_OTP_Reload(&hbsec, SHADOW_REG_INDEX) != HAL_OK)
    {
      if(HAL_BSEC_GetErrorCode(&hbsec, &bsec_status) != HAL_OK)
      {
        if(bsec_status != HAL_BSEC_ERROR_LOCK)
        {
          Error_Handler();
        }
        /* The expected error occurred, continue the example */
      }
    } else
    {
      Error_Handler();
    }

    /* Reset to start the second part of the example */
    HAL_NVIC_SystemReset();
  }else
  {
    /* Second execution, start the second part of the tests */
    if(HAL_BSEC_OTP_ReadShadow(&hbsec, SHADOW_REG_INDEX, &otp_value) != HAL_OK)
    {
      Error_Handler();
    }

    /* Verify that the value written previously was not applied */
    if(otp_value & 0x06)
    {
      Error_Handler();
    }

     /* Write again to show that the lock has been locked */
    if(HAL_BSEC_OTP_WriteShadow(&hbsec, SHADOW_REG_INDEX, otp_value) != HAL_OK)
    {
      Error_Handler();
    }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(500);    
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

/**
  * @brief BSEC Initialization Function
  * @param None
  * @retval None
  */
static void MX_BSEC_Init(void)
{

  /* USER CODE BEGIN BSEC_Init 0 */

  /* USER CODE END BSEC_Init 0 */
  __HAL_RCC_BSEC_CLK_ENABLE();

  /* USER CODE BEGIN BSEC_Init 1 */

  /* USER CODE END BSEC_Init 1 */
  /* USER CODE BEGIN BSEC_Init 2 */

  /* USER CODE END BSEC_Init 2 */

}

/* USER CODE BEGIN 4 */

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
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(500);
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
