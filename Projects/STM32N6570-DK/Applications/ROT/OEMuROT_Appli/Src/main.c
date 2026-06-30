/**
  ******************************************************************************
  * @file    AppliSecure/Src/main.c
  * @author  GPM Application Team
  * @brief   Secure main program.
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2024 STMicroelectronics
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6xx_it.h"
#include "low_level_ext_flash.h"
#include "appli_flash_layout.h"
#include "secure_nsc.h"
#include "common.h"
#include "com.h"
#include "fw_update_app.h"
#include "ns_data.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Uncomment to enable boot time print */
/* #define PRINT_BOOT_TIME */
#define EXT_FLASH_BASE_ADDRESS         XSPI2_BASE          /* External Flash (XSPI2 + MCE2 - AES) */

/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';
extern ARM_DRIVER_FLASH Driver_EXT_FLASH0;
#ifdef PRINT_BOOT_TIME
static uint64_t time;
static uint32_t end;
#endif

/* Private function prototypes -----------------------------------------------*/
void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);

void SecureFault_Callback(void);
void SecureError_Callback(void);

void MPU_Config(void);
void LL_SECU_DisableCleanMpu(void);

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
int io_putchar(int ch);
#define PUTCHAR_PROTOTYPE int io_putchar(int ch)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int32_t uart_putc(int32_t ch)
#endif /* __ICCARM__ */

PUTCHAR_PROTOTYPE
{
  COM_Transmit((uint8_t*)&ch, 1, TX_TIMEOUT);
  return ch;
}

/* Redirects printf to DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
FILE __stdout;

/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
  /* Send byte to USART */
  io_putchar(ch);

  /* Return character written */
  return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
  int i;

  for (i = 0; i < len; i++)
  {
    /* Send byte to USART */
    uart_putc(str[i]);
  }

  /* Return the number of characters written */
  return len;
}
#elif defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /*  __GNUC__ */
/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
#ifdef PRINT_BOOT_TIME
  /* Get boot cycles */
  end = DWT->CYCCNT;
#endif

  /*!!! To boot in a secure way, the RoT has configured and activated the Memory Protection Unit
  In order to keep a secure environment execution, you should reconfigure the
  MPU to make it compatible with your application.
  This example provides a basic configuration for the Memory Protection Unit*/
  MPU_Config();


  /* Enable I-Cache and D-Cache */
  SCB_EnableICache();
  SCB_EnableDCache();

  /* Enable BusFault and SecureFault handlers */
  SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk);

  /* Reset peripherals, initialize Flash interface and systick */
  HAL_Init();

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPION_CLK_ENABLE();
  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPIOP_CLK_ENABLE();
  __HAL_RCC_GPIOQ_CLK_ENABLE();

  /* Configure all GPIO pins as SECURE (do NOT set to non-secure) */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOC, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOF, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPION, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_ALL, GPIO_PIN_SEC);
  HAL_GPIO_ConfigPinAttributes(GPIOQ, GPIO_PIN_ALL, GPIO_PIN_SEC);
/* Set USART1 as configurable by secure */
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_USART1, RIF_ATTRIBUTE_SEC);
#if (DOWNLOAD_MENU == 1)
  Driver_EXT_FLASH0.Initialize(NULL);
#endif /* DOWNLOAD_MENU == 1 */

  /* Initialize communication interface */
  COM_Init();

#ifdef PRINT_BOOT_TIME
  /* Calculate boot time */
  time = ((uint64_t)(end) * 1000U / SystemCoreClock);
  printf("\r\nBoot time : %u ms at %u MHz", (unsigned int)(time), (unsigned int)(SystemCoreClock/1000000U));
  printf("\r\n");
#endif

  /* Print banner */
  pUserAppId = (uint8_t *)&UserAppId;
  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2024 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                          User App #%c                               =", *pUserAppId);
  printf("\r\n======================================================================");
  printf("\r\n\r\n");

  /* Run the integrated application main loop */
  FW_APP_Run();

  /* Should never reach here */
  while (1)
  {
    __NOP();
  }
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n=================== Main Menu ============================\r\n\n");
#if (NS_DATA_IMAGE_NUMBER == 1)
  printf("  Non-Secure Data --------------------------------------- 1\r\n\n");
#endif
#if (DOWNLOAD_MENU == 1)
  printf("  New Fw Image ------------------------------------------ 2\r\n\n");
#endif
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Main menu loop for user input and app execution
  */
void FW_APP_Run(void)
{
  uint8_t key = 0U;

  FW_APP_PrintMainMenu();

  while (1U)
  {
    COM_Flush();

    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
#if (DOWNLOAD_MENU == 1)
        case '2' :
          FW_UPDATE_Run();
          break;
#endif
        default:
          printf("Invalid Number !\r");
          break;
      }
      FW_APP_PrintMainMenu();
    }
  }
}

/**
  * @brief  Secure Fault callback
  */
void SecureFault_Callback(void)
{
  while (1) { }
}

/**
  * @brief  Secure Error callback
  */
void SecureError_Callback(void)
{
  while (1) { }
}

/**
  * @brief  MPU configuration for secure application
  */

void MPU_Config(void)
{
  /* Disable the MPU to allow configuration changes */
  HAL_MPU_Disable();
  
  /* Disable and clean previous MPU config */
  LL_SECU_DisableCleanMpu();

  MPU_Region_InitTypeDef default_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t primask_bit = __get_PRIMASK();


  /* Create attribute configuration for the MPU */
  attr_config.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  attr_config.Number = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  attr_config.Attributes = INNER_OUTER(MPU_NO_ALLOCATE);
  attr_config.Number = MPU_ATTRIBUTES_NUMBER1;
  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  /* Common settings for all MPU regions configured below */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  /* Configure a non cacheable Flash region (Primary and Secondary slots: Appli Secure and Nonsecure, Data Secure and Nonsecure) */
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  default_config.Number = MPU_REGION_NUMBER0;
  default_config.BaseAddress = EXT_FLASH_BASE_ADDRESS + S_APPLI_OFFSET;
  default_config.LimitAddress = EXT_FLASH_BASE_ADDRESS + S_APPLI_SECONDARY_OFFSET + S_DATA_PARTITION_SIZE + NS_DATA_PARTITION_SIZE + NS_APPLI_PARTITION_SIZE + S_APPLI_PARTITION_SIZE - 1;
  HAL_MPU_ConfigRegion(&default_config);

  /* Configure The SRAM2 */
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  default_config.Number = MPU_REGION_NUMBER1;
  default_config.BaseAddress = SRAM2_AXI_BASE_S;
  default_config.LimitAddress =SRAM2_AXI_BASE_S + SRAM2_AXI_SIZE - 1;
  HAL_MPU_ConfigRegion(&default_config);

  /* Configure The SRAM1 */
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  default_config.Number = MPU_REGION_NUMBER2;
  default_config.BaseAddress = SRAM1_AXI_BASE_S;
  default_config.LimitAddress =SRAM1_AXI_BASE_S + SRAM1_AXI_SIZE - 1;
  HAL_MPU_ConfigRegion(&default_config);

  /* enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  /* Exit critical section to lock the system and avoid any issue around MPU mechanisme */
  __set_PRIMASK(primask_bit);
}

/**
  * @brief  Disable and clean MPU regions
  */
void LL_SECU_DisableCleanMpu(void)
{
  for (uint8_t i = MPU_REGION_NUMBER0; i <= MPU_REGION_NUMBER15; i++)
  {
    HAL_MPU_DisableRegion(i);
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  while (1) { }
}
#endif