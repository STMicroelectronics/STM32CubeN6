 /**
 ******************************************************************************
 * @file    utils.h
 * @author  GPM Application Team
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

#ifndef UTILS
#define UTILS

#define ALIGN_32 __attribute__((aligned (32)))
#if defined(__CC_ARM)
#define IN_PSRAM __attribute__((section(".bss.NoInit"))) __attribute__((zero_init))
#elif defined(__ARMCC_VERSION)
#define IN_PSRAM __attribute__((section(".bss.NoInit")))
#else
#define IN_PSRAM __attribute__((section (".psram_bss")))
#endif
#define IN_RAM   __attribute__((section(".bss")))
#define IN_UNCACHED_RAM  __attribute__((section(".noncacheable")))


#endif
