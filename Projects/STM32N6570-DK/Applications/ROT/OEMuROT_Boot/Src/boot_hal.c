/**
  ******************************************************************************
  * @file    boot_hal.c
  * @author  MCD Application Team
  * @brief   This file contains  mcuboot stm32n6xx hardware specific implementation
  *
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal.h"
#include "boot_hal_hash_ref.h"
#include "boot_hal_imagevalid.h"
#include "boot_hal_flowcontrol.h"
#include "mcuboot_config/mcuboot_config.h"
#include "uart_stdout.h"
#include "low_level_security.h"
#include "bootutil/boot_record.h"
#include "target_cfg.h"
#include "cmsis.h"
#include "Driver_Flash.h"
#include "region_defs.h"
#include "low_level_rng.h"
#include "low_level_otp.h"
#include "low_level_ext_flash.h"
#include "low_level_ext_ram.h"
#include "low_level_mce.h"
#include "debug_authentication.h"
#include "bootutil_priv.h"
#include "bootutil/crypto/sha256.h"

extern ARM_DRIVER_FLASH EXT_FLASH_DEV_NAME;

#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
/* Global variables to memorize images validation status */
#if (MCUBOOT_IMAGE_NUMBER == 1)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 2)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 3)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 4)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#else
#error "MCUBOOT_IMAGE_NUMBER not supported"
#endif /* (MCUBOOT_IMAGE_NUMBER == 1) */
uint8_t ImageValidIndex = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF) || defined(MCUBOOT_USE_HASH_REF)
uint8_t ImageValidEnable = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF || MCUBOOT_USE_HASH_REF */

#if defined(MCUBOOT_USE_HASH_REF)
uint8_t ImageValidHashUpdate = 0;
uint8_t ImageValidHashRef[MCUBOOT_IMAGE_NUMBER * SHA256_LEN] = {0};
#endif /* MCUBOOT_USE_HASH_REF */

#if defined(FLOW_CONTROL)
/* Global variable for Flow Control state */
volatile uint32_t uFlowProtectValue = FLOW_CTRL_INIT_VALUE;
#endif /* FLOW_CONTROL */
volatile uint32_t uFlowStage = FLOW_STAGE_CFG;

#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLAG_SIZE 16
#define CONF_FLAG_ERASE_VALUE 0x00
const uint8_t boot_conf_flag_pattern[FLAG_SIZE] =
{
  0xb7, 0xbb, 0x1f, 0x00, 0x45, 0x64, 0x23, 0x00,
  0x89, 0xa8, 0x23, 0x00, 0x11, 0xd1, 0x23, 0x00
};
/* Place data in a specific section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ ".flagnoinit"
#elif defined(__ARMCC_VERSION)
__attribute__((section(".bss.flagnoinit")))
#elif defined(__GNUC__)
__attribute__((section(".flagnoinit")))
#endif /* __ICCARM__ */
volatile uint8_t boot_conf_flag[FLAG_SIZE];
/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#endif /* __ICCARM__ */
#endif /* not MCUBOOT_OVERWRITE_ONLY */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup BOOT_HAL_Private_Functions  Private Functions
  * @{
  */
static void boot_jump_to_image(uint32_t boot_jump_addr, uint32_t reset_handler_addr);
bool boot_is_in_primary(uint8_t fa_id, uint32_t offset_in_flash, size_t len);
bool boot_is_in_primary_and_erased(uint8_t fa_id, uint32_t offset_in_flash, size_t len, uint8_t erased_val);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
int boot_check_conf_flag(void);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if defined(MCUBOOT_USE_HASH_REF)
static int boot_compute_run_img_hash(void);
#endif /* MCUBOOT_USE_HASH_REF */
/**
  * @}
  */
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Jump_Code"
#else
__attribute__((section(".BL2_Jump_Code")))
#endif /* __ICCARM__ */
/**
  * @brief  Jumping to image after erasing boot DATA area twice
  * @param  boot_jump_addr : jump address in order to execute erasing twice
  * @param  reset_handler_addr : image vector table
  */
void boot_jump_to_image(uint32_t boot_jump_addr, uint32_t reset_handler_addr)
{
  __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                 \n"
#endif /* not __ICCARM__ */
    "mov     r7, r0                  \n"
    "mov     r8, r1                  \n"
    "bl      boot_clear_bl2_ram_area \n" /* Clear RAM before jump */
    "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
    "mov     r1, r0                  \n" /* except R7 */
    "mov     r2, r0                  \n"
    "mov     r3, r0                  \n"
    "mov     r4, r0                  \n"
    "mov     r5, r0                  \n"
    "mov     r6, r0                  \n"
    "mov     r9, r0                  \n"
    "mov     r10, r0                 \n"
    "mov     r11, r0                 \n"
    "mov     r12, r0                 \n"
    "mov     lr,  r0                 \n"
    "mov     r0, r8                  \n"
    "mov     r8, r1                  \n"
    "bx      r7                      \n" /* Jump to image*/
  );
}

/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_Jump_Code")))
#endif /* __GNUC__ */

/**
  * @brief This function is called to clear all RAM area before jumping in
  * in Secure application .
  * @note
  * @retval void
  */
void boot_clear_bl2_ram_area(void)
{
  /* Clean code area */
  __IO uint32_t *pt = (uint32_t *)(BL2_CODE_START);
  uint32_t index;

  for (index = 0; index < (BL2_CODE_SIZE / 4); index++)
  {
    pt[index] = 0;
  }

  /* Clean data area */
  pt = (uint32_t *)BL2_DATA_START;
  for (index = 0; index < (BL2_DATA_SIZE / 4); index++)
  {
    pt[index] = 0;
  }

  /* Clean AHBSRAM2 */
  pt = (uint32_t *)SRAM2_AHB_BASE_S;
  for (index = 0; index < (SRAM2_AHB_SIZE / 4); index++)
  {
    pt[index] = 0;
  }
}

/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_Jump_Code")))
#endif /* __GNUC__ */

/**
  * @brief This function manage the jump to secure application.
  * @note
  * @retval void
  */
void boot_platform_quit(struct boot_arm_vector_table *vector)
{
  static struct boot_arm_vector_table *vt;

  vt = (struct boot_arm_vector_table *)vector;

#if  (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  struct image_header *img_hdr;
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
  uint32_t image_index;

  (void)fih_delay();
  /* Check again if images have been validated, to resist to basic hw attacks */
  for (image_index = 0; image_index < MCUBOOT_IMAGE_NUMBER; image_index++)
  {
    if (ImageValidStatus[image_index] != IMAGE_VALID)
    {
      Error_Handler();
    }
  }
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */

#if defined(MCUBOOT_USE_HASH_REF)
  /* Store new hash references in flash for next boot */
  if (ImageValidHashUpdate)
  {
    if (boot_hash_ref_store())
    {
      Error_Handler();
    }
  }
#endif /* MCUBOOT_USE_HASH_REF */

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_2);
  uFlowStage = FLOW_STAGE_CFG;

#if (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  /* Get the header structure from application secure image */
  img_hdr = (struct image_header *)FLASH_APPLI_S_ADDRESS;
  memcpy((void *)S_CODE_START,
         (void *)(FLASH_APPLI_S_ADDRESS), img_hdr->ih_hdr_size + img_hdr->ih_img_size);

#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  /* Get the header structure from data secure image */
  img_hdr = (struct image_header *)FLASH_DATA_S_ADDRESS;
  memcpy((void *)S_DATA2_START,
         (void *)(FLASH_DATA_S_ADDRESS), img_hdr->ih_hdr_size + img_hdr->ih_img_size);
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */

#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
  /* Get the header structure from application non-secure image */
  img_hdr = (struct image_header *)FLASH_APPLI_NS_ADDRESS;
  memcpy((void *)NS_CODE_START,
         (void *)(FLASH_APPLI_NS_ADDRESS), img_hdr->ih_hdr_size + img_hdr->ih_img_size);
#endif

#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  /* Get the header structure from data non-secure image */
  img_hdr = (struct image_header *)FLASH_DATA_NS_ADDRESS;
  memcpy((void *)NS_DATA2_START,
         (void *)(FLASH_DATA_NS_ADDRESS), img_hdr->ih_hdr_size + img_hdr->ih_img_size);
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */

  vt = (struct boot_arm_vector_table *)(S_CODE_START + BL2_HEADER_SIZE);
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

#if defined(MCUBOOT_USE_HASH_REF)
  if (boot_compute_run_img_hash() != 0)
  {
    Error_Handler();
  }
#endif /* MCUBOOT_USE_HASH_REF */

  RNG_DeInit();

#ifdef OEMUROT_CACHE_ENABLE
  /* Disable I-Cache */
  SCB_DisableICache();
#endif /* OEMUROT_CACHE_ENABLE */

#if (OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  if (EXT_FLASH_DEV_NAME.Uninitialize() != ARM_DRIVER_OK)
  {
    Error_Handler();
  }
#endif /* OEMUROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

  /* Update run time protections for application execution */
  LL_SECU_UpdateRunTimeProtections();

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_3);
  uFlowStage = FLOW_STAGE_CHK;

  /* Double the update of run time protections, to resist to basic hardware attacks */
  LL_SECU_UpdateRunTimeProtections();

  /* Check Flow control for dynamic protections */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_4);

  /* Set the secure vector */
  SCB->VTOR = (uint32_t)vt;

  /*  Change stack limit  */
  __set_MSPLIM(0);
  __set_MSP(vt->msp);
  __DSB();
  __ISB();

  boot_jump_to_image((uint32_t)&boot_jump_to_image, (vt->reset));
  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while (1);
#endif /* !defined(__ICCARM__) */
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#if defined(MCUBOOT_USE_HASH_REF)
/**
  * @brief This function store all hash references in flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_store(void)
{
  /* Erase hash references flash sector */
  if (FLASH_DEV_NAME.EraseSector(FLASH_AREA_HASH_REF_OFFSET) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  /* Store new hash references in flash */
  if (FLASH_DEV_NAME.ProgramData(FLASH_AREA_HASH_REF_OFFSET, ImageValidHashRef,
      (SHA256_LEN * MCUBOOT_IMAGE_NUMBER)) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function load all hash references from flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_load(void)
{
  /* Read hash references */
  if (FLASH_DEV_NAME.ReadData(FLASH_AREA_HASH_REF_OFFSET, ImageValidHashRef,
      (SHA256_LEN * MCUBOOT_IMAGE_NUMBER)) != ARM_DRIVER_OK)
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function set one hash reference in ram
  * @param hash_ref hash reference to update
  * @param size size of the hash references
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_set(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Set hash reference */
  memcpy(ImageValidHashRef + (image_index * SHA256_LEN), hash_ref, SHA256_LEN);

  /* Memorize that hash references will have to be updated in flash (later) */
  ImageValidHashUpdate++;

  return 0;
}

/**
  * @brief This function get one hash reference from ram
  * @param hash_ref hash reference to get
  * @param size size of the hash reference
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_get(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Get hash reference */
  memcpy(hash_ref, ImageValidHashRef + (image_index * SHA256_LEN), SHA256_LEN);

  return 0;
}
#endif /* MCUBOOT_USE_HASH_REF */

/**
  * @brief This function check if a buffer is in primary code
  * @param fa_id identifier of the flash area
  * @param offset_in_flash offset of the buffer in the flash area
  * @param len size of the buffer
  * @return true if the buffer is in primary code; false otherwise.
  */
bool boot_is_in_primary(uint8_t fa_id, uint32_t offset_in_flash, size_t len)
{
  uint32_t buffer_in_flash;
  uint32_t primary_start = FLASH_PRIMARY_AREA_START_ADDRESS;
  uint32_t primary_end = FLASH_PRIMARY_AREA_LIMIT_ADDRESS;

  /* Check the fa_id */
  if (fa_id == FLASH_AREA_0_ID
  #if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
      || fa_id == FLASH_AREA_4_ID
  #endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
  #if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
      || fa_id == FLASH_AREA_5_ID
  #endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
  #if (MCUBOOT_APP_IMAGE_NUMBER == 2)
      || fa_id == FLASH_AREA_1_ID
  #endif /* MCUBOOT_APP_IMAGE_NUMBER == 2 */
      )
  {
      buffer_in_flash = EXT_FLASH_BASE_ADDRESS + offset_in_flash;
  }
  else
  {
    return false;
  }

  /* Check the address */
  if ((buffer_in_flash >= primary_start) && (buffer_in_flash <= primary_end))
  {
    return true;
  }
  if (((buffer_in_flash + len) >= primary_start) && ((buffer_in_flash + len) <= primary_end))
  {
    return true;
  }
  return false;
}

/**
  * @brief This function check if a buffer is erased, taking MCE into account
  * @param fa_id identifier of the flash area
  * @param offset_in_flash offset of the buffer in the flash area
  * @param len size of the buffer
  * @param erased_val value found in the erased flash
  * @return true if the buffer is erased; false otherwise.
  */
bool boot_is_in_primary_and_erased(uint8_t fa_id, uint32_t offset_in_flash, size_t len, uint8_t erased_val)
{
  size_t i;
  uint8_t *buffer_in_flash;
  uint8_t *u8b;
  bool crypted_content_erased = true;
  bool decrypted_content_erased = true;

  /* Check the location of the buffer */
  if (boot_is_in_primary(fa_id, offset_in_flash, len) == false)
  {
    return false;
  }

  /* Get the address */
  buffer_in_flash = (uint8_t *)(EXT_FLASH_BASE_ADDRESS + offset_in_flash);

  /* Check the buffer without checking MCE status:
   *   - MCE enabled: the buffer can have been forced to "erased_val"
   *   - MCE disabled: the external memory can be truly erased */
  for (i = 0, u8b = buffer_in_flash; i < len; i++)
  {
    if (u8b[i] != erased_val)
    {
      decrypted_content_erased = false;
    }
  }

  /* Check the buffer is in the encrypted area */
  if (LL_MCE_IsBufferInCodePrimaryRegion(buffer_in_flash, len) == false)
  {
    return decrypted_content_erased;
  }

  /* Check whether the buffer is encrypted */
  if (LL_MCE_IsCodePrimaryRegionEnabled() == false)
  {
    return decrypted_content_erased;
  }

  /* Check the content of the external flash without MCE */
  if (LL_MCE_DisableCodePrimaryRegion() != HAL_OK)
  {
    Error_Handler();
  }
  for (i = 0, u8b = buffer_in_flash; i < len; i++)
  {
    if (u8b[i] != erased_val)
    {
      crypted_content_erased = false;
    }
  }
  if (LL_MCE_EnableCodePrimaryRegion() != HAL_OK)
  {
    Error_Handler();
  }

  /* Return the result */
  if ((decrypted_content_erased == true) || (crypted_content_erased == true))
  {
    return true;
  }
  return false;
}

#if !defined(MCUBOOT_OVERWRITE_ONLY)
/**
  * Check the confirmation flag in RAM.
  *
  * @return 0 on success; nonzero on failure.
  */
int boot_check_conf_flag(void)
{
  int rc = 0U;
  uint32_t i;

  for (i = 0U; i < sizeof(boot_conf_flag_pattern); i++)
  {
    rc |= (boot_conf_flag_pattern[i] ^ boot_conf_flag[i]);
  }

  return rc;
}
#endif /* not MCUBOOT_OVERWRITE_ONLY */

#if defined(MCUBOOT_USE_HASH_REF)
/**
  * Compute SHA256 over the image to run.
  * @return 0 on success; nonzero on failure.
  */
static int boot_compute_run_img_hash(void)
{
  uint32_t size = 0;
  bootutil_sha256_context sha256_ctx = {0};
  struct image_header *hdr = NULL;
  uint8_t hash_ref[32] = {0};
  uint8_t hash_run[32] = {0};
  fih_int fih_rc = FIH_FAILURE;

  /* Get the reference hash */
  if (boot_hash_ref_get(hash_ref, sizeof(hash_ref), 0) != 0)
  {
    return BOOT_EBADSTATUS;
  }

  /* Get the header structure from the executable area */
  hdr = (struct image_header *)FLASH_APPLI_S_ADDRESS;

  /* Hash is computed over image header and image itself. */
  /* If protected TLVs are present they are also hashed. */
  size = hdr->ih_hdr_size + hdr->ih_img_size + hdr->ih_protect_tlv_size;

  /* Compute hash on the executable area */
  bootutil_sha256_init(&sha256_ctx);
  bootutil_sha256_update(&sha256_ctx,
                         (void *)FLASH_APPLI_S_ADDRESS,
                         size);
  bootutil_sha256_finish(&sha256_ctx, hash_run);

  FIH_CALL(boot_fih_memequal, fih_rc, hash_ref, hash_run, sizeof(hash_ref));
  if (fih_not_eq(fih_rc, FIH_SUCCESS))
  {
    return BOOT_EBADIMAGE;
  }

  return 0;
}
#endif /* MCUBOOT_USE_HASH_REF */

/* exported variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

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
static void SystemClock_Config(void)
{
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
  * @brief  Platform init
  * @param  None
  * @retval status
  */
int32_t boot_platform_init(void)
{
  /* Configures the System clock source, PLL Multiplier and Divider factors */
  SystemClock_Config();

  /* STM32N6xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

#ifdef MCUBOOT_HAVE_LOGGING
  /* Init for log */
  stdio_init();
#endif /*  MCUBOOT_HAVE_LOGGING */

#ifdef OEMUROT_CACHE_ENABLE
  /* Enable I-Cache */
  SCB_EnableICache();
#endif /* OEMUROT_CACHE_ENABLE */

  /* Start HW randomization */
  RNG_Init();
  (void)fih_delay_init();

  /* Disable and clean previous mpu config */
  LL_SECU_DisableCleanMpu();

  /* Get keys from OTP bits */
  OTP_InitKeys();

  /* Authorize NS debugging when requested */
  debug_authentication();

  if (EXT_FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    BOOT_LOG_ERR("Error while initializing External Flash Interface");
    Error_Handler();
  }

#if (OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  if (Ext_Ram_Initialize() != HAL_OK)
  {
    BOOT_LOG_ERR("Error while initializing External Ram Interface");
    Error_Handler();
  }
#endif /* OEMUROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  /* Apply Run time Protection */
  LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  LL_SECU_CheckStaticProtections();

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_1);
  uFlowStage = FLOW_STAGE_CHK;
  /* Double protections apply / check to resist to basic fault injections */
  /* Apply Run time Protection */
  (void)fih_delay();

  /* Apply Run time Protection */
  LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  LL_SECU_CheckStaticProtections();

#if defined(MCUBOOT_USE_HASH_REF)
  /* Load all images hash references (for mcuboot) */
  if (boot_hash_ref_load())
  {
    BOOT_LOG_ERR("Error while loading Hash references from FLash");
    Error_Handler();
  }
#endif /* MCUBOOT_USE_HASH_REF */

#if !defined(MCUBOOT_OVERWRITE_ONLY)
  if (boot_check_conf_flag())
  {
    BOOT_LOG_DBG("No confirmation required");
  }
  else
  {
    if (boot_set_confirmed() == 0U)
    {
      BOOT_LOG_DBG("Confirm Flag correctly set");
    }
    else
    {
      BOOT_LOG_ERR("Confirm Flag Not Correctly Written");
      Error_Handler();
    }
  }
  memset((void *)boot_conf_flag, CONF_FLAG_ERASE_VALUE, sizeof(boot_conf_flag));
#endif /* not MCUBOOT_OVERWRITE_ONLY */

  return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  *         This function does not return.
  * @param  None
  * @retval None
  */
#ifdef OEMUROT_ERROR_HANDLER_STOP_EXEC
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
#else
__attribute__((section(".BL2_Error_Code")))
#endif /* __ICCARM__ */
void Error_Handler(void)
{
  while(1);
}
#else /* OEMUROT_ERROR_HANDLER_STOP_EXEC */
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
__NO_RETURN void Error_Handler(void)
#else /* not __ICCARM__ */
__attribute__((section(".BL2_Error_Code")))
void Error_Handler(void)
#endif /* __ICCARM__ */
{
  /* It is customizeable */
  NVIC_SystemReset();
#if !defined(__ICCARM__)
  /* Avoid bx lr instruction (for fault injection) */
  __builtin_unreachable();
#endif /* defined(__ICCARM__) */
}
#endif /* OEMUROT_ERROR_HANDLER_STOP_EXEC */

#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef OEMUROT_DEV_MODE
  printf("assertion \" %s \" failed: file %s %d\n", expr, file, line);
#endif /*  OEMUROT_DEV_MODE  */
  Error_Handler();
}
#endif  /*  __ARMCC_VERSION */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  Error_Handler();
}
#endif /* USE_FULL_ASSERT */
