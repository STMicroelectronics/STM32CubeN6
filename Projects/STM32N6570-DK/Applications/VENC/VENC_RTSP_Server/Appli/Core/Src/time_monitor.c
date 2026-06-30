/**
 *******************************************************************************
 * @file    time_monitor.c
 * @brief   Time monitor implementation (DWT cycle counter based)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *******************************************************************************
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32n6xx_hal.h"
#include "time_monitor.h"

/* Period for printing stats (in seconds) */
#define PRINT_STATS_PERIOD_SECONDS  5U

/* Structure to hold the statistics */
typedef struct {
    uint64_t maxDeltaTime;
    uint64_t minDeltaTime;
    uint64_t totalDeltaTime;
    uint64_t numCalls;
    uint64_t startTime;
    uint64_t lastCallTime;
} DeltaTimeStats;

/* Initialize minDeltaTime to UINT64_MAX so first measured delta becomes the min */
static DeltaTimeStats stats = {0ULL, UINT64_MAX, 0ULL, 0ULL, 0ULL, 0ULL};

/* Variables to track 32-bit DWT wraparound */
static volatile uint32_t overflowCount = 0U;
static volatile uint32_t lastCycleCount = 0U;
static uint32_t initDone = 0U;
static uint32_t system_clock_frequency = 0U;

static unsigned int display_u64(uint64_t value)
{
    if (value > (uint64_t)UINT32_MAX)
    {
        return UINT32_MAX;
    }

    return (unsigned int)value;
}

/**
 * @brief  Initialize the DWT cycle counter and capture system clock frequency.
 *
 * Safe to call multiple times; actual initialization is performed once.
 */
void initCycleCounter(void)
{
    if (initDone == 0U)
    {
        /* Capture CPU clock frequency (Hz) */
        system_clock_frequency = HAL_RCC_GetCpuClockFreq();
        printf("Monitoring: CPU: %u MHz\n", (unsigned int)(system_clock_frequency / 1000000UL));

        /* Enable trace and DWT if not already enabled */
        if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
        {
            CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        }

        /* Reset and enable cycle counter */
        DWT->CYCCNT = 0U;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

        lastCycleCount = DWT->CYCCNT;
        initDone = 1U;
    }
}

/**
 * @brief  Detect and account for 32-bit DWT cycle counter overflow.
 *
 * Must be called before reading the extended cycle count composed with overflowCount.
 */
void handleCycleCounterOverflow(void)
{
    uint32_t currentCycleCount = DWT->CYCCNT;
    if (currentCycleCount < lastCycleCount)
    {
        overflowCount++;
    }
    lastCycleCount = currentCycleCount;
}

/**
 * @brief  Get current time in milliseconds based on the DWT cycle counter.
 * @retval Current time in milliseconds. Returns 0 if system clock frequency is unknown.
 */
uint64_t getCurrentTimeMillis(void)
{
    initCycleCounter();
    handleCycleCounterOverflow();

    if (system_clock_frequency == 0U)
    {
        /* Avoid division by zero if frequency unknown */
        return 0ULL;
    }

    uint64_t totalCycles = (((uint64_t)overflowCount) << 32) | (uint64_t)DWT->CYCCNT;

    /* Compute milliseconds using 64-bit arithmetic to preserve precision:
       ms = totalCycles * 1000 / system_clock_frequency */
    uint64_t ms = (totalCycles * 1000ULL) / (uint64_t)system_clock_frequency;
    return ms;
}

/**
 * @brief  Print collected timing statistics.
 *
 * Guards against zero divisions and handles empty statistics gracefully.
 */
void printStats(void)
{
    uint64_t elapsed_ms = 0ULL;
    uint64_t avg_ms = 0ULL;
    unsigned int max_delta_ms;
    unsigned int min_delta_ms;
    unsigned int avg_delta_ms;
    unsigned int elapsed_ms_u32;
    unsigned int num_calls;

    if (stats.startTime != 0ULL)
    {
        elapsed_ms = getCurrentTimeMillis() - stats.startTime;
    }

    if (stats.numCalls != 0ULL)
    {
        avg_ms = stats.totalDeltaTime / stats.numCalls;
    }

    max_delta_ms = display_u64(stats.maxDeltaTime);
    min_delta_ms = display_u64((stats.minDeltaTime == UINT64_MAX) ? 0ULL : stats.minDeltaTime);
    avg_delta_ms = display_u64(avg_ms);
    elapsed_ms_u32 = display_u64(elapsed_ms);
    num_calls = display_u64(stats.numCalls);

    printf("======================================\n");
    printf("Max Delta Time          : %10u ms\n", max_delta_ms);
    printf("Min Delta Time          : %10u ms\n", min_delta_ms);
    printf("Average Delta Time      : %10u ms\n", avg_delta_ms);
    printf("Total Time              : %10u ms\n", elapsed_ms_u32);
    printf("Number of Calls         : %10u   \n", num_calls);

    if (elapsed_ms > 0ULL)
    {
        uint64_t calls_per_sec = (1000ULL * stats.numCalls) / elapsed_ms;
        printf("Number of Calls/Seconds : %10u   \n", display_u64(calls_per_sec));
    }
    else
    {
        printf("Number of Calls/Seconds : %10s   \n", "N/A");
    }
}

/**
 * @brief  Periodic function to update timing statistics.
 *
 * This function should be called periodically by the monitored task to record
 * time deltas between successive invocations.
 */
void timeMonitor(void)
{
    uint64_t currentTime = getCurrentTimeMillis();

    /* If first call, initialize baselines */
    if (stats.lastCallTime == 0ULL)
    {
        stats.lastCallTime = currentTime;
        stats.startTime = currentTime;
        return;
    }

    uint64_t deltaTime = currentTime - stats.lastCallTime;

    /* Update statistics */
    if (deltaTime > stats.maxDeltaTime)
    {
        stats.maxDeltaTime = deltaTime;
    }
    if (deltaTime < stats.minDeltaTime)
    {
        stats.minDeltaTime = deltaTime;
    }

    stats.totalDeltaTime += deltaTime;
    stats.numCalls++;

    /* Update last call timestamp */
    stats.lastCallTime = currentTime;

    /* Print stats every PRINT_STATS_PERIOD_SECONDS seconds */
    if ((currentTime - stats.startTime) >= ((uint64_t)PRINT_STATS_PERIOD_SECONDS * 1000ULL))
    {
        printStats();

        /* Reset period counters */
        stats.startTime = currentTime;
        stats.maxDeltaTime = 0ULL;
        stats.minDeltaTime = UINT64_MAX;
        stats.numCalls = 0ULL;
        stats.totalDeltaTime = 0ULL;
    }
}

/**
 * @brief  Initialize the monitor baseline times.
 *
 * Call once before starting periodic timeMonitor() invocations.
 */
void timeMonitorStart(void)
{
    stats.lastCallTime = getCurrentTimeMillis();
    stats.startTime = stats.lastCallTime;
}

/**
 * @brief  Stop monitor and print final max delta time if the last interval exceeded previous max.
 */
void timeMonitorStop(void)
{
    if (stats.lastCallTime == 0ULL)
    {
        return;
    }

    uint64_t currentTime = getCurrentTimeMillis();
    uint64_t deltaTime = currentTime - stats.lastCallTime;

    if (deltaTime > stats.maxDeltaTime)
    {
        printf("Max Delta Time          : %10u ms\n", display_u64(deltaTime));
        stats.maxDeltaTime = deltaTime;
    }
}

/**
 * @brief  Test helper that runs a mock monitoring loop.
 *
 * This routine is blocking and uses a busy-wait delay; replace with a timer or RTOS sleep in production.
 */
void testTimeMonitor(void)
{
    /* Initialize baseline times */
    stats.startTime = getCurrentTimeMillis();
    stats.lastCallTime = stats.startTime;

    /* Mock periodic loop */
    while (true)
    {
        timeMonitor();

        /* crude busy-wait delay - replace with sleep in real use */
        for (volatile uint32_t i = 0U; i < 1000000U; i++)
        {
        }
    }
}

