/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @author  GPM Application Team
  * @brief   This sample code shows how to use STM32N6xx FMC HAL API
  *          to store, load then run an executable binary
  *          from a Toshiba NAND device TC58BVG2S0HBAI4.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_C
#define __MAIN_C
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
#define NAND_MANUFACTURER_CODE 0x98
#define NAND_DEVICE_CODE       0xDC
#define NAND_THIRD_ID          0x90
#define NAND_FOURTH_ID         0x26

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

NAND_HandleTypeDef hnand1;

/* USER CODE BEGIN PV */
#pragma location="application_buffer"
uint8_t aExecBuffer[NUMBER_PAGES_APPLICATION*NAND_PAGE_SIZE] = {0x00};

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
NAND_IDTypeDef NAND_ID;
NAND_AddressTypeDef Address;

pFunction Jump_To_Application;
__IO uint32_t JumpAddress;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FMC_Init(void);
/* USER CODE BEGIN PFP */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static void MPU_Config(void);
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
  uint32_t status = 0;
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
  MX_FMC_Init();
  /* USER CODE BEGIN 2 */

  /*#-STEP1-#- Configure the MPU ############################################*/
  MPU_Config();

  /*#-STEP2-#- NAND Loader Utility ##########################################*/

  /*##1- Initialize FMC to manage a NAND device  ############################*/

  /*##2- Verify that NAND device is ready  ##################################*/
  do
  {
    status = HAL_NAND_Read_Status(&hnand1);
  } while (status == NAND_BUSY);

  if (status == NAND_ERROR)
  {
    return 1;
  }

  /*##3- Retrieve and compare a NAND device identifier ######################*/
  HAL_NAND_Read_ID(&hnand1, &NAND_ID);

  if ((NAND_ID.Maker_Id  != NAND_MANUFACTURER_CODE) ||
      (NAND_ID.Device_Id != NAND_DEVICE_CODE)       ||
      (NAND_ID.Third_Id  != NAND_THIRD_ID)          ||
      (NAND_ID.Fourth_Id != NAND_FOURTH_ID))
  {
    Error_Handler();
  }

  /*##4- Erase Block area in a NAND device  #################################*/
  /* Initialize the nand address  */
  Address.Block = 0;
  Address.Page  = 0;
  Address.Plane  = 0;

  /* Erase the NAND Block  */
  HAL_NAND_Erase_Block(&hnand1, &Address);

  /*##5- Verify that NAND device is ready  ##################################*/
  do
  {
    status = HAL_NAND_Read_Status(&hnand1);
  } while (status == NAND_BUSY);

  if (status == NAND_ERROR)
  {
    Error_Handler();
  }

  /*##6- Write the application buffer content inside a NAND device  #########*/
  HAL_NAND_Write_Page_8b(&hnand1, &Address, aAppliBuffer, NUMBER_PAGES_APPLICATION);

  do
  {
    status = HAL_NAND_Read_Status(&hnand1);
  } while (status == NAND_BUSY);

  if (status == NAND_ERROR)
  {
    Error_Handler();
  }

  /*#-STEP3-#- Load And Run #################################################*/

  /*##1- Retrieve data from a NAND device  ##################################*/
  HAL_NAND_Read_Page_8b(&hnand1, &Address, aExecBuffer, NUMBER_PAGES_APPLICATION);

  /*##2- Verify consistency of Data retrieve from a NAND Device
         with Buffer use during NAND Loader phase ###########################*/
  if (Buffercmp((uint8_t *)aAppliBuffer, (uint8_t *)aExecBuffer, EXECBUFFERSIZE))
  {
    Error_Handler();
  }

  /*##3- Jump to code loaded from NAND memory
         to the buffer aExecBuffer and execute it ###########################*/
  JumpAddress = *(__IO uint32_t*) (aExecBuffer + 4);
  Jump_To_Application = (pFunction)JumpAddress;

  Jump_To_Application();
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

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
  FMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the NAND1 memory initialization sequence
  */
  hnand1.Instance = FMC_NAND_DEVICE;
  /* hnand1.Init */
  hnand1.Init.NandBank = FMC_NAND_BANK1;
  hnand1.Init.Waitfeature = FMC_NAND_WAIT_FEATURE_DISABLE;
  hnand1.Init.MemoryDataWidth = FMC_NAND_MEM_BUS_WIDTH_8;
  hnand1.Init.EccComputation = FMC_NAND_ECC_ENABLE;
  hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_512BYTE;
  hnand1.Init.TCLRSetupTime = 0;
  hnand1.Init.TARSetupTime = 2;
  /* hnand1.Config */
  hnand1.Config.PageSize = NAND_PAGE_SIZE;
  hnand1.Config.SpareAreaSize = NAND_SPARE_AREA_SIZE;
  hnand1.Config.BlockSize = NAND_BLOCK_SIZE;
  hnand1.Config.BlockNbr = NAND_BLOCK_NBR;
  hnand1.Config.PlaneNbr = NAND_PLANE_NBR;
  hnand1.Config.PlaneSize = NAND_PLANE_SIZE;
  hnand1.Config.ExtraCommandEnable = ENABLE;
  /* ComSpaceTiming */
  ComSpaceTiming.SetupTime = 2;
  ComSpaceTiming.WaitSetupTime = 10;
  ComSpaceTiming.HoldSetupTime = 7;
  ComSpaceTiming.HiZSetupTime = 0;
  /* AttSpaceTiming */
  AttSpaceTiming.SetupTime = 2;
  AttSpaceTiming.WaitSetupTime = 10;
  AttSpaceTiming.HoldSetupTime = 7;
  AttSpaceTiming.HiZSetupTime = 0;

  if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  if (HAL_NAND_ConfigDevice(&hnand1, &hnand1.Config) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOP_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_PWREx_EnableVddIO4();
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_EnableVddIO3();
  HAL_PWREx_EnableVddIO5();
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Compare two buffers.
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

/**
  * @brief  This function configures the MPU context of the application.
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  MPU_Attributes_InitTypeDef   MPU_AttributesInit;
  uint32_t address;
  uint32_t size;

  /* Disable MPU */
  HAL_MPU_Disable();

  /* Define Device nGnRnE attribute */
  MPU_AttributesInit.Number             = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes         = MPU_DEVICE_NGNRNE;
  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  /* Configure NAND region as Region Number 0, R/W region with execution */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x37EFF000UL;
  MPU_InitStruct.LimitAddress = 0x37EFFfffUL;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Initialize the non cacheable region for Application executable buffer*/
#if defined ( __ICCARM__ )
  /* get the region attribute form the icf file */
  extern uint32_t __APPLICATIONBUFFER_start;
  extern uint32_t __APPLICATION_size;

  address = (uint32_t)&__APPLICATIONBUFFER_start;
  size = (uint32_t)&__APPLICATION_size;
#else
#error "Compiler toolchain is unsupported"
#endif

  /* create an attribute configuration for the MPU */
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_R_ALLOCATE);
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER1;

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);

  /* TEX=1, Normal memory type, code execution allowed */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = address;
  MPU_InitStruct.LimitAddress = address+size;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#undef __MAIN_C
#endif
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
  while(1)
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
  /* Prevent unused argument(s) compilation warning */
  UNUSED(file);
  UNUSED(line);

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
