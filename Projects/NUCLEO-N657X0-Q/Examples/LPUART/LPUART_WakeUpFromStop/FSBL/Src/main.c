/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    UART/LPUART_WakeUpFromStop/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use UART through the STM32N6xx
  *          HAL API (LPUART instance) to wake up the MCU from STOP mode
  *          Two boards are used, one which enters STOP mode and the second
  *          one which sends the wake-up stimuli.
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
/* to enable for the board entering STOP mode,
   to disable for the board sending wake-up stimuli */
#define BOARD_IN_STOP_MODE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */
UART_WakeUpTypeDef WakeUpSelection;
__IO uint32_t UserButtonStatus = 0;  /* set to 1 after User Button interrupt  */

/* Buffer used for confirmation messages transmission */
uint8_t aTxBuffer1[] = "RXNE wake-up successful";
uint8_t aTxBuffer2[] = "Start bit detection wake-up successful";
uint8_t aTxBuffer3[] = "7-bit address match wake-up successful";
uint8_t aTxBuffer4[] = "4-bit address match wake-up successful";

uint8_t aTxBuffer[] = "Start bit detection wake-up successful";
uint8_t WakeUpTrigger1 = 'S'; /* Whatever the value */
uint8_t WakeUpTrigger2 = 'T'; /* Whatever the value */
uint8_t WakeUpTrigger3 = 0xA9; /* Value matches with address 0x29 */
uint8_t WakeUpTrigger4 = 0x82; /* Value matches with address 0x02 */

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
/* USER CODE BEGIN PFP */
#if defined(BOARD_IN_STOP_MODE)
void SystemClock_Config_fromSTOP(void);
#else
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
#endif /* defined(BOARD_IN_STOP_MODE) */
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /*##########################################################################*/
  /*##-1- Wake Up first step  ################################################*/
  /*##########################################################################*/

  /* Configure LED1*/
  BSP_LED_Init(LED1);

#ifdef BOARD_IN_STOP_MODE
  /* LED1 is on till stop mode */
  BSP_LED_On(LED1);

  /* Specify HSI as the clock source used after wake up from stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

  /* wait for two seconds before test start */
  HAL_Delay(2000);
  __HAL_RCC_LPUART1_CLK_SLEEP_ENABLE();
  /* make sure that no LPUART transfer is on-going */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_BUSY) == SET);
  /* make sure that LPUART is ready to receive
   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_REACK) == RESET);

  /* set the wake-up event:
   * specify wake-up on RXNE flag */
  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_READDATA_NONEMPTY;
  if (HAL_UARTEx_StopModeWakeUpSourceConfig(&hlpuart1, WakeUpSelection) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the UART Wake UP from STOP mode Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_WUF);

  /* about to enter STOP mode: switch off LED1 */
  BSP_LED_Off(LED1);

  /* enable MCU wake-up by LPUART */
  HAL_UARTEx_EnableStopMode(&hlpuart1);
  /* enter STOP mode */
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* ... STOP mode ... */


  /* at that point, MCU has been awoken: LED1 has been turned back on */
  SystemClock_Config_fromSTOP();

  /* Wake Up based on RXNE flag successful */
  HAL_UARTEx_DisableStopMode(&hlpuart1);

  /* wait for some delay */
  HAL_Delay(100);

  /* Inform other board that wake up is successful */
  if (HAL_UART_Transmit(&hlpuart1, (uint8_t *)aTxBuffer1, COUNTOF(aTxBuffer1) - 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /*##########################################################################*/
  /*##-2- Wake Up second step  ###############################################*/
  /*##########################################################################*/
  
  /* make sure that no LPUART transfer is on-going */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_BUSY) == SET);

  /* make sure that LPUART is ready to receive
   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_REACK) == RESET);

  /* set the wake-up event:
   * specify wake-up on start-bit detection */
  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
  if (HAL_UARTEx_StopModeWakeUpSourceConfig(&hlpuart1, WakeUpSelection) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the LPUART Wake UP from STOP mode Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_WUF);

  /* about to enter STOP mode: switch off LED1 */
  BSP_LED_Off(LED1);
  /* enable MCU wake-up by LPUART */
  HAL_UARTEx_EnableStopMode(&hlpuart1);
  /* enter STOP mode */
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* ... STOP mode ... */


  /* at that point, MCU has been awoken: LED1 has been turned back on */
  SystemClock_Config_fromSTOP();

  /* Wake Up on start bit detection successful */
  HAL_UARTEx_DisableStopMode(&hlpuart1);
  /* wait for some delay */
  HAL_Delay(100);
  /* Inform other board that wake up is successful */
  if (HAL_UART_Transmit(&hlpuart1, (uint8_t *)aTxBuffer2, COUNTOF(aTxBuffer2) - 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /*##########################################################################*/
  /*##-3- Wake Up third step  ################################################*/
  /*##########################################################################*/

  /* make sure that no LPUART transfer is on-going */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_BUSY) == SET);

  /* make sure that LPUART is ready to receive
   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_REACK) == RESET);

  /* set the wake-up event:
   * specify address-to-match type.
   * The address is 0x29, meaning the character triggering the
   * address match is 0xA9 */
  WakeUpSelection.WakeUpEvent   = UART_WAKEUP_ON_ADDRESS;
  WakeUpSelection.AddressLength = UART_ADDRESS_DETECT_7B;
  WakeUpSelection.Address       = 0x29;
  if (HAL_UARTEx_StopModeWakeUpSourceConfig(&hlpuart1, WakeUpSelection) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the LPUART Wake UP from stop mode Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_WUF);

  /* about to enter stop mode: switch off LED1 */
  BSP_LED_Off(LED1);
  /* enable MCU wake-up by LPUART */
  HAL_UARTEx_EnableStopMode(&hlpuart1);
  /* enter STOP mode */
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* ... STOP mode ... */


  /* at that point, MCU has been awoken: LED1 has been turned back on */
  SystemClock_Config_fromSTOP();

  /* Wake Up on 7-bit address detection successful */
  HAL_UARTEx_DisableStopMode(&hlpuart1);
  /* wait for some delay */
  HAL_Delay(100);
  /* Inform other board that wake up is successful */
  if (HAL_UART_Transmit(&hlpuart1, (uint8_t *)aTxBuffer3, COUNTOF(aTxBuffer3) - 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /*##########################################################################*/
  /*##-4- Wake Up fourth step  ###############################################*/
  /*##########################################################################*/

  /* make sure that no LPUART transfer is on-going */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_BUSY) == SET);

  /* make sure that LPUART is ready to receive
   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
  while (__HAL_UART_GET_FLAG(&hlpuart1, USART_ISR_REACK) == RESET);

  /* set the wake-up event:
   * specify address-to-match type.
   * The address is 0x2, meaning the character triggering the
   * address match is 0x82 */
  WakeUpSelection.WakeUpEvent   = UART_WAKEUP_ON_ADDRESS;
  WakeUpSelection.AddressLength = UART_ADDRESS_DETECT_4B;
  WakeUpSelection.Address       = 0x2;
  if (HAL_UARTEx_StopModeWakeUpSourceConfig(&hlpuart1, WakeUpSelection) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the LPUART Wake UP from STOP mode Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_WUF);

  /* about to enter STOP mode: switch off LED1 */
  BSP_LED_Off(LED1);
  /* enable MCU wake-up by LPUART */
  HAL_UARTEx_EnableStopMode(&hlpuart1);
  /* enter STOP mode */
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* ... STOP mode ... */


  /* at that point, MCU has been awoken: LED1 has been turned back on */
  SystemClock_Config_fromSTOP();

  /* Wake Up on 4-bit address detection successful */
  /* wait for some delay */
  HAL_Delay(100);
  /* Inform other board that wake up is successful */
  if (HAL_UART_Transmit(&hlpuart1, (uint8_t *)aTxBuffer4, COUNTOF(aTxBuffer4) - 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

#else
  /*##########################################################################*/
  /*##-1- Prepare the wake-up from stop mode #################################*/
  /*##########################################################################*/

  /* initialize the User push-button in Interrupt mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Wait for User push-button press before starting the test.
     In the meantime, LED1 is blinking (100ms delay)*/
  while (UserButtonStatus == 0)
  {
    /* Toggle LED1 */
    BSP_LED_Toggle(LED1);
    HAL_Delay(100);
  }
  /*##########################################################################*/
  /*##-2- Send the wake-up from stop mode first trigger ######################*/
  /*##    (RXNE flag setting)                           ######################*/
  /*##########################################################################*/
  BSP_LED_On(LED1);
  if (HAL_UART_Transmit(&hlpuart1, &WakeUpTrigger1, 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /* Put LPUART peripheral in reception process to wait for other board
     wake up confirmation */
  if (HAL_UART_Receive(&hlpuart1, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer1) - 1, 10000) != HAL_OK)
  {
    Error_Handler();
  }
  BSP_LED_Off(LED1);

  /* Compare the expected and received buffers */
  if (Buffercmp((uint8_t *)aTxBuffer1, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer1) - 1))
  {
    Error_Handler();
  }

  /* wait for two seconds before test second step */
  HAL_Delay(2000);
  /*##########################################################################*/
  /*##-3- Send the wake-up from stop mode second trigger #####################*/
  /*##    (start Bit detection)                          #####################*/
  /*##########################################################################*/
  BSP_LED_On(LED1);
  if (HAL_UART_Transmit(&hlpuart1, &WakeUpTrigger2, 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /* Put LPUART peripheral in reception process to wait for other board
     wake up confirmation */
  if (HAL_UART_Receive(&hlpuart1, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer2) - 1, 10000) != HAL_OK)
  {
    Error_Handler();
  }
  BSP_LED_Off(LED1);

  /* Compare the expected and received buffers */
  if (Buffercmp((uint8_t *)aTxBuffer2, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer2) - 1))
  {
    Error_Handler();
  }

  /* wait for two seconds before test third step */
  HAL_Delay(2000);
  /*##########################################################################*/
  /*##-4- Send the wake-up from stop mode third trigger ######################*/
  /*##    (7-bit address match)                         ######################*/
  /*##########################################################################*/
  BSP_LED_On(LED1);
  if (HAL_UART_Transmit(&hlpuart1, &WakeUpTrigger3, 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /* Put LPUART peripheral in reception process to wait for other board
     wake up confirmation */
  if (HAL_UART_Receive(&hlpuart1, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer3) - 1, 10000) != HAL_OK)
  {
    Error_Handler();
  }
  BSP_LED_Off(LED1);

  /* Compare the expected and received buffers */
  if (Buffercmp((uint8_t *)aTxBuffer3, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer3) - 1))
  {
    Error_Handler();
  }

  /* wait for two seconds before test fourth and last step */
  HAL_Delay(2000);

  /*##########################################################################*/
  /*##-5- Send the wake-up from stop mode fourth trigger #####################*/
  /*##    (4-bit address match)                          #####################*/
  /*##########################################################################*/
  BSP_LED_On(LED1);
  if (HAL_UART_Transmit(&hlpuart1, &WakeUpTrigger4, 1, 5000) != HAL_OK)
  {
    Error_Handler();
  }


  /* Put LPUART peripheral in reception process to wait for other board
     wake up confirmation */
  if (HAL_UART_Receive(&hlpuart1, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer4) - 1, 10000) != HAL_OK)
  {
    Error_Handler();
  }
  BSP_LED_Off(LED1);

  /* Compare the expected and received buffers */
  if (Buffercmp((uint8_t *)aTxBuffer4, (uint8_t *)aRxBuffer, COUNTOF(aTxBuffer4) - 1))
  {
    Error_Handler();
  }

  HAL_Delay(2000);

#endif /* BOARD_IN_STOP_MODE */


  /* Turn on LED1 if test passes then enter infinite loop */
  BSP_LED_On(LED1);
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
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */
#ifdef BOARD_IN_STOP_MODE
  LPUART1_EXTI_ENABLE_IT();
#endif /* BOARD_IN_STOP_MODE */
  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#if defined(BOARD_IN_STOP_MODE)
/**
  * @brief  System Clock Configuration restored after STOP mode
  * @param  None
  * @retval None
  */
void SystemClock_Config_fromSTOP(void)
{
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//
//  /* Get the Oscillators configuration from the internal RCC registers */
//  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
//  /* Wake up on HSI, re-enable PLL with HSI as source */
//  /* Oscillator configuration unchanged */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
//  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* Get the clock prescalers configuration from the internal RCC registers */
//  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
//  /* Select PLL as system clock source */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//*******************************************
//  SystemClock_Config();
//******************************************
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the System Power Supply
  */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif
/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  Error_Handler();
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_USER_PIN)
  {
    UserButtonStatus = 1;
  }
}

/**
  * @brief UART wakeup from Stop mode callback
  * @param huart: uart handle
  * @retval None
  */
void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart)
{
  /* Turn back on LED1 */
  BSP_LED_On(LED1);
}


#if !defined(BOARD_IN_STOP_MODE)
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
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
#endif /* !defined(BOARD_IN_STOP_MODE) */
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
    /* In case of error, LED1 transmits a sequence of three dots, three dashes, three dots */
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(300);
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(300);
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(300);
    BSP_LED_On(LED1);
    HAL_Delay(700);
    BSP_LED_Off(LED1);
    HAL_Delay(700);
    BSP_LED_On(LED1);
    HAL_Delay(700);
    BSP_LED_Off(LED1);
    HAL_Delay(700);
    BSP_LED_On(LED1);
    HAL_Delay(700);
    BSP_LED_Off(LED1);
    HAL_Delay(700);
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(300);
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(300);
    BSP_LED_On(LED1);
    HAL_Delay(300);
    BSP_LED_Off(LED1);
    HAL_Delay(800);
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
