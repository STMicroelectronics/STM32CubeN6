/**
  ******************************************************************************
  * @file    system_stm32n6xx_fsbl.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M55 Device Peripheral Access Layer System Source File
  *          to be used after the boot ROM execution in an applicative code called
  *          "FSBL" for First Stage Boot Loader.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at secure startup just after the
  *                      boot ROM execution and before branch to secure main program.
  *                      This call is made inside the "startup_stm32n6xx_fsbl.s" file.
  *                      This function does not manage security isolation (IDAU/SAU,
  *                      interrupts, ...) unless USER_TZ_SAU_SETUP is defined at
  *                      project level.
  *
  *      - SystemCoreClock variable: Contains the CPU core clock, it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32n6xx_fsbl.s" file, to
  *   configure the system before to branch to main program.
  *
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

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32N6xx_System
  * @{
  */

/** @addtogroup STM32N6xx_System_Private_Includes
  * @{
  */

#include "stm32n6xx.h"
#if defined(USER_TZ_SAU_SETUP)
#include "partition_stm32n6xx.h"  /* Trustzone-M core secure attributes */
#endif /* USER_TZ_SAU_SETUP */
#include <math.h>

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_TypesDefinitions
  * @{
  */

#if defined ( __ICCARM__ )
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE)
#define HSE_VALUE      48000000UL /*!< Value of the High-Speed External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE      64000000UL /*!< Value of the High-Speed Internal oscillator in Hz */
#endif /* HSI_VALUE */

#if !defined  (MSI_VALUE)
  #define MSI_VALUE       4000000UL /*!< Minimum value of the Low-power Internal oscillator in Hz */
#endif /* MSI_VALUE */

#if !defined  (EXTERNAL_I2S_CLOCK_VALUE)
  #define EXTERNAL_I2S_CLOCK_VALUE  12288000UL /*!< Value of the External clock for I2S_CKIN in Hz */
#endif /* EXTERNAL_I2S_CLOCK_VALUE */


/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in memory, else the vector table is kept at the automatic
     selected boot address */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
#if !defined(VECT_TAB_BASE_ADDRESS)
#define VECT_TAB_BASE_ADDRESS   SRAM1_AXI_BASE_S /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x400. */
#endif

#if !defined(VECT_TAB_OFFSET)
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x400. */
#endif
#endif /* USER_VECT_TAB_ADDRESS */

/******************************************************************************/
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
uint32_t SystemCoreClock = HSI_VALUE;
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_FunctionPrototypes
  * @{
  */
static void SystemInit_ExtMemCtl(void);
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Functions
  * @{
  */

#if defined(__ICCARM__)
extern uint32_t __vector_table;
#define INTVECT_START ((uint32_t)&__vector_table)
#elif defined(__ARMCC_VERSION)
extern void *__Vectors;
#define INTVECT_START ((uint32_t)&__Vectors)
#elif defined(__GNUC__)
extern void *g_pfnVectors;
#define INTVECT_START ((uint32_t)&g_pfnVectors)
#endif

/**
  * @brief  Setup the microcontroller system.
  * @retval None
  */

void SystemInit(void)
{

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#else
  SCB->VTOR = INTVECT_START;
#endif  /* USER_VECT_TAB_ADDRESS */

  /* RNG reset */
  RCC->AHB3RSTSR = RCC_AHB3RSTSR_RNGRSTS;
  RCC->AHB3RSTCR = RCC_AHB3RSTCR_RNGRSTC;
  /* Deactivate RNG clock */
  RCC->AHB3ENCR = RCC_AHB3ENCR_RNGENC;

  /* Clear SAU regions */
  SAU->RNR = 0;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 1;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 2;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 3;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 4;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 5;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 6;
  SAU->RBAR = 0;
  SAU->RLAR = 0;
  SAU->RNR = 7;
  SAU->RBAR = 0;
  SAU->RLAR = 0;

  /* System configuration setup */
  RCC->APB4ENSR2 = RCC_APB4ENSR2_SYSCFGENS;
  /* Delay after an RCC peripheral clock enabling */
  (void)RCC->APB4ENR2;

  /* Set default Vector Table location after system reset or return from Standby */
  SYSCFG->INITSVTORCR = SCB->VTOR;

  /* Compensation cells setting according to Errata Sheet ES0620 */
  /* a/ Enable access and configuration of VDDIOxCCCR registers  */
  PWR->SVMCR1 |= PWR_SVMCR1_VDDIO4SV;
  PWR->SVMCR2 |= PWR_SVMCR2_VDDIO5SV;
  PWR->SVMCR3 |= PWR_SVMCR3_VDDIO2SV | PWR_SVMCR3_VDDIO3SV;

  /* b/ Enable access and configuration of VDDIOxCCCR registers */
  SYSCFG->VDDIO2CCCR = 0x00000287UL; /* VDDIO2 power domain compensation */
  SYSCFG->VDDIO3CCCR = 0x00000287UL; /* VDDIO3 power domain compensation */
  SYSCFG->VDDIO4CCCR = 0x00000287UL; /* VDDIO4 power domain compensation */
  SYSCFG->VDDIO5CCCR = 0x00000287UL; /* VDDIO4 power domain compensation */
  SYSCFG->VDDCCCR    = 0x00000287UL; /* VDD power domain compensation    */

  /* Enable VDDADC CLAMP */
  PWR->SVMCR3 |= PWR_SVMCR3_ASV;
  PWR->SVMCR3 |= PWR_SVMCR3_AVMEN;
  /* read back the register to make sure that the transaction has taken place */
  (void) PWR->SVMCR3;
  /* enable VREF */
  RCC->APB4ENR1 |= RCC_APB4ENR1_VREFBUFEN;

  /* RCC Fix to lower power consumption */
  RCC->APB4ENR2 |= 0x00000010UL;
  (void) RCC->APB4ENR2;
  RCC->APB4ENR2 &= ~(0x00000010UL);

  /* XSPI2 & XSPIM reset                                  */
  RCC->AHB5RSTSR = RCC_AHB5RSTSR_XSPIMRSTS | RCC_AHB5RSTSR_XSPI2RSTS;
  RCC->AHB5RSTCR = RCC_AHB5RSTCR_XSPIMRSTC | RCC_AHB5RSTCR_XSPI2RSTC;

  /* TIM2 reset */
  RCC->APB1RSTSR1 = RCC_APB1RSTSR1_TIM2RSTS;
  RCC->APB1RSTCR1 = RCC_APB1RSTCR1_TIM2RSTC;
  /* Deactivate TIM2 clock */
  RCC->APB1ENCR1 = RCC_APB1ENCR1_TIM2ENC;

  /* Deactivate GPIOG clock */
  RCC->AHB4ENCR = RCC_AHB4ENCR_GPIOGENC;

  /* Read back the value to make sure it is written before deactivating SYSCFG */
  (void) SYSCFG->INITSVTORCR;
  /* Deactivate SYSCFG clock */
  RCC->APB4ENCR2 = RCC_APB4ENCR2_SYSCFGENC;

#if defined(USER_TZ_SAU_SETUP)
  /* SAU/IDAU, FPU and Interrupts secure/non-secure allocation settings */
  TZ_SAU_Setup();
#endif /* USER_TZ_SAU_SETUP */

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */

  SCB_NS->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif /* __FPU_PRESENT && __FPU_USED */

  /* Configure FMC interface and its GPIO for SDRAM management */
  /* Send Command to SDRAM to enable it as RAM memory access */
  SystemInit_ExtMemCtl();

}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the CPU core clock changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If CPUCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *
  *           - If CPUCLK source is MSI, SystemCoreClock will contain the MSI_VALUE(**)
  *
  *           - If CPUCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If CPUCLK source is IC1, SystemCoreClock will contain the HSI_VALUE(*)
  *             or MSI_VALUE(**) or HSE_VALUE(***) or EXTERNAL_I2S_CLOCK_VALUE (****)
  *             multiplied/divided by the PLL factors.
  *
  *         (*) HSI_VALUE default value is 64 MHz.
  *             With the HAL, HSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file
  *             but the real value may vary depending on the variations in voltage and temperature.
  *
  *        (**) MSI_VALUE default value is 4 MHz.
  *             With the HAL, MSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file
  *             but the real value may vary depending on the variations in voltage and temperature.
  *
  *       (***) HSE_VALUE default value is 30 MHz.
  *             With the HAL, HSE_VALUE is a constant defined in stm32n6xx_hal_conf.h file.
  *             User has to ensure that HSE_VALUE is same as the real frequency of the crystal used.
  *             Otherwise, this function may have wrong result.
  *
  *      (****) EXTERNAL_I2S_CLOCK_VALUE default value is 12.288 MHz.
  *             With the HAL, EXTERNAL_I2S_CLOCK_VALUE is a constant defined in stm32n6xx_hal_conf.h file.
  *             User has to ensure that EXTERNAL_I2S_CLOCK_VALUE is same as the real I2S_CKIN
  *             pin frequency. Otherwise, this function may have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t sysclk = 0;
  uint32_t pllm = 0;
  uint32_t plln = 0;
  uint32_t pllfracn = 0;
  uint32_t pllp1 = 0;
  uint32_t pllp2 = 0;
  uint32_t pllcfgr, pllsource, pllbypass, ic_divider;
  float_t pllvco;

  /* Get CPUCLK source -------------------------------------------------------*/
  switch (RCC->CFGR1 & RCC_CFGR1_CPUSWS)
  {
  case 0:  /* HSI used as system clock source (default after reset) */
    sysclk = HSI_VALUE >> ((RCC->HSICFGR & RCC_HSICFGR_HSIDIV) >> RCC_HSICFGR_HSIDIV_Pos);
    break;

  case RCC_CFGR1_CPUSWS_0:  /* MSI used as system clock source */
    if (READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL) == 0UL)
    {
      sysclk = MSI_VALUE;
    }
    else
    {
      sysclk = 16000000UL;
    }
    break;

  case RCC_CFGR1_CPUSWS_1:  /* HSE used as system clock source */
    sysclk = HSE_VALUE;
    break;

  case (RCC_CFGR1_CPUSWS_1 | RCC_CFGR1_CPUSWS_0):  /* IC1 used as system clock  source */
    /* Get IC1 clock source parameters */
    switch (READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1SEL))
    {
    case 0:  /* PLL1 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL1CFGR1);
      pllsource = pllcfgr & RCC_PLL1CFGR1_PLL1SEL;
      pllbypass = pllcfgr & RCC_PLL1CFGR1_PLL1BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL1CFGR1_PLL1DIVM) >>  RCC_PLL1CFGR1_PLL1DIVM_Pos;
        plln = (pllcfgr & RCC_PLL1CFGR1_PLL1DIVN) >>  RCC_PLL1CFGR1_PLL1DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC) >>  RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL1CFGR3);
        pllp1 = (pllcfgr & RCC_PLL1CFGR3_PLL1PDIV1) >>  RCC_PLL1CFGR3_PLL1PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL1CFGR3_PLL1PDIV2) >>  RCC_PLL1CFGR3_PLL1PDIV2_Pos;
      }
      break;
    case RCC_IC1CFGR_IC1SEL_0:  /* PLL2 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL2CFGR1);
      pllsource = pllcfgr & RCC_PLL2CFGR1_PLL2SEL;
      pllbypass = pllcfgr & RCC_PLL2CFGR1_PLL2BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL2CFGR1_PLL2DIVM) >>  RCC_PLL2CFGR1_PLL2DIVM_Pos;
        plln = (pllcfgr & RCC_PLL2CFGR1_PLL2DIVN) >>  RCC_PLL2CFGR1_PLL2DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC) >>  RCC_PLL2CFGR2_PLL2DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL2CFGR3);
        pllp1 = (pllcfgr & RCC_PLL2CFGR3_PLL2PDIV1) >>  RCC_PLL2CFGR3_PLL2PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL2CFGR3_PLL2PDIV2) >>  RCC_PLL2CFGR3_PLL2PDIV2_Pos;
      }
      break;

    case RCC_IC1CFGR_IC1SEL_1:  /* PLL3 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL3CFGR1);
      pllsource = pllcfgr & RCC_PLL3CFGR1_PLL3SEL;
      pllbypass = pllcfgr & RCC_PLL3CFGR1_PLL3BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL3CFGR1_PLL3DIVM) >>  RCC_PLL3CFGR1_PLL3DIVM_Pos;
        plln = (pllcfgr & RCC_PLL3CFGR1_PLL3DIVN) >>  RCC_PLL3CFGR1_PLL3DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC) >>  RCC_PLL3CFGR2_PLL3DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL3CFGR3);
        pllp1 = (pllcfgr & RCC_PLL3CFGR3_PLL3PDIV1) >>  RCC_PLL3CFGR3_PLL3PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL3CFGR3_PLL3PDIV2) >>  RCC_PLL3CFGR3_PLL3PDIV2_Pos;
      }
      break;

    default: /* RCC_IC1CFGR_IC1SEL_1 | RCC_IC1CFGR_IC1SEL_0 */  /* PLL4 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL4CFGR1);
      pllsource = pllcfgr & RCC_PLL4CFGR1_PLL4SEL;
      pllbypass = pllcfgr & RCC_PLL4CFGR1_PLL4BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL4CFGR1_PLL4DIVM) >>  RCC_PLL4CFGR1_PLL4DIVM_Pos;
        plln = (pllcfgr & RCC_PLL4CFGR1_PLL4DIVN) >>  RCC_PLL4CFGR1_PLL4DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC) >>  RCC_PLL4CFGR2_PLL4DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL4CFGR3);
        pllp1 = (pllcfgr & RCC_PLL4CFGR3_PLL4PDIV1) >>  RCC_PLL4CFGR3_PLL4PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL4CFGR3_PLL4PDIV2) >>  RCC_PLL4CFGR3_PLL4PDIV2_Pos;
      }
      break;
    }

    /* Get oscillator frequency used as PLL clock source */
    switch (pllsource)
    {
    case 0:  /* HSI selected as PLL clock source */
      sysclk = HSI_VALUE >> ((RCC->HSICFGR & RCC_HSICFGR_HSIDIV) >> RCC_HSICFGR_HSIDIV_Pos);
      break;
    case RCC_PLL1CFGR1_PLL1SEL_0: /* MSI selected as PLL clock source */
      if (READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL) == 0UL)
      {
        sysclk = MSI_VALUE;
      }
      else
      {
        sysclk = 16000000UL;
      }
      break;
    case RCC_PLL1CFGR1_PLL1SEL_1: /* HSE selected as PLL clock source */
      sysclk = HSE_VALUE;
      break;
    case (RCC_PLL1CFGR1_PLL1SEL_1 | RCC_PLL1CFGR1_PLL1SEL_0):  /* I2S_CKIN selected as PLL clock source */
      sysclk = EXTERNAL_I2S_CLOCK_VALUE;
      break;
    default:
      /* Nothing to do, should not occur */
      break;
    }

    /* Check whether PLL is in bypass mode or not */
    if (pllbypass == 0U)
    {
      /* Compte PLL output frequency (Integer and fractional modes) */
      /* PLLVCO = (Freq * (DIVN + (FRACN / 0x1000000) / DIVM) / (DIVP1 * DIVP2)) */
      pllvco = ((float_t)sysclk * ((float_t)plln + ((float_t)pllfracn/(float_t)0x1000000UL))) / (float_t)pllm;
      sysclk = (uint32_t)((float_t)(pllvco/(((float_t)pllp1) * ((float_t)pllp2))));
    }
    /* Apply IC1 divider */
    ic_divider = (READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1INT) >> RCC_IC1CFGR_IC1INT_Pos) + 1UL;
    sysclk = sysclk / ic_divider;
    break;
  default:
    /* Nothing to do, should not occur */
    break;
  }

  /* Return system clock frequency (CPU frequency) */
  SystemCoreClock = sysclk;
}

/**
  * @brief  Secure Non-Secure-Callable function to return the current
  *         SystemCoreClock value after SystemCoreClock update.
  *         The SystemCoreClock variable contains the CPU core clock, it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  * @retval SystemCoreClock value
  */
CMSE_NS_ENTRY uint32_t SECURE_SystemCoreClockUpdate(void)
{
  SystemCoreClockUpdate();

  return SystemCoreClock;
}

/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32n6xx.s before jump to main.
  *         This function configures the external memories (SRAM/SDRAM)
  *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
static void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0;

  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

  /* Enable PWR interface clock */
  WRITE_REG(RCC->AHB4ENSR, RCC_AHB4ENR_PWREN);

  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB4ENR);

  /* Enable PWR VDDIO2, VDDIO3, VDDIO4, VDDIO5 */
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV);
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV);
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV);
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV);

  /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOO and GPIOP interface clock */
  WRITE_REG(RCC->AHB4ENSR, (RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN | RCC_AHB4ENR_GPIODEN | \
                            RCC_AHB4ENR_GPIOEEN | RCC_AHB4ENR_GPIOOEN | RCC_AHB4ENR_GPIOPEN));

  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB4ENR);

/*-- GPIO Configuration ------------------------------------------------------*/
/*-- GPIO Configuration ------------------------------------------------------*/
  /* FMC GPIO Configuration
    PA0   ------> FMC_D7
    PA1   ------> FMC_D6
    PA2   ------> FMC_D5
    PA4   ------> FMC_A13
    PA8   ------> FMC_D4
    PA9   ------> FMC_D3
    PA10  ------> FMC_D2
    PA11  ------> FMC_D1
    PA12  ------> FMC_D0

    PB3   ------> FMC_NBL1
    PB4   ------> FMC_D13
    PB5   ------> FMC_D12
    PB6   ------> FMC_D14
    PB7   ------> FMC_D15
    PB10  ------> FMC_D11
    PB11  ------> FMC_D10
    PB12  ------> FMC_D9

    PC3   ------> FMC_D8
    PC10  ------> FMC_CLK

    PD0   ------> FMC_A6
    PD1   ------> FMC_A7
    PD2   ------> FMC_A0
    PD3   ------> FMC_A10
    PD4   ------> FMC_A11
    PD6   ------> FMC_A1
    PD7   ------> FMC_A2
    PD8   ------> FMC_NBL0
    PD9   ------> FMC_SDCLK
    PD10  ------> FMC_A3
    PD12  ------> FMC_A5
    PD14  ------> FMC_A9
    PD15  ------> FMC_A8

    PE5   ------> FMC_SDNE1
    PE6   ------> FMC_SDKE1
    PE7   ------> FMC_A4
    PE8   ------> FMC_A12
    PE9   ------> FMC_A14 (Bank address 0)
    PE10  ------> FMC_A15 (Bank address 1)
    PE11  ------> FMC_SDNWE
    PE12  ------> FMC_NRAS
    PE13  ------> FMC_NCAS
    PE14  ------> FMC_SDNE0
    PE15  ------> FMC_SDKE0

    PO4   ------> FMC_NBL2
    PO5   ------> FMC_NBL3

    PP0   ------> FMC_D16
    PP1   ------> FMC_D17
    PP2   ------> FMC_D18
    PP3   ------> FMC_D19
    PP4   ------> FMC_D20
    PP5   ------> FMC_D21
    PP6   ------> FMC_D22
    PP7   ------> FMC_D23
    PP8   ------> FMC_D24
    PP9   ------> FMC_D25
    PP10  ------> FMC_D26
    PP11  ------> FMC_D27
    PP12  ------> FMC_D28
    PP13  ------> FMC_D29
    PP14  ------> FMC_D30
    PP15  ------> FMC_D31
  */

  /* Configure PAx pins in Alternate function mode */
  GPIOA->MODER   = 0xAAAAEEEA;
  /* Configure PAx pins Output type to push-pull */
  GPIOA->OTYPER  = 0x00000000;
  /* Configure PAx pins very high speed */
  GPIOA->OSPEEDR = 0x0FFF033F;
  /* Configure PAx pins in Pull-up */
  GPIOA->PUPDR   = 0x65550115;
  /* Connect PAx pins to FMC Alternate function */
  GPIOA->AFR[0]  = 0x000C0CCC;
  GPIOA->AFR[1]  = 0x000CCCCC;

  /* Configure PBx pins in Alternate function mode */
  GPIOB->MODER   = 0xFEAFAABF;
  /* Configure PBx pins Output type to push-pull */
  GPIOB->OTYPER  = 0x00000000;
  /* Configure PBx pins very high speed */
  GPIOB->OSPEEDR = 0x03F0FFC0;
  /* Configure PBx pins in Pull-up */
  GPIOB->PUPDR   = 0x01505540;
  /* Connect PBx pins to FMC Alternate function */
  GPIOB->AFR[0]  = 0xCCCCC000;
  GPIOB->AFR[1]  = 0x000CCC00;

  /* Configure PCx pins in Alternate function mode */
  GPIOC->MODER   = 0xFFEFFFBF;
  /* Configure PCx pins Output type to push-pull */
  GPIOC->OTYPER  = 0x00000000;
  /* Configure PCx pins very high speed */
  GPIOC->OSPEEDR = 0x003000C0;
  /* Configure PCx pins in Pull-up */
  GPIOC->PUPDR   = 0x00100040;
  /* Connect PCx pins to FMC Alternate function */
  GPIOC->AFR[0]  = 0x0000C000;
  GPIOC->AFR[1]  = 0x00000C00;

  /* Configure PDx pins in Alternate function mode */
  GPIOD->MODER   = 0xAEEAAEAA;
  /* Configure PDx pins Output type to push-pull */
  GPIOD->OTYPER  = 0x00000000;
  /* Configure PDx pins very high speed */
  GPIOD->OSPEEDR = 0xF33FF3FF;
  /* Configure PDx pins in Pull-up */
  GPIOD->PUPDR   = 0x51155155;
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0xCC0CCCCC;
  GPIOD->AFR[1]  = 0xCC0C0CCC;

  /* Configure PEx pins in Alternate function mode */
  GPIOE->MODER   = 0xAAAAABFF;
  /* Configure PEx pins Output type to push-pull */
  GPIOE->OTYPER  = 0x00000000;
  /* Configure PEx pins very high speed */
  GPIOE->OSPEEDR = 0xFFFFFC00;
  /* Configure PEx pins in Pull-up */
  GPIOE->PUPDR   = 0x55555400;
  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xCCC00000;
  GPIOE->AFR[1]  = 0xCCCCCCCC;

  /* Configure POx pins in Alternate function mode */
  GPIOO->MODER   = 0x00000AFF;
  /* Configure POx pins Output type to push-pull */
  GPIOO->OTYPER  = 0x00000000;
  /* Configure POx pins very high speed */
  GPIOO->OSPEEDR = 0x00000F00;
  /* Configure POx pins in Pull-up */
  GPIOO->PUPDR   = 0x00000500;
  /* Connect POx pins to FMC Alternate function */
  GPIOO->AFR[0]  = 0x00EE0000;
  GPIOO->AFR[1]  = 0x00000000;

  /* Configure PPx pins in Alternate function mode */
  GPIOP->MODER   = 0xAAAAAAAA;
  /* Configure PPx pins Output type to push-pull */
  GPIOP->OTYPER  = 0x00000000;
  /* Configure PPx pins very high speed */
  GPIOP->OSPEEDR = 0xFFFFFFFF;
  /* Configure PPx pins in Pull-up */
  GPIOP->PUPDR   = 0x55555555;
  /* Connect PPx pins to FMC Alternate function */
  GPIOP->AFR[0]  = 0xCCCCCCCC;
  GPIOP->AFR[1]  = 0xCCCCCCCC;

/*-- FMC Configuration ------------------------------------------------------*/
  /* Enable the FMC interface clock */
  WRITE_REG(RCC->AHB5ENSR, RCC_AHB5ENR_FMCEN);

  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB5ENR);

  /*SDRAM Timing and access interface configuration*/
  /* Init->ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9
     Init->RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13
     Init->MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_8
     Init->InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4
     Init->CASLatency         = FMC_SDRAM_CAS_LATENCY_3
     Init->WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE
     Init->SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_3
     Init->ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0 */

  FMC_Bank5_6_R->SDCR[0] = 0x00000DC9;

  /* Timing->LoadToActiveDelay     = 2
     Timing->ExitSelfRefreshDelay  = 7
     Timing->SelfRefreshTime       = 4
     Timing->RowCycleDelay         = 7
     Timing->WriteRecoveryTime     = 2
     Timing->RPDelay               = 2
     Timing->RCDDelay              = 2 */

  FMC_Bank5_6_R->SDTR = 0x01116361;

  /* FMC Enable */
  FMC_Common_R->CFGR |= FMC_CFGR_FMCEN;

  /* SDRAM initialization sequence */
  FMC_Bank5_6_R->SDCR[0] |=  FMC_SDCRx_SDEN;

  /* Clock configuration enable command */
  FMC_Bank5_6_R->SDCMR   =  (0x1 | FMC_SDCMR_DS1); /* FMC_SDRAM_CMD_CLK_ENABLE | FMC_SDRAM_CMD_TARGET_BANK1 */
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  }

  /* Delay */
  for (index = 0; index < 1000; index++)
  {
    __NOP();
  }

  /* PALL (precharge all) command */
  FMC_Bank5_6_R->SDCMR   =  (0x2 | FMC_SDCMR_DS1); /* FMC_SDRAM_CMD_PALL | FMC_SDRAM_CMD_TARGET_BANK1 */
  timeout = 0xFFFF;
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  }

  /* Configure a Auto-Refresh command */
  FMC_Bank5_6_R->SDCMR   =  (0x3 | FMC_SDCMR_DS1 | (7 << FMC_SDCMR_NRFS_Pos)); /* FMC_SDRAM_CMD_AUTOREFRESH_MODE | FMC_SDRAM_CMD_TARGET_BANK1 | AutoRefreshNumber = 8 */
  timeout = 0xFFFF;
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  }

  /* Configure Load mode */
  FMC_Bank5_6_R->SDCMR   =  (0x4 | FMC_SDCMR_DS1); /* FMC_SDRAM_CMD_LOAD_MODE | FMC_SDRAM_CMD_TARGET_BANK1 */
  timeout = 0xFFFF;
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
  }

  /* Configure FMC refresh timer register */
  FMC_Bank5_6_R->SDRTR |= ((uint32_t)((105)<< 1));

  /* SDRAM initialization done */
  FMC_Bank5_6_R->SDCR[0] |= FMC_SDCRx_SDINIT;

  (void)(tmp);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
