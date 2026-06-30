/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_TimeBase_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a timer instance to generate a 
  *          time base using the STM32N6xx TIM LL API.
  *          Peripheral initialization done using LL unitary services functions.
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
/* Number of time base frequencies */
#define TIM_BASE_FREQ_NB 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t tim_prescaler = 0;
static uint32_t tim_period = 0;
static uint32_t TimOutClock = 1;

/* Actual autoreload value multiplication factor */
static uint8_t AutoreloadMult = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
uint32_t GetTimerCLKFreq(void);
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
  /* Set the pre-scaler value to have TIM1 counter clock equal to 10 kHz      */
  /*
In this example TIM1 input clock TIM1CLK is set to TIMPRE clock.
      As TIMPRE pre-scaler is equal to 1, and since AHB pre-scaler is equal to 2,
      => TIM1CLK = SystemBusClock (400 MHz)
  */
  TimOutClock = GetTimerCLKFreq();
  tim_prescaler = __LL_TIM_CALC_PSC(TimOutClock, 10000);

  /* TIM1CLK = SystemCoreClock / (APB prescaler & multiplier)              */
  TimOutClock = TimOutClock;
  tim_period = __LL_TIM_CALC_ARR(TimOutClock, tim_prescaler, 10);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  /* Clear the update flag */
  LL_TIM_ClearFlag_UPDATE(TIM1);

  /* Enable the update interrupt */
  LL_TIM_EnableIT_UPDATE(TIM1);

  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
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

  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_1);

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

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* TIM1 interrupt Init */
  NVIC_SetPriority(TIM1_BRK_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_BRK_IRQn);
  NVIC_SetPriority(TIM1_UP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_UP_IRQn);
  NVIC_SetPriority(TIM1_TRG_COM_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = tim_prescaler;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = tim_period;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief Return the timer 1 kernel clock frequency (tim_ker_ck).
  * @param None
  * @retval Timer kernel clock frequency
  */
uint32_t GetTimerCLKFreq(void)
{
  uint32_t timer_clock_freq = 0; /* Timer clock frequency result */
  uint32_t TIMPRE_prescaler = 0;   /* APB2 prescaler */
  LL_RCC_ClocksTypeDef rcc_clocks = {0};

  /* Retrieve RCC clocks frequencies */
  LL_RCC_GetSystemClocksFreq(&rcc_clocks);

  /* Retrieve TIM_PRESCALER prescaler */
  TIMPRE_prescaler = LL_RCC_GetTIMPrescaler();

  if (TIMPRE_prescaler == LL_RCC_TIM_PRESCALER_1)
  {
    /* If TIM_PRESCALER prescaler (TIMPRE) is equal to 1 */
    timer_clock_freq = rcc_clocks.SYSCLK_Frequency;
  }
  else if (TIMPRE_prescaler == LL_RCC_TIM_PRESCALER_2)
  {
    /* If TIM_PRESCALER prescaler (TIMPRE) is equal to 2 */
    timer_clock_freq = rcc_clocks.SYSCLK_Frequency / 2;
  }
  else if (TIMPRE_prescaler == LL_RCC_TIM_PRESCALER_4)
  {
    /* If TIM_PRESCALER prescaler (TIMPRE) is equal to 4 */
  }
  else if (TIMPRE_prescaler == LL_RCC_TIM_PRESCALER_8)
  {
    /* If TIM_PRESCALER prescaler (TIMPRE) is equal to 2 */
    timer_clock_freq = rcc_clocks.SYSCLK_Frequency / 8;
  }


  return timer_clock_freq;
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Update the timer update event period
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Change the update event period by modifying the autoreload value.        */
  /* In up-counting update event is generated at each counter overflow (when  */
  /* the counter reaches the auto-reload value).                              */
  /* Update event period is calculated as follows:                            */
  /*   Update_event = TIM1CLK /((PSC + 1)*(ARR + 1)*(RCR + 1))                */
  /*   where TIM1CLK is 600 MHz                                                */
  AutoreloadMult = AutoreloadMult % TIM_BASE_FREQ_NB;
  LL_TIM_SetAutoReload(TIM1, tim_period * (AutoreloadMult +1));

  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM1);

  AutoreloadMult++;
}

/**
  * @brief  Timer update interrupt processing
  * @param  None
  * @retval None
  */
void TimerUpdate_Callback(void)
{
  LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);  
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
