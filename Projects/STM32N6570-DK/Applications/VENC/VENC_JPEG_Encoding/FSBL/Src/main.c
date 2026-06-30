/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   VENC JPEG encoding application entry point.
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
#include <string.h>
#include <stdio.h>
#include "jpegencapi.h"

/** @addtogroup STM32N6xx_HAL_Template
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
    

#pragma pack(2)
typedef struct _tagBITMAPINFOHEADER{
	unsigned long      biSize;
	long       biWidth;
	long       biHeight;
	unsigned short       biPlanes;
	unsigned short       biBitCount;
	unsigned long      biCompression;
	unsigned long      biSizeImage;
	long       biXPelsPerMeter;
	long       biYPelsPerMeter;
	unsigned long      biClrUsed;
	unsigned long      biClrImportant;
} T_BITMAPINFOHEADER;



typedef struct _tagBITMAPFILEHEADER {
	unsigned short    bfType;
	unsigned long   bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned long   bfOffBits;
} T_BITMAPFILEHEADER;
#pragma pack()

    

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
    
#define DIB_HEADER_MARKER   ((unsigned short) ('M' << 8) | 'B')
#define	PADPIXEL2BYTE(dx,depth)    ((dx * depth + 8-1)/8)
#define	PAD4BYTES(dx)			    (((dx+4-1) / 4)*4)
#define BI_RGB                          0
#define BI_BITFIELDS                    3


  
#if defined(__ICCARM__) /*!< IAR Compiler */
  #pragma segment = "RW_NOCACHERAM"
  #define SECTION_DMA_OFF  ((uint32_t)__segment_begin("RW_NOCACHERAM"))
  #define SECTION_DMA_END  ((uint32_t)__segment_end("RW_NOCACHERAM"))
  #define SECTION_DMA_SIZE (SECTION_DMA_END   -  SECTION_DMA_OFF) 
#elif defined(__ARMCC_VERSION)
  extern uint32_t Image$$RW_NOCACHERAM$$Base;
  extern uint32_t Image$$RW_NOCACHERAM$$Limit;
  #define SECTION_DMA_OFF ((uint32_t) &Image$$RW_NOCACHERAM$$Base)
  #define SECTION_DMA_END   ((uint32_t)&Image$$RW_NOCACHERAM$$Limit)
  #define SECTION_DMA_SIZE (SECTION_DMA_END   -  SECTION_DMA_OFF)
#elif defined(__GNUC__) || defined(__CC_ARM)
  extern uint8_t _snocacheram[];
  extern uint8_t _enocacheram[];
  #define SECTION_DMA_OFF  ((uint32_t)_snocacheram)
  #define SECTION_DMA_END  ((uint32_t)_enocacheram)
  #define SECTION_DMA_SIZE (SECTION_DMA_END   -  SECTION_DMA_OFF) 
#else
  #error "Tool chain not supported"
#endif
  
  
#if defined ( __ICCARM__ )
#define VAR_DECLARE_ALIGN_AT_SECTION(type, var, section)   \
_Pragma("data_alignment=16") type var @ section
#elif defined(__GNUC__)
#define VAR_DECLARE_ALIGN_AT_SECTION(type, var, section)   \
__attribute__((__section__(section)))                       \
type var __attribute__ ((aligned (16)))
#elif defined(__CC_ARM)
#define VAR_DECLARE_ALIGN_AT_SECTION(type, var, section)   \
__attribute__((__section__(section)))                       \
__align(16) type var
#else
#error "unsupported compiler"
#endif
  
    

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/



/* USER CODE BEGIN PV */
/* Buffer used for transmission */
/* Buffer used for reception */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MPU_Setup(void);

static  uint8_t   AttrbNCache = MPU_ATTRIBUTES_NUMBER1;
static  uint8_t   AttrbCache  = MPU_ATTRIBUTES_NUMBER2;
static  uint8_t   regionNumber= MPU_REGION_NUMBER1;
static   MPU_Attributes_InitTypeDef   attrMem;

#include "refImage_bmp_raw.c"
VAR_DECLARE_ALIGN_AT_SECTION(uint8_t,tJpegStream[100*1024],".NOCACHERAM_region");
VAR_DECLARE_ALIGN_AT_SECTION(uint8_t,tRawImage[200*1024],".NOCACHERAM_region");

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
int8_t  save_file(char *pName,uint8_t *pStream,uint32_t szOut)
{

  int8_t ret = 0;
#if defined ( __ICCARM__ )
  /* stdio FILE works only on IAR system */
  FILE *pFile;
  
  pFile = fopen(pName,"wb");
  if(pFile)
  {
      while(szOut)
      {
        uint32_t blk = 1024;
        if(szOut < blk )  blk = szOut;
        
        uint32_t written = fwrite(pStream,1,blk,pFile);
        szOut-= written;
        pStream+=written;
      }
      
      fclose(pFile);
      ret  = 1;
      printf("the file %s is in the current folder",pName);
    
    
  }
#endif
  return ret;
}



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();
  
  MPU_Setup();


  /* MCU Configuration--------------------------------------------------------*/

    /* STM32N6xx HAL library initialization:
      - SysTick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();
  

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_VENC  , &RIMC_master);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_VENC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  
  

  /* USER CODE BEGIN SysInit */
  /* Configure LED1 */
  BSP_LED_Init(LED1);
  /* Configure LED2 */
  BSP_LED_Init(LED2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* Turn on LED1 if test passes then enter infinite loop */
  BSP_LED_Off(LED1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  /* start BMP parsing */
  uint8_t *pRawData= (void *)refImage_bmp;
  
  T_BITMAPFILEHEADER h;
  
  memcpy(&h,pRawData,sizeof(h));
  pRawData+= sizeof(h);
  uint32_t szJpeg = 0;
 /* check signature */
 if(h.bfType == DIB_HEADER_MARKER && h.bfSize >= sizeof(T_BITMAPFILEHEADER)) 
  {
    uint32_t cptImage = 0;
    char tScratch[100];
    /* extract header */
    T_BITMAPINFOHEADER hbitheader;
    memcpy(&hbitheader,pRawData,sizeof(hbitheader));
    pRawData+= sizeof(hbitheader);
    /* if in photoshop , select flip row order size is negative */
    if(hbitheader.biHeight< 0) hbitheader.biHeight = - hbitheader.biHeight;
    if(hbitheader.biWidth< 0) hbitheader.biWidth = - hbitheader.biWidth;
    
    /* if in photoshop , select  advanced modes */
    if(hbitheader.biCompression == BI_BITFIELDS ) 
    {
      pRawData+= 4*sizeof(uint32_t);
    }
    /* copy in Ncache and aligned 16 */
    uint32_t szHeader = pRawData-refImage_bmp;
    memcpy(tRawImage,pRawData,sizeof(refImage_bmp)-szHeader); 
    
    /* init Venc with parameters */
    int8_t ret = encoder_jpeg_init(JPEGENC_RGB565,hbitheader.biWidth,hbitheader.biHeight,tJpegStream,sizeof(tJpegStream));
      
    if(ret != 0)
    {
      /* encode the frame */
      szJpeg = encode_jpeg_frame(tRawImage);
      if(szJpeg  ==0) ret = 0;
      else            ret = 1;
    }
    if(ret)
    {
      /* Save the jpeg file in the current folder */
      snprintf(tScratch,sizeof(tScratch),"Img%03lu.jpeg",(unsigned long)cptImage++);
      ret = save_file(tScratch,tJpegStream,szJpeg);
    }
    Led_TypeDef led = LED2;
    if(ret != 0) led = LED1;
    while(1)
    {
      BSP_LED_On(led );
      HAL_Delay(1000);
      BSP_LED_Off(led);
      HAL_Delay(1000);
    }
  }
  /* USER CODE END 3 */
}

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* Tuen LED1 Off and turn LED2 On for error */
  BSP_LED_Off(LED1);
  BSP_LED_On(LED2);
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
void   MPU_Set(int32_t regionNumber, uint32_t offset,uint32_t size, int8_t bCached)
{
  MPU_Region_InitTypeDef       regionMem;
  memset(&regionMem,0,sizeof(regionMem));
  
  /* BaseAddress-LimitAddress configuration */
  regionMem.Enable           = MPU_REGION_ENABLE;
  regionMem.Number           = regionNumber; /* last one */
  if(bCached)                regionMem.AttributesIndex  = AttrbCache;
  else                       regionMem.AttributesIndex  = AttrbNCache;
  regionMem.BaseAddress      = offset;
  regionMem.LimitAddress     = offset + (size*1024U)-1;
  regionMem.AccessPermission  = MPU_REGION_ALL_RW;
  regionMem.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  regionMem.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  HAL_MPU_ConfigRegion(&regionMem);
  
}





static void MPU_Setup(void)
{
  uint32_t primask_bit = __get_PRIMASK();
  
  
  HAL_MPU_Disable();
  
  
  
  /* Define cacheable memory via MPU */
  attrMem.Number             = AttrbNCache;
  attrMem.Attributes         = INNER_OUTER(MPU_NOT_CACHEABLE);
  HAL_MPU_ConfigMemoryAttributes(&attrMem);
  
  attrMem.Number             = AttrbCache;
  attrMem.Attributes         = INNER_OUTER( MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_RW_ALLOCATE);
  HAL_MPU_ConfigMemoryAttributes(&attrMem);
  uint32_t szDmaKb = (SECTION_DMA_SIZE + 1024UL-1UL)/1024UL;
  MPU_Set(regionNumber++,SECTION_DMA_OFF,szDmaKb ,0);
  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  /* Exit critical section to lock the system and avoid any issue around MPU mechanisme */
  __set_PRIMASK(primask_bit);
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
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

/**
  * @}
  */

/**
  * @}
  */

