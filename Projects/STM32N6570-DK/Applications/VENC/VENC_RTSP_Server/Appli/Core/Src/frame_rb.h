#ifndef FRAME_RB_H
#define FRAME_RB_H

/**
 * @file frame_rb.h
 * @brief Frame ring buffer module
 *
 * This file contains the declarations for the frame ring buffer module.
 * It provides functions to initialize the ring buffer, allocate memory,
 * push frames, pull frames, and return frames.
 *
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief Initializes the ring buffer.
 *
 * This function initializes the ring buffer if it has not been initialized yet. It allocates memory
 * for the ring buffer from a specific memory type, resets the ring buffer, and initializes the list of frames.
 *
 * @return `true` if the initialization is successful, `false` otherwise.
 */
bool frb_init(uint8_t * buffer, uint32_t bufferSize);

/**
 * @brief Resets FRB content and statistics while keeping initialized storage.
 */
void frb_reset(void);

/**
 * @brief Gets the size of the ring buffer.
 *
 * @return The size of the ring buffer.
 */
uint32_t frb_get_rb_size(void);

/**
 * @brief Gets the address of the ring buffer.
 *
 * @return A pointer to the ring buffer.
 */
void *frb_get_rb_addr(void);

/**
 * @brief Gets the current number of frames stored in FRB.
 *
 * @return Number of frames currently queued in FRB.
 */
uint32_t frb_get_frames_stored(void);

/**
 * @brief Gets the maximum frame slots available in FRB.
 *
 * @return Total frame slot capacity.
 */
uint32_t frb_get_frames_capacity(void);

/**
 * @brief Allocates a block of memory from a ring buffer.
 *
 * This function looks for the biggest contiguous/8 bytes aligned memory chunk available in ring buffer.
 * It returns the current write pointer and size if there is enough space.
 * If there is not enough space, it returns NULL.
 *
 * @param nbBytes The number of bytes to allocate.
 * @return A pointer to the allocated memory block if successful, or NULL if there is not enough space.
 */
void *frb_alloc(uint32_t *nbBytes);

/**
 * @brief Releases the last pending allocation from the ring buffer.
 *
 * This function releases bytes from the tail of the most recent successful
 * frb_alloc() reservation. If nbBytes is greater than or equal to the pending
 * reserved size, the whole reservation is canceled.
 *
 * @param nbBytes Number of bytes to release (used for consistency checks).
 */
void frb_free(uint32_t nbBytes);

/**
 * @brief Writes a frame to the ring buffer and updates the frame list.
 *
 * @param frame The frame to write.
 * @param nbBytes The number of bytes in the frame.
 * @param timeStamp The timestamp of the frame.
 * @return `true` if the frame is successfully written, `false` otherwise.
 */
bool frb_push(void *frame, uint32_t nbBytes, uint32_t timeStamp);

/**
 * @brief Retrieves the oldest frame from the frame list.
 *
 * @param size A pointer to store the size of the frame.
 * @param timeStamp A pointer to store the timestamp of the frame.
 * @return A pointer to the oldest frame.
 */
void *frb_pull(uint32_t *size, uint32_t *timeStamp);

/**
 * @brief Returns the oldest frame to the ring buffer.
 */
void frb_return_frame(void);

/**
 * @brief Updates and periodically prints FRB statistics.
 *
 * Prints statistics every 5 seconds then resets the statistic window.
 */
void frb_stats(void);

#endif // FRAME_RB_H