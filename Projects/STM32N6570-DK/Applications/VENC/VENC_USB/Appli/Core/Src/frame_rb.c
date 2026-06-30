/**
******************************************************************************
* @file          frame_rb.c
* @author        MCD Application Team
* @brief        Frame ring buffer module
******************************************************************************
* @attention
*
* Copyright (c) 2018(-2022) STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32n6xx_hal.h"
#include "st_rb.h"
#include "frame_rb.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

typedef struct frb_frame_t
{
  uint32_t timeStamp;
  int32_t  szPayload;
  void * pData;

}frb_frame_t;

#define  FRB_NB_FRAMES   32U
#define  FRB_RB_SIZE    (500UL*1024UL)
#define  FRB_FRAME_SIZE (500UL*1024UL)

/* Private define ------------------------------------------------------------*/
#define NEXT_FRAME(i)      ((i + 1) % FRB_NB_FRAMES)
#define INCREMENT_FRAME(i) (i = NEXT_FRAME(i))
#define FRB_OVERFLOW()    (NEXT_FRAME(iNextFrame) == iOldestFrame)

/* Private macro -------------------------------------------------------------*/
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define ALIGN_TO_8_BYTES(size)  (((size) + 7) & ~0x07)

#define USE_INTERRUPT_LOCK 1
#define FRB_STATS_ENABLE 0
#if FRB_STATS_ENABLE
#define FRB_STATS_PERIOD_MS 5000U
#endif

/* Set to 1U to print expected frb_alloc() exhaustion traces, 0U to silence them. */
#ifndef FRB_ALLOC_FAIL_TRACE_ENABLE
#define FRB_ALLOC_FAIL_TRACE_ENABLE 0U
#endif

#if USE_INTERRUPT_LOCK
#define FRB_LOCK()      do { __disable_irq(); } while(0)
#define FRB_UNLOCK()    do { __enable_irq(); } while(0)
#else
#define FRB_LOCK()
#define FRB_UNLOCK()
#endif


#ifndef ST_TRACE_ERROR
#define ST_TRACE_ERROR printf
#endif



#ifndef ST_ASSERT
#define ST_ASSERT(cond) \
  do { \
    if (!(cond)) { \
      ST_TRACE_ERROR("ST_ASSERT failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__); \
    } \
  } while(0)
#endif

/* Private variables ---------------------------------------------------------*/
static frb_frame_t  pFrames[FRB_NB_FRAMES];
static uint32_t     iOldestFrame;
static uint32_t     iNextFrame;
static st_rb_t      hRb;
static bool         bInitDone = false;
static uint32_t     gCurrentFramesStored;
#if FRB_STATS_ENABLE
static uint32_t     gStatsWindowStartMs;
static uint32_t     gStatsPullCount;
static uint32_t     gStatsPullBytesSum;
static uint32_t     gStatsPullMin;
static uint32_t     gStatsPullMax;
static uint32_t     gStatsPushCount;
static uint32_t     gStatsPushBytesSum;
static uint32_t     gStatsPushMin;
static uint32_t     gStatsPushMax;
static uint32_t     gStatsMaxFramesStored;
#endif
static bool         gAllocPending;
static uint32_t     gAllocPrevWrite;
static uint32_t     gAllocGrantedBytes;

/* Private functions prototypes-----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
* @brief Initializes the ring buffer.
*/
bool frb_init(uint8_t * buffer, uint32_t bufferSize)
{

  if (bInitDone)
  {
    return true;
  }


  if(buffer == NULL || bufferSize  == 0)
  {
    ST_TRACE_ERROR("FRB  Alloc");
    return false;
  }

  /* Reset frame buffer*/
  memset(&hRb, 0, sizeof(hRb));
  hRb.szBuffer = bufferSize;
  hRb.pBuffer = buffer;
  st_rb_reset(&hRb,0);

  /* Reset list of frames*/
  memset(&pFrames, 0, sizeof(pFrames));

  gCurrentFramesStored = 0U;
#if FRB_STATS_ENABLE
  gStatsWindowStartMs = HAL_GetTick();
  gStatsPullCount = 0U;
  gStatsPullBytesSum = 0U;
  gStatsPullMin = 0U;
  gStatsPullMax = 0U;
  gStatsPushCount = 0U;
  gStatsPushBytesSum = 0U;
  gStatsPushMin = 0U;
  gStatsPushMax = 0U;
  gStatsMaxFramesStored = 0U;
#endif
  gAllocPending = false;
  gAllocPrevWrite = 0U;
  gAllocGrantedBytes = 0U;

  bInitDone = true;
  return true;
}

void frb_reset(void)
{
  if (bInitDone == false)
  {
    return;
  }

  FRB_LOCK();

  st_rb_reset(&hRb, 0);
  memset(&pFrames, 0, sizeof(pFrames));
  iOldestFrame = 0U;
  iNextFrame = 0U;

  gCurrentFramesStored = 0U;
#if FRB_STATS_ENABLE
  gStatsWindowStartMs = HAL_GetTick();
  gStatsPullCount = 0U;
  gStatsPullBytesSum = 0U;
  gStatsPullMin = 0U;
  gStatsPullMax = 0U;
  gStatsPushCount = 0U;
  gStatsPushBytesSum = 0U;
  gStatsPushMin = 0U;
  gStatsPushMax = 0U;
  gStatsMaxFramesStored = 0U;
#endif
  gAllocPending = false;
  gAllocPrevWrite = 0U;
  gAllocGrantedBytes = 0U;
  FRB_UNLOCK();
}


/**
 * @brief Gets the size of the ring buffer.
 */
uint32_t  frb_get_rb_size(void)
{
  return hRb.szBuffer;
}

/**
 * @brief Gets the address of the ring buffer.
 */
void * frb_get_rb_addr(void)
{
  return (void *)hRb.pBuffer;
}

uint32_t frb_get_frames_stored(void)
{
  uint32_t frames;
  FRB_LOCK();
  frames = gCurrentFramesStored;
  FRB_UNLOCK();
  return frames;
}

uint32_t frb_get_frames_capacity(void)
{
  return FRB_NB_FRAMES;
}
/**
* @brief Allocates a block of memory from a ring buffer.
*/
void *  frb_alloc(uint32_t * nbBytes)
{
  FRB_LOCK();

  uint32_t requestedNbBytes = *nbBytes;
  uint32_t  iRead  = hRb.iRead;
  uint32_t  iWrite = hRb.iWrite;
  void * pWrite= NULL;
  uint32_t maxNbBytes= 0;
  uint32_t framesAvailable;

  /* VENC requires an 8-byte-aligned start address. */
  uint32_t alignedNbBytes=  ALIGN_TO_8_BYTES(*nbBytes);

  /* If the buffer is empty, restart from the beginning to provide the largest chunk. */
  if  (iRead == iWrite)
  {
    hRb.iRead  = 0;
    hRb.iWrite = 0;

    iRead = 0;
    iWrite = 0;
  }

  pWrite =  hRb.pBuffer + hRb.iWrite;

  /* Look for the largest contiguous chunk. */

  /*                 iWrite <- maxNbBytes -> iRead           (szBuffer-1)     */
  /*                    |                      |                |             */
  /* 0...1...2...    ...x...                ...y...          ...z             */
  if (iRead > iWrite)
  {
    maxNbBytes =  (iRead - iWrite - 1U);
  }
  /* <- maxNbBytes? ->  iRead ...   iWrite <- maxNbBytes? -> (szBuffer-1)     */
  /*                     |            |                           |           */
  /* 0...1...2...    ...x...       ...y...                      ...z          */
  else
  {
    maxNbBytes = hRb.szBuffer -  iWrite;
    if (iRead > maxNbBytes)
    {
      maxNbBytes = iRead;
      pWrite =  hRb.pBuffer;
    }
  }

  /* Return current write pointer if enough contiguous bytes  */
  if (maxNbBytes >= alignedNbBytes)
  {
    while (maxNbBytes < *nbBytes)
    {
      ST_TRACE_ERROR("Allocation size mismatch: available=%lu bytes, requested=%lu bytes",
                     (unsigned long)maxNbBytes, (unsigned long)(*nbBytes));
    }

    //ST_TRACE_ERROR("Alloc : %d vs required %d", maxNbBytes, *nbBytes);

  /* Return the maximum contiguous bytes available. */
    *nbBytes = maxNbBytes;

    gAllocPending = true;
    gAllocPrevWrite = iWrite;
    gAllocGrantedBytes = maxNbBytes;

    st_rb_force_write(&hRb, pWrite);

    FRB_UNLOCK();
    /* Return the frame start address. */
    return (void *) pWrite;
  }

  if (iOldestFrame > iNextFrame)
  {
    framesAvailable = iOldestFrame - iNextFrame - 1U;
  }
  else
  {
    framesAvailable = FRB_NB_FRAMES + iOldestFrame - iNextFrame - 1U;
  }

  /* Return 0, NULL if not enough space */
  if (FRB_ALLOC_FAIL_TRACE_ENABLE)
  {
    ST_TRACE_ERROR("frb_alloc() fails :\n"
                   ".memory requested : %lu Bytes / Available %lu Bytes\n"
                   ".frames available : %lu\n",
                   (unsigned long)requestedNbBytes,
                   (unsigned long)maxNbBytes,
                   (unsigned long)framesAvailable);
  }

  *nbBytes = 0;

  FRB_UNLOCK();
  return NULL;
}

/**
 * @brief Releases the last pending allocation done by frb_alloc.
 */
void frb_free(uint32_t nbBytes)
{
  uint32_t alignedNbBytes;
  uint32_t newWrite;

  FRB_LOCK();

  if (gAllocPending == false)
  {
    FRB_UNLOCK();
    return;
  }

  /* UINT32_MAX is used as a force-release sentinel by callers. */
  if (nbBytes == UINT32_MAX)
  {
    alignedNbBytes = gAllocGrantedBytes;
  }
  else
  {
    alignedNbBytes = ALIGN_TO_8_BYTES(nbBytes);
  }

  if (alignedNbBytes == 0U)
  {
    FRB_UNLOCK();
    return;
  }

  if (alignedNbBytes >= gAllocGrantedBytes)
  {
    if (alignedNbBytes > gAllocGrantedBytes)
    {
      ST_TRACE_ERROR("FRB free request too large: granted=%lu bytes, requested=%lu bytes",
                     (unsigned long)gAllocGrantedBytes,
                     (unsigned long)alignedNbBytes);
    }

    /* Full release: restore write index from before frb_alloc(). */
    hRb.iWrite = gAllocPrevWrite;
    gAllocPending = false;
    gAllocPrevWrite = 0U;
    gAllocGrantedBytes = 0U;

    FRB_UNLOCK();
    return;
  }

  /* Partial release: consume from the tail of the last allocated contiguous region. */
  newWrite = (gAllocPrevWrite + (gAllocGrantedBytes - alignedNbBytes)) % hRb.szBuffer;
  hRb.iWrite = newWrite;
  gAllocPrevWrite = newWrite;
  gAllocGrantedBytes -= alignedNbBytes;

  FRB_UNLOCK();
}

/**
 * @brief Writes a frame to the ring buffer and updates the frame list.
 */
bool frb_push(void *frame, uint32_t nbBytes, uint32_t timeStamp)
{
  FRB_LOCK();
  /* Venc requires 8 bytes aligned start @*/
  uint32_t alignedNbBytes=  ALIGN_TO_8_BYTES(nbBytes);

  /* Ensure that the frame is located at the end of the ring buffer */
  ST_ASSERT(frame == (void *) st_rb_write_ptr(&hRb));

  /* Ensure that the frame has the correct size */
  ST_ASSERT(alignedNbBytes == st_rb_write_count_aligned(&hRb, alignedNbBytes));

  /* Check enough place in frame list */
  if (FRB_OVERFLOW())
  {
    FRB_UNLOCK();
    ST_TRACE_ERROR("FRB overflow (fr)");
    return false;
  }

  /* Update the frame list */
  pFrames[iNextFrame].timeStamp = timeStamp;
  pFrames[iNextFrame].szPayload = nbBytes; /* Keep the real size*/
  pFrames[iNextFrame].pData     = frame;
  INCREMENT_FRAME(iNextFrame);

#if FRB_STATS_ENABLE
  if (nbBytes > 0U)
  {
    gStatsPushCount++;
    gStatsPushBytesSum += nbBytes;
    if ((gStatsPushMin == 0U) || (nbBytes < gStatsPushMin))
    {
      gStatsPushMin = nbBytes;
    }
    if (nbBytes > gStatsPushMax)
    {
      gStatsPushMax = nbBytes;
    }
  }
  gCurrentFramesStored++;
  if (gCurrentFramesStored > gStatsMaxFramesStored)
  {
    gStatsMaxFramesStored = gCurrentFramesStored;
  }
#else
  gCurrentFramesStored++;
#endif

  /* Move the write pointer */
  st_rb_write_move(&hRb, alignedNbBytes);

  gAllocPending = false;
  gAllocPrevWrite = 0U;
  gAllocGrantedBytes = 0U;

  FRB_UNLOCK();
#if FRB_STATS_ENABLE
  frb_stats();
#endif
  return true;
}

/**
 * @brief Retrieves the oldest frame from the frame list.
 */
void *frb_pull(uint32_t *size, uint32_t * timeStamp)
{
  uint32_t payloadSize;
  FRB_LOCK();
  /* Return the oldest frame parameters */
  /* Those parameters may be NULL if no frame is ready */
  *size = pFrames[iOldestFrame].szPayload;
  *timeStamp =  pFrames[iOldestFrame].timeStamp;

  payloadSize = *size;
  if (payloadSize > 0U)
  {
#if FRB_STATS_ENABLE
    gStatsPullCount++;
    gStatsPullBytesSum += payloadSize;
    if ((gStatsPullMin == 0U) || (payloadSize < gStatsPullMin))
    {
      gStatsPullMin = payloadSize;
    }
    if (payloadSize > gStatsPullMax)
    {
      gStatsPullMax = payloadSize;
    }
#endif
  }
  FRB_UNLOCK();
#if FRB_STATS_ENABLE
  frb_stats();
#endif
  return pFrames[iOldestFrame].pData;
}


/**
 * @brief Returns the oldest frame to the ring buffer.
 */
void frb_return_frame(void)
{
  FRB_LOCK();

  /*Total size was  8 bytes aligned  @*/
  uint32_t alignedNbBytes=  ALIGN_TO_8_BYTES(pFrames[iOldestFrame].szPayload);

  st_rb_force_read(&hRb, (uint8_t *)pFrames[iOldestFrame].pData + alignedNbBytes);
  st_rb_force_read(&hRb, (uint8_t *)((uint32_t)pFrames[iOldestFrame].pData + alignedNbBytes));

  /* Reset the latest frame consumed */
  memset(&pFrames[iOldestFrame], 0, sizeof(pFrames[iOldestFrame]));

  /* Increment the index of the oldest frame */
  INCREMENT_FRAME(iOldestFrame);

  if (gCurrentFramesStored > 0U)
  {
    gCurrentFramesStored--;
  }

  FRB_UNLOCK();
}

void frb_stats(void)
{
#if FRB_STATS_ENABLE
  uint32_t now;
  uint32_t elapsedMs;
  uint32_t pullCount;
  uint32_t pullBytesSum;
  uint32_t pullMin;
  uint32_t pullMax;
  uint32_t pushCount;
  uint32_t pushBytesSum;
  uint32_t pushMin;
  uint32_t pushMax;
  uint32_t maxFramesStored;
  uint32_t pushesPerSec_x100;
  uint32_t pullsPerSec_x100;
  uint32_t avgPushSize;
  uint32_t avgPullSize;
  uint64_t avgPushBitrateBps;
  uint64_t avgBitrateBps;
  uint64_t pushBitrateKbps_x100;
  uint64_t pushBitrateMbps_x100;
  uint64_t bitrateKbps_x100;
  uint64_t bitrateMbps_x100;
  const char *pushBitrateUnit = "kbps";
  const char *pullBitrateUnit = "kbps";
  uint64_t pushBitrate_x100 = 0ULL;
  uint64_t pullBitrate_x100 = 0ULL;
  uint32_t pushBitrateInt;
  uint32_t pushBitrateFrac;
  uint32_t pullBitrateInt;
  uint32_t pullBitrateFrac;

  now = HAL_GetTick();
  elapsedMs = now - gStatsWindowStartMs;
  if (elapsedMs < FRB_STATS_PERIOD_MS)
  {
    return;
  }

  FRB_LOCK();
  now = HAL_GetTick();
  elapsedMs = now - gStatsWindowStartMs;
  if (elapsedMs < FRB_STATS_PERIOD_MS)
  {
    FRB_UNLOCK();
    return;
  }

  pullCount = gStatsPullCount;
  pullBytesSum = gStatsPullBytesSum;
  pullMin = gStatsPullMin;
  pullMax = gStatsPullMax;
  pushCount = gStatsPushCount;
  pushBytesSum = gStatsPushBytesSum;
  pushMin = gStatsPushMin;
  pushMax = gStatsPushMax;
  maxFramesStored = gStatsMaxFramesStored;

  gStatsWindowStartMs = now;
  gStatsPullCount = 0U;
  gStatsPullBytesSum = 0U;
  gStatsPullMin = 0U;
  gStatsPullMax = 0U;
  gStatsPushCount = 0U;
  gStatsPushBytesSum = 0U;
  gStatsPushMin = 0U;
  gStatsPushMax = 0U;
  gStatsMaxFramesStored = gCurrentFramesStored;
  FRB_UNLOCK();

  if (pushCount > 0U)
  {
    avgPushSize = pushBytesSum / pushCount;
    pushesPerSec_x100 = (pushCount * 100000U) / elapsedMs;
    avgPushBitrateBps = (((uint64_t)pushBytesSum) * 8000ULL) / (uint64_t)elapsedMs;
  }
  else
  {
    avgPushSize = 0U;
    pushesPerSec_x100 = 0U;
    avgPushBitrateBps = 0ULL;
  }

  if (pullCount > 0U)
  {
    avgPullSize = pullBytesSum / pullCount;
    pullsPerSec_x100 = (pullCount * 100000U) / elapsedMs;
    avgBitrateBps = (((uint64_t)pullBytesSum) * 8000ULL) / (uint64_t)elapsedMs;
  }
  else
  {
    avgPullSize = 0U;
    pullsPerSec_x100 = 0U;
    avgBitrateBps = 0ULL;
  }

  pushBitrateKbps_x100 = (avgPushBitrateBps * 100ULL) / 1000ULL;
  pushBitrateMbps_x100 = (avgPushBitrateBps * 100ULL) / 1000000ULL;
  bitrateKbps_x100 = (avgBitrateBps * 100ULL) / 1000ULL;
  bitrateMbps_x100 = (avgBitrateBps * 100ULL) / 1000000ULL;

  if (avgPushBitrateBps >= 1000000ULL)
  {
    pushBitrateUnit = "Mbps";
    pushBitrate_x100 = pushBitrateMbps_x100;
  }
  else
  {
    pushBitrateUnit = "kbps";
    pushBitrate_x100 = pushBitrateKbps_x100;
  }

  if (avgBitrateBps >= 1000000ULL)
  {
    pullBitrateUnit = "Mbps";
    pullBitrate_x100 = bitrateMbps_x100;
  }
  else
  {
    pullBitrateUnit = "kbps";
    pullBitrate_x100 = bitrateKbps_x100;
  }

  pushBitrateInt  = (uint32_t)(pushBitrate_x100 / 100ULL);
  pushBitrateFrac = (uint32_t)(pushBitrate_x100 % 100ULL);
  pullBitrateInt  = (uint32_t)(pullBitrate_x100 / 100ULL);
  pullBitrateFrac = (uint32_t)(pullBitrate_x100 % 100ULL);


  printf("FRB stats [%lu ms] max_frames=%lu\n"
         "  push: size[min/max/avg]=%lu/%lu/%lu B, rate=%lu.%02lu/s, bitrate=%lu.%02lu %s\n",
         (unsigned long)elapsedMs,
         (unsigned long)maxFramesStored,
         (unsigned long)pushMin,
         (unsigned long)pushMax,
         (unsigned long)avgPushSize,
         (unsigned long)(pushesPerSec_x100 / 100U),
         (unsigned long)(pushesPerSec_x100 % 100U),
         (unsigned long)pushBitrateInt,
         (unsigned long)pushBitrateFrac,
         pushBitrateUnit);

  printf("  pull: size[min/max/avg]=%lu/%lu/%lu B, rate=%lu.%02lu/s, bitrate=%lu.%02lu %s\n",
         (unsigned long)pullMin,
         (unsigned long)pullMax,
         (unsigned long)avgPullSize,
         (unsigned long)(pullsPerSec_x100 / 100U),
         (unsigned long)(pullsPerSec_x100 % 100U),
         (unsigned long)pullBitrateInt,
         (unsigned long)pullBitrateFrac,
         pullBitrateUnit);
#else
  /* Stats collection is compiled out in this configuration. */
#endif
}
