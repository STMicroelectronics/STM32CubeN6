/*
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __STM32N6xx_CFG_H__
#define __STM32N6xx_CFG_H__

#include "uart_stdout.h"

#define DRIVER_STDIO    Driver_USART0

/*
// <o>Floating Point Unit usage
//     <0=> Secure state only
//     <3=> Secure and Non-Secure state
//   <i> Value for SCB->NSACR register bits CP10, CP11
*/
#define SCB_NSACR_CP10_11_VAL       3

/*
// <o>Treat floating-point registers as Secure
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit TS
*/
#define FPU_FPCCR_TS_VAL            0

/*
// <o>Clear on return (CLRONRET) accessibility
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for FPU->FPCCR register bit CLRONRETS
*/
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
// <o>Clear floating-point caller saved registers on exception return
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit CLRONRET
*/
#define FPU_FPCCR_CLRONRET_VAL      1

#define SAU_REGION_NOT_NSC 0  /**< Region is not Non-Secure Callable */
#define SAU_REGION_NSC     1  /**< Region is Non-Secure Callable */

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

typedef enum
{
  OEMIROT_FALSE = 0U,
  OEMIROT_TRUE = !OEMIROT_FALSE
} OEMIROT_BoolTypeDef;

typedef enum
{
  OEMIROT_DISABLE = 0U,
  OEMIROT_ENABLE = !OEMIROT_DISABLE
} OEMIROT_FunctionalState;

struct sau_cfg_t {
    uint32_t RNR;
    uint32_t RBAR;
    uint32_t RLAR;
    uint8_t nsc;
#ifdef FLOW_CONTROL
    uint32_t flow_step_enable;
    uint32_t flow_ctrl_enable;
    uint32_t flow_step_check;
    uint32_t flow_ctrl_check;
#endif
};

#endif /* __STM32N6xx_CFG_H__ */