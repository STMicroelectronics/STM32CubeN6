/**
    ******************************************************************************
    * @file           : st_monitor_bitrate.c
    * @brief          : Implement bitrate monitoring
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "st_monitor_bitrate.h"
#include "tx_api.h"
#include "stm32n6xx_hal.h"

__weak void Error_Handler(void)
{
  printf("Error_Handler called at %s:%d\n", __FILE__, __LINE__);
  while(1);
}

typedef struct {
    uint64_t total_bytes;
    uint32_t frame_count;
    float    last_bitrate_kbps;
    float    min_bitrate_kbps;
    float    max_bitrate_kbps;
    uint32_t last_tick;
    uint32_t bytes_this_sec;
    uint32_t frames_this_sec;
    float    last_framerate_fps;
} bitrate_stats_t;

static bitrate_stats_t audio_stats = {0};
static bitrate_stats_t video_stats = {0};

static bitrate_stats_t* get_stats(const char* stream) {
    if (strcmp(stream, "audio") == 0) return &audio_stats;
    if (strcmp(stream, "video") == 0) return &video_stats;
    return NULL;
}

/* Display statistics every 5 seconds */
#define MONITOR_DISPLAY_SECONDS  (5)
#define MONITOR_DISPLAY_TICKS    (TX_TIMER_TICKS_PER_SECOND*MONITOR_DISPLAY_SECONDS)

void monitor_bitrate(const char* stream, uint32_t frameSize)
{
    bitrate_stats_t* stats = get_stats(stream);
    if (!stats) return;

    stats->total_bytes += frameSize;
    stats->frame_count++;
    stats->bytes_this_sec += frameSize;
    stats->frames_this_sec++;

    ULONG now = tx_time_get();
    //printf("time : %d\n", now);
    ULONG elapsed_ticks = now - stats->last_tick;
    if (elapsed_ticks >= MONITOR_DISPLAY_TICKS) {
        uint32_t framerate_fps_x10;
        float seconds = (float)MONITOR_DISPLAY_SECONDS;
        float bitrate_kbps = (float)(stats->bytes_this_sec * 8) / (seconds * 1000.0f);
        float framerate_fps = (float)stats->frames_this_sec / seconds;

        framerate_fps_x10 = (stats->frames_this_sec * 10U + (MONITOR_DISPLAY_SECONDS / 2U)) / MONITOR_DISPLAY_SECONDS;

        stats->last_bitrate_kbps = bitrate_kbps;
        stats->last_framerate_fps = framerate_fps;

        if (stats->frame_count == 1) {
            stats->min_bitrate_kbps = bitrate_kbps;
            stats->max_bitrate_kbps = bitrate_kbps;
        } else {
            if (bitrate_kbps < stats->min_bitrate_kbps) stats->min_bitrate_kbps = bitrate_kbps;
            if (bitrate_kbps > stats->max_bitrate_kbps) stats->max_bitrate_kbps = bitrate_kbps;
        }

        if (strcmp(stream, "video") == 0)
        {
            printf("[%s] bitrate: last :%u kbps, min: %u kbps, max: %u kbps // framerate : %u.%u fps\n",
                stream,
                (unsigned int)bitrate_kbps,
                (unsigned int)stats->min_bitrate_kbps,
                (unsigned int)stats->max_bitrate_kbps,
                (unsigned int)(framerate_fps_x10 / 10U),
                (unsigned int)(framerate_fps_x10 % 10U));
        }
        else
        {
            printf("[%s] bitrate: last :%u kbps, min: %u kbps, max: %u kbps\n",
                stream,
                (unsigned int)bitrate_kbps,
                (unsigned int)stats->min_bitrate_kbps,
                (unsigned int)stats->max_bitrate_kbps);
        }

        stats->bytes_this_sec = 0;
        stats->frames_this_sec = 0;
        stats->last_tick = now;
    }
}

void monitor_get_stats(const char* stream, float* last_bitrate_kbps, float* min_bitrate_kbps, float* max_bitrate_kbps)
{
    bitrate_stats_t* stats = get_stats(stream);
    if (!stats) return;
    if (last_bitrate_kbps) *last_bitrate_kbps = stats->last_bitrate_kbps;
    if (min_bitrate_kbps) *min_bitrate_kbps = stats->min_bitrate_kbps;
    if (max_bitrate_kbps) *max_bitrate_kbps = stats->max_bitrate_kbps;
}
