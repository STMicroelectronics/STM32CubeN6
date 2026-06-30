/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RCC/RCC_UseHSI_PLLasSystemClock/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to change dynamically SYSCLK through
  *          the STM32N6xx RCC LL API.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Structure based on parameters used for PLL config */
typedef struct
{
  uint32_t Frequency;   /*!< SYSCLK frequency requested */
  uint32_t PLLM;        /*!< PLLM factor used for PLL */
  uint32_t PLLN;        /*!< PLLN factor used for PLL */
  uint32_t PLLP1;        /*!< PLLP factor used for PLL */
  uint32_t PLLP2;        /*!< PLLP factor used for PLL */
} RCC_PLL_ConfigTypeDef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Number of PLL Config */
#define RCC_PLL_CONFIG_NB   2
#define RCC_FREQUENCY_LOW          ((uint32_t)453333333) /* Low Frequency set to 450MHz*/
#define RCC_FREQUENCY_HIGH         ((uint32_t)600000000) /* High Frequency set to 600MHz*/

/* Oscillator time-out values */
#define HSE_TIMEOUT_VALUE          ((uint32_t)5000) /* Time out for HSE start up, in ms */
#define HSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define PLL_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define CLOCKSWITCH_TIMEOUT_VALUE  ((uint32_t)5000) /* 5 s    */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Variable to set different PLL config with HSI as PLL source clock */
static RCC_PLL_ConfigTypeDef aPLL_ConfigHSI[RCC_PLL_CONFIG_NB] =
{
  {RCC_FREQUENCY_LOW, 12, 170, 1, 1},
  {RCC_FREQUENCY_HIGH, 4, 75, 1, 1}
};

/* PLL Config index */
__IO uint8_t bPLLIndex = 0;

/* Variable to save the current configuration to apply */
static uint32_t uwFrequency = RCC_FREQUENCY_LOW, uwPLLN = 0, uwPLLM = 0, uwPLLP1  = 0, uwPLLP2 = 0;

/* Variable to indicate a change of PLL config after a button press */
__IO uint8_t bButtonPress = 0;

uint32_t Timeout = 0; /* Variable used for Timeout management */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

uint32_t RCC_StartHSIAndWaitForHSIReady(void);
uint32_t ChangePLL_HSI_Config(void);
void     LED_Blinking(uint32_t Period);
void ChangePLLConfiguration(uint32_t uwFrequency, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP1, uint32_t PLLP2);

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

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Toggle LED accordingly to the frequency */
    if (uwFrequency == RCC_FREQUENCY_LOW)
    {
      /* Slow toggle */
      LED_Blinking(LED_BLINK_SLOW);
    }
    else
    {
      /* Fast toggle */
      LED_Blinking(LED_BLINK_FAST);
    }

    /* PLL config change has been requested */
    if (ChangePLL_HSI_Config() != RCC_ERROR_NONE)
    {
      /* Problem to switch to HSI, blink LD1 */
      LED_Blinking(LED_BLINK_ERROR);
    }

    /* Reset button press */
    bButtonPress = 0;
  }
  /* USER CODE END 3 */
}
/* USER CODE BEGIN CLK 1 */
/* USER CODE END CLK 1 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Configure the System Power Supply */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
  LL_PWR_ConfigSupply(LL_PWR_EXTERNAL_SOURCE_SUPPLY);
  while (LL_PWR_IsActiveFlag_ACTVOSRDY() == 0U)
  {
  }

  /** Configure the main internal regulator output voltage
  */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while(LL_PWR_IsActiveFlag_VOSRDY() == 0)
  {
  }

  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() == 0)
  {
  }

 /** Get current CPU/System buses clocks configuration and
  *if necessary switch to intermediate HSI clock to ensure target clock can be set
  */
  if ((LL_RCC_GetCpuClkSource() == LL_RCC_CPU_CLKSOURCE_STATUS_IC1) ||
     (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11))
  {
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
    {
    }
    LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_HSI);
    while(LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_HSI)
    {
    }
  }
  LL_RCC_PLL1_Disable();
  while(LL_RCC_PLL1_IsReady() == 1)
  {
  }
  LL_RCC_PLL1_DisableModulationSpreadSpectrum();
  LL_RCC_PLL1_DisableBypass();
  LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSI);
  LL_RCC_PLL1_SetM(4);
  LL_RCC_PLL1_SetN(75);
  LL_RCC_PLL1_SetP1(1);
  LL_RCC_PLL1_SetP2(1);
  LL_RCC_PLL1_SetFRACN(0);
  LL_RCC_PLL1_AssertModulationSpreadSpectrumReset();
  LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1_Enable();
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_1);
  LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_1);

  LL_RCC_IC1_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC1_SetDivider(2);
  LL_RCC_IC1_Enable();
  LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
  while(LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1)
  {
  }

  LL_RCC_IC2_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC2_SetDivider(3);
  LL_RCC_IC6_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC6_SetDivider(4);
  LL_RCC_IC11_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC11_SetDivider(3);
  LL_RCC_IC2_Enable();
  LL_RCC_IC6_Enable();
  LL_RCC_IC11_Enable();
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11)
  {
  }

  LL_Init1msTick(600000000);

  LL_SetSystemCoreClock(600000000);
  LL_RCC_EnableMCO(LL_RCC_MCO2);
  LL_RCC_ConfigMCO(LL_RCC_MCO2SOURCE_HSI, LL_RCC_MCO2_DIV_1);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);

  /**/
  LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTC, LL_EXTI_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_DOWN);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
   LL_PWR_EnableVddIO4();
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Enable HSI and Wait for HSI ready
  * @param  None
  * @retval RCC_ERROR_NONE if no error
  */
uint32_t RCC_StartHSIAndWaitForHSIReady()
{
  /* Enable HSI and wait for HSI ready*/
  LL_RCC_HSI_Enable();

#if (USE_TIMEOUT == 1)
  Timeout = HSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while (LL_RCC_HSI_IsReady() != 1)
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the Time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      if (Timeout-- == 0)
      {
        /* Time-out occurred. Return an error */
        return RCC_ERROR_TIMEOUT;
      }
    }
#endif /* USE_TIMEOUT */
  }

  return RCC_ERROR_NONE;
}

/**
  * @brief  Switch the PLL source to HSI, and select the PLL as SYSCLK
  *         source to reach new requested frequency.
  * @param  None
  * @retval RCC_ERROR_NONE if no error
  */
uint32_t ChangePLL_HSI_Config(void)
{
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
  LL_PWR_ConfigSupply(LL_PWR_EXTERNAL_SOURCE_SUPPLY);
  while (LL_PWR_IsActiveFlag_ACTVOSRDY() == 0U)
  {
  }

  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() == 0)
  {
  }

  /** Get current CPU/System buses clocks configuration and
  *if necessary switch to intermediate HSI clock to ensure target clock can be set
  */
  if ((LL_RCC_GetCpuClkSource() == LL_RCC_CPU_CLKSOURCE_STATUS_IC1) ||
     (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11))
  {
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
   while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
   {
   }
   LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_HSI);
   while(LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_HSI)
   {
   }
  }
  LL_RCC_PLL1_Disable();
  while(LL_RCC_PLL1_IsReady() == 1)
  {
  }
  LL_RCC_PLL1_DisableModulationSpreadSpectrum();
  LL_RCC_PLL1_DisableBypass();
  LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSI);
  /* Configure PLL with new configuration */
  ChangePLLConfiguration(uwFrequency, uwPLLM, uwPLLN, uwPLLP1, uwPLLP2);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  /* Set systick to 1ms */
  LL_Init1msTick(uwFrequency);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = uwFrequency;
    
  return RCC_ERROR_NONE;
}

/**
  * @brief  Function to change Main PLL configuration
 #error
  * @retval RCC_ERROR_NONE if no error
  */
void ChangePLLConfiguration(uint32_t uwFrequency, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP1, uint32_t PLLP2)
{
  if (uwFrequency==RCC_FREQUENCY_LOW)
  { 
  LL_RCC_PLL1_SetM(PLLM);
  LL_RCC_PLL1_SetN(PLLN);
  LL_RCC_PLL1_SetP1(PLLP1);
  LL_RCC_PLL1_SetP2(PLLP2);
  LL_RCC_PLL1_SetFRACN(0);
  LL_RCC_PLL1_AssertModulationSpreadSpectrumReset();
  LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1_Enable();
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_1);
  LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_1);

  LL_RCC_IC1_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC1_SetDivider(2);
  LL_RCC_IC1_Enable();
  LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
  while(LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1)
  {
  }

  LL_RCC_IC2_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC2_SetDivider(3);
  LL_RCC_IC6_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC6_SetDivider(4);
  LL_RCC_IC11_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC11_SetDivider(3);
  LL_RCC_IC2_Enable();
  LL_RCC_IC6_Enable();
  LL_RCC_IC11_Enable();
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11)
  {
  }

  LL_Init1msTick(600000000);

  LL_SetSystemCoreClock(600000000);
  }
  if (uwFrequency==RCC_FREQUENCY_HIGH)
  {


  LL_RCC_PLL1_SetM(PLLM);
  LL_RCC_PLL1_SetN(PLLN);
  LL_RCC_PLL1_SetP1(PLLP1);
  LL_RCC_PLL1_SetP2(PLLP2);
  LL_RCC_PLL1_SetFRACN(0);
  LL_RCC_PLL1_AssertModulationSpreadSpectrumReset();
  LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1_Enable();
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_1);
  LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_1);

  LL_RCC_IC1_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC1_SetDivider(2);
  LL_RCC_IC1_Enable();
  LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
  while(LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1)
  {
  }

  LL_RCC_IC2_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC2_SetDivider(3);
  LL_RCC_IC6_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC6_SetDivider(4);
  LL_RCC_IC11_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC11_SetDivider(3);
  LL_RCC_IC2_Enable();
  LL_RCC_IC6_Enable();
  LL_RCC_IC11_Enable();
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11)
  {
  }

  LL_Init1msTick(453333333);

  LL_SetSystemCoreClock(453333333);
}
}
/**
  * @brief  Set LD1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  *         Exit of this function when a press button is detected
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  if (Period != LED_BLINK_ERROR)
  {
    /* Toggle IO in an infinite loop up to a detection of press button */
    while (bButtonPress != 1)
    {
      /* LD1 is blinking at Period ms */
      LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
      LL_mDelay(Period);
    }
  }
  else
  {
    /* Toggle IO in an infinite loop due to an error */
    while (1)
    {
      /* Error if LD1 is slowly blinking (1 sec. period) */
      LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
      LL_mDelay(Period);
    }
  }
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage User button press
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Get the PLL config to apply */
  uwFrequency = aPLL_ConfigHSI[bPLLIndex].Frequency;
  uwPLLM      = aPLL_ConfigHSI[bPLLIndex].PLLM;
  uwPLLN      = aPLL_ConfigHSI[bPLLIndex].PLLN;
  uwPLLP1     = aPLL_ConfigHSI[bPLLIndex].PLLP1;
  uwPLLP2     = aPLL_ConfigHSI[bPLLIndex].PLLP2;

  /* Set new PLL config Index */
  bPLLIndex = (bPLLIndex + 1) % RCC_PLL_CONFIG_NB;

  /* Set variable to request of PLL config change */
  bButtonPress = 1;
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
  /* Infinite loop */
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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
