/**
******************************************************************************
* @file          st_rb.h
* @author        MCD Application Team
* @brief         implement a ring buffer
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


/* Exported typedef -----------------------------------------------------------*/

typedef struct st_rb_t
{
  uint8_t *pBuffer;
  uint32_t szBuffer;
  // ARM says, Load and Store instructions are atomic
  // and it's execution is guaranteed to be complete before interrupt handler executes
  // So, no lock code needed
  volatile uint32_t iWrite;
  volatile uint32_t iRead;
} st_rb_t;


/* Exported defines -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief Init a byte ring buffer
 *
 * @param pRb   instance  pointer
 * @param  pBuffer  ring buffer
 * @param  szBuffer  ring buffer size
 * @return true if OK, else false
 */
static inline bool st_rb_init(st_rb_t *pRb, void *pBuffer,uint32_t szBuffer)
{
  bool ok = true;
  memset(pRb, 0, sizeof(*pRb));
  pRb->szBuffer = szBuffer;
  pRb->pBuffer = pBuffer;
  if (pRb->pBuffer != NULL)
  {
    memset(pRb->pBuffer, 0, szBuffer);
  }
  else
  {
    ok = false;
  }
  return ok;
}


/**
 * @brief terminate a byte ring buffer
 *
 * @param pRb   instance  pointer
 */

static inline  void st_rb_term(st_rb_t *pRb)
{
    pRb->pBuffer = NULL;
    pRb->szBuffer = 0UL;
}


/**
 * @brief terminate a byte ring buffer
 *
 * @param pRb   instance  pointer
 */

static inline  int8_t st_rb_is_valid(st_rb_t *pRb)
{
    return (pRb->pBuffer != NULL);
}



/**
 * @brief reset
 *
 * @param pRb   instance  pointer
 */

static inline  void st_rb_reset(st_rb_t *pRb, uint32_t value)
{
  pRb->iRead = 0;
  pRb->iWrite = value;
  memset(pRb->pBuffer, 0, pRb->szBuffer);

}


/**
 * @brief return the read pointer ( consumer )
 *
 * @param pRb   instance  pointer
 * @return a ptr
 */
static inline  uint8_t *st_rb_read_ptr(st_rb_t *pRb)
{
  return pRb->pBuffer + pRb->iRead;
}

/**
 * @brief return the write pointer ( producer)
 *
 * @param pRb   instance  pointer
 * @return a ptr
 */
static inline  uint8_t *st_rb_write_ptr(st_rb_t *pRb)
{
  return pRb->pBuffer + pRb->iWrite;
}


/**
 * @brief return the  size to read available
 *
 * @param pRb   instance  pointer
 * @return uint32_t read size  available
 */
static inline  uint32_t st_rb_read_available(st_rb_t *pRb)
{
  uint32_t const iRead  = pRb->iRead;
  uint32_t const iWrite = pRb->iWrite;
  return (iWrite >= iRead) ? (iWrite - iRead) : (pRb->szBuffer + iWrite - iRead);
}


/**
 * @brief resize the write count to the maximum aligned in the buffer
 *
 * @param pRb   instance  pointer
 * @param count count produced
 * @return uint32_t read size  available
 */

static inline  uint32_t st_rb_write_count_aligned(st_rb_t *pRb, uint32_t count)
{
  uint32_t const iWrite = pRb->iWrite;
  if ((iWrite + count) > pRb->szBuffer)
  {
    count = pRb->szBuffer - iWrite;
  }
  return count;
}


/**
 * @brief move the read position
 *
 * @param pRb   instance  pointer
 * @param count count consumed
 * @return uint32_t read size  available
 */
static inline  void st_rb_write_move(st_rb_t *pRb, uint32_t count)
{
  uint32_t iWrite = (pRb->iWrite + count) % pRb->szBuffer;
  pRb->iWrite = iWrite ;
}

/**
 * @brief move the write position
 *
 * @param pRb   instance  pointer
 * @param count count produced
 * @return uint32_t read size  available
 */

static inline  void st_rb_read_move(st_rb_t *pRb, uint32_t count)
{
  uint32_t iRead = (pRb->iRead + count) % pRb->szBuffer;
  pRb->iRead = iRead ;
}

/**
 * @brief force the read position according to a read address
 *
 * @param pRb   instance  pointer
 * @param buffer read position
 */


static inline  void st_rb_force_read(st_rb_t *pRb, uint8_t *buffer)
{
  pRb->iRead = buffer - pRb->pBuffer;
}

/**
 * @brief force the write position according to a write address
 *
 * @param pRb   instance  pointer
 * @param buffer write position
 */


static inline  void st_rb_force_write(st_rb_t *pRb, uint8_t *buffer)
{
  pRb->iWrite = buffer - pRb->pBuffer;
}



/**
 * @brief resize the read count to the maximum aligned in the buffer
 *
 * @param pRb   instance  pointer
 * @param count count produced
 * @return uint32_t read size  available
 */

static inline  uint32_t st_rb_read_count_aligned(st_rb_t *pRb, uint32_t count)
{
  uint32_t const iRead = pRb->iRead;
  if ((iRead + count) > pRb->szBuffer)
  {
    count = pRb->szBuffer - iRead;
  }
  return count;
}



/**
 * @brief return the write size to write  available
 *
 * @param pHandle   instance  pointer
 * @return uint32_t write  size  available
 */

static inline  uint32_t st_rb_write_available(st_rb_t *pRb)
{
  uint32_t const iRead  = pRb->iRead;
  uint32_t const iWrite = pRb->iWrite;
  return (iRead > iWrite) ? (iRead - iWrite - 1U) : (pRb->szBuffer + iRead - iWrite - 1U);
  /*
  remove 1 to the size to prevent read=write that could mean buffer empty or buffer full
  we assume read==write read empty
  */
}

static inline void st_rb_write_ensure_available(st_rb_t *pRb, uint32_t size)
{
  // uint32_t sz = st_rb_write_available(pRb);
  st_rb_write_available(pRb);
  if (st_rb_write_available(pRb) < size)
  {
    uint32_t szSeek = size - st_rb_write_available(pRb);
    st_rb_read_move(pRb, szSeek);
    szSeek = st_rb_write_available(pRb);

  }
}



/**
* @brief produce data in the ring buffer
* @param pRb   instance  pointer
* @param pBuffer  buffer pointer
* @param szBuffer  buffer size
* @return count not written
*/
static inline uint32_t st_rb_write(st_rb_t *pRb, uint8_t *pBuffer, uint32_t szBuffer)
{
  uint32_t szWritten = 0;

  if (st_rb_write_available(pRb) >= szBuffer)
  {
    while (szBuffer)
    {
      uint32_t countAligned = st_rb_write_count_aligned(pRb, szBuffer);
      if (countAligned > szBuffer)
      {
        countAligned = szBuffer;
      }
      memcpy(st_rb_write_ptr(pRb), pBuffer, countAligned);
      st_rb_write_move(pRb, countAligned);
      pBuffer += countAligned;
      szBuffer -= countAligned;
      szWritten += countAligned;
    }
  }
  return szWritten;
}



/**
* @brief consume data from the ring buffer
* @param pRb   instance  pointer
* @param pBuffer  buffer pointer
* @param szBuffer  buffer size
* @return count not read
*/
static inline uint32_t st_rb_read(st_rb_t *pRb, uint8_t *pBuffer, uint32_t szBuffer)
{
  uint32_t szRead = 0;
  if (st_rb_read_available(pRb) >= szBuffer)
  {
    while (szBuffer)
    {
      uint32_t countAligned = st_rb_read_count_aligned(pRb, szBuffer);
      if (countAligned > szBuffer)
      {
        countAligned = szBuffer;
      }
      memcpy(pBuffer, st_rb_read_ptr(pRb), countAligned);
      st_rb_read_move(pRb, countAligned);
      pBuffer += countAligned;
      szBuffer -= countAligned;
      szRead += countAligned;


    }
  }
  return szRead;
}




/**
* @brief consume data from the ring buffer without moving the position
* @param pRb   instance  pointer
* @param pBuffer  buffer pointer
* @param szBuffer  buffer size
* @return count  read
*/
static inline uint32_t st_rb_read_fetch(st_rb_t *pRb, uint8_t *pBuffer, uint32_t szBuffer)
{
  uint32_t index = 0UL;
  if (st_rb_read_available(pRb) >= szBuffer)
  {
    while (szBuffer)
    {
      *pBuffer ++ = pRb->pBuffer[(pRb->iRead + index) % pRb->szBuffer];
      index ++;
      szBuffer--;
    }
  }
  return index;
}