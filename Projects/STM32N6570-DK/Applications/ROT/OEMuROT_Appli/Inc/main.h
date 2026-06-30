/**
  ******************************************************************************
  * @file    AppliSecure/Inc/main.h
  * @author  GPM Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/******************************************************************************/
#ifdef __MAIN_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Exported variables ------------------------------------------------------- */

/******************************************************************************/
#endif /* MAIN_H */
