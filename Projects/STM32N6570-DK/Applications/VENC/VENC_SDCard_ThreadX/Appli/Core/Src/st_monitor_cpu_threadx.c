/**
  ******************************************************************************
  * @file           : st_monitor_cpu_threadx.c
  * @brief          : CPU load monitor implementation (ThreadX)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "st_monitor.h"
#include "tx_api.h"
#include "tx_execution_profile.h"
#include "stm32n6xx_hal.h"

/* Global variables ----------------------------------------------------------*/
extern TX_BYTE_POOL tx_app_byte_pool;

/* Private typedef / macros --------------------------------------------------*/
/* Use DWT->CYCCNT for cycle counter reads (Cortex-M DWT) */

/* Private globals -----------------------------------------------------------*/
static st_os_monitor_cpu_info_t cpuInfo;
static uint32_t                 osCPU_StartTime = 0U;

/* Private function prototypes -----------------------------------------------*/
static void     s_execution_idle_time_reset(void);
static uint32_t s_execution_idle_time_get(void);

/* Weak fallback for Error_Handler to avoid link issues in unit contexts */
__weak void Error_Handler(void)
{
    printf("Error_Handler called at %s:%d\n", __FILE__, __LINE__);
    while (1)
        ;
}

/**
 * @brief  Initialize CPU monitor.
 * @param  freqCpu  CPU frequency in Hz (used for any frequency-related reporting).
 * @retval int8_t   Returns 1 to indicate an idle task is expected/required.
 *
 * @note   This function enables the DWT cycle counter if not already enabled.
 */
int8_t st_os_monitor_cpu_init(uint32_t freqCpu)
{
    cpuInfo.osCPU_UsageRatio = 0.0f;
    cpuInfo.osCPU_PeakRatio  = 0.0f;
    cpuInfo.osCpu_Freq       = (float)freqCpu;

    /* Enable DWT cycle counter if not already enabled */
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0U;                 /* reset the counter */
        DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk; /* enable the counter */
    }

    return 1;
}

/**
 * @brief  Called from the idle loop to measure CPU usage over the period.
 *
 * This routine reads the cycle counter and uses ThreadX execution-time APIs
 * to compute the portion of time spent executing (non-idle) between samples.
 * It sleeps for ST_OS_MONITOR_CALCULATION_PERIOD_MS between measurements.
 */
void st_os_monitor_cpu_idle(void)
{
    static bool firstTime = true;
    uint32_t currentTime = DWT->CYCCNT;

    if (firstTime)
    {
        s_execution_idle_time_reset();
        osCPU_StartTime = currentTime;
        firstTime = false;
    }
    else
    {
        uint32_t elapsedTime = currentTime - osCPU_StartTime;
        uint32_t totalCpuSpentTime = elapsedTime - s_execution_idle_time_get();

        s_execution_idle_time_reset();

        /* compute usage ratio in [0..1] */
        cpuInfo.osCPU_UsageRatio = (float)totalCpuSpentTime / (float)elapsedTime;
        if (cpuInfo.osCPU_UsageRatio >= cpuInfo.osCPU_PeakRatio)
        {
            cpuInfo.osCPU_PeakRatio = cpuInfo.osCPU_UsageRatio;
        }

        osCPU_StartTime = currentTime;
    }

    /* Sleep for the configured monitoring period (in ms). */
    tx_thread_sleep(ST_OS_MONITOR_CALCULATION_PERIOD_MS);
}

/**
 * @brief  Terminate CPU monitor.
 * @retval int8_t  1 on success.
 */
int8_t st_os_monitor_cpu_term(void)
{
    /* No resources to free in current implementation */
    return 1;
}

/**
 * @brief  Retrieve CPU monitor information.
 * @param  pInfo  Pointer to structure that will receive CPU info.
 * @retval int8_t 1 on success, 0 on invalid parameter.
 */
int8_t st_os_monitor_cpu_info(st_os_monitor_cpu_info_t *pInfo)
{
    int8_t ok = 0;
    if (pInfo != NULL)
    {
        ok = 1;
        *pInfo = cpuInfo;
    }
    return ok;
}

/**
 * @brief  Clear the peak CPU usage reference.
 * @retval int8_t 1 on success.
 */
int8_t st_os_monitor_cpu_clear(void)
{
    cpuInfo.osCPU_PeakRatio = 0.0f;
    return 1;
}

/**
 * @brief  Monitor thread function that periodically reports CPU usage and MHz.
 * @param  arg Thread entry argument (unused).
 */
void monitor_thread_func(ULONG arg)
{
    (void)arg;
    /* Initialize monitor with actual CPU frequency (Hz) */
    st_os_monitor_cpu_init(HAL_RCC_GetCpuClockFreq());

    float cpuCurrent = 0.0f, cpuPeak = 0.0f;
    float mhzCurrent = 0.0f, mhzPeak = 0.0f;

    while (1)
    {
        st_os_monitor_cpu_idle();

        /* Query usage in percent and frequency in MHz */
        (void)st_os_monitor_cpu_percent(&cpuCurrent, &cpuPeak);
        (void)st_os_monitor_cpu_Mhz(&mhzCurrent, &mhzPeak);

        /* Print with float formatting (embedded printf float support may vary) */
        printf("CPU Usage: %.1f %%  (%.0f MHz), Peak: %.1f %% (%.0f MHz)\n",
               cpuCurrent, mhzCurrent, cpuPeak, mhzPeak);

        monitor_network();
    }
}

#define LOWEST_PRIORITY   (TX_MAX_PRIORITIES - 1U)

/**
 * @brief  Create and start the monitor thread.
 */
void monitor_thread_create(void)
{
    static TX_THREAD monitor_thread;
    UINT status = TX_SUCCESS;
    void *thread_stack_pointer = NULL;

    if (tx_byte_allocate(&tx_app_byte_pool, &thread_stack_pointer, 1024U, TX_NO_WAIT) != TX_SUCCESS)
    {
        Error_Handler();
    }

    status = tx_thread_create(&monitor_thread, "Monitor App Thread", monitor_thread_func, 0,
                              thread_stack_pointer, 1024U, LOWEST_PRIORITY, LOWEST_PRIORITY,
                              TX_NO_TIME_SLICE, TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        Error_Handler();
    }
}

/* Private functions -------------------------------------------------------- */

/**
 * @brief  Get the accumulated idle execution time from ThreadX execution profile.
 * @retval uint32_t idle time in DWT cycles (casted from EXECUTION_TIME).
 */
static uint32_t s_execution_idle_time_get(void)
{
    EXECUTION_TIME idleTime;
    _tx_execution_idle_time_get(&idleTime);
    return (uint32_t)idleTime;
}

/**
 * @brief  Reset ThreadX execution-time idle counters.
 */
static void s_execution_idle_time_reset(void)
{
    _tx_execution_idle_time_reset();
}

