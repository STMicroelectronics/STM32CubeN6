/**
 *******************************************************************************
 * @file    Appli/Core/Inc/st_monitor.h
 * @author  MCD Application Team
 * @brief   CPU load / OS monitor interface
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may obtain a copy of the License in the LICENSE file
 * in the root directory of this software component.
 *
 *******************************************************************************
 */

#ifndef __ST_MONITOR_H__
#define __ST_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* To ensure correct cycles measurement, the calculation period (ms) must be
   less than 2^32 * 1000 / osCpu_Freq. For example, with osCpu_Freq = 520 MHz,
   ST_OS_MONITOR_CALCULATION_PERIOD_MS < 8259 ms. */
#ifndef ST_OS_MONITOR_CALCULATION_PERIOD_MS
#define ST_OS_MONITOR_CALCULATION_PERIOD_MS 500UL
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct st_os_monitor_cpu_info_t
{
  float osCPU_UsageRatio; /* Current CPU usage ratio [0..1] */
  float osCPU_PeakRatio;  /* Peak CPU usage ratio observed [0..1] */
  float osCpu_Freq;       /* CPU frequency in MHz */
} st_os_monitor_cpu_info_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Clear internal CPU monitoring statistics (reset peaks/counters).
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_clear(void);

/**
 * @brief  Query CPU usage in percent (current and peak).
 * @param  pCpuCurrent Pointer to float to receive current usage percentage.
 * @param  pCpuPeak    Pointer to float to receive peak usage percentage.
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_percent(float *pCpuCurrent, float *pCpuPeak);

/**
 * @brief  Query CPU frequency in MHz (current and peak estimations).
 * @param  pCpuCurrent Pointer to float to receive current CPU frequency (MHz).
 * @param  pCpuPeak    Pointer to float to receive peak CPU frequency (MHz).
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_Mhz(float *pCpuCurrent, float *pCpuPeak);

/**
 * @brief  Retrieve a full CPU info structure (usage ratios and frequency).
 * @param  pInfo Pointer to st_os_monitor_cpu_info_t to be filled by the function.
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_info(st_os_monitor_cpu_info_t *pInfo);

/**
 * @brief  Initialize CPU monitor with the CPU frequency in Hz.
 * @param  freqCpu CPU frequency in Hz.
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_init(uint32_t freqCpu);

/**
 * @brief  Terminate CPU monitor and release resources.
 * @retval int8_t 0 on success, negative on error
 */
int8_t st_os_monitor_cpu_term(void);

/**
 * @brief  Called from idle loop to accumulate idle cycles for usage calculation.
 * @note   This function should be lightweight and called often from the idle task.
 */
void st_os_monitor_cpu_idle(void);

/**
 * @brief  Network monitoring helper (legacy name; kept for compatibility).
 * @note   Implementation may be shared with bitrate/network monitors.
 */
void monitor_network(void);

#ifdef __cplusplus
}
#endif

#endif /* __ST_MONITOR_H__ */


