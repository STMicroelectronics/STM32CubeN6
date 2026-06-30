/**
  ******************************************************************************
  * @file    low_level_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_device
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
#include "appli_flash_layout.h"
#include "low_level_ext_flash.h"

XSPI_HandleTypeDef XSPI_HANDLE;

static struct ext_flash_range access_vect[] =
{
  { S_APPLI_SECONDARY_OFFSET, S_APPLI_SECONDARY_OFFSET + S_APPLI_PARTITION_SIZE - 1},
#if (S_DATA_IMAGE_NUMBER == 1)
  { S_DATA_SECONDARY_OFFSET, S_DATA_SECONDARY_OFFSET + S_DATA_PARTITION_SIZE - 1},
#endif /* (S_DATA_IMAGE_NUMBER == 1) */
};

#if (MCUBOOT_OVERWRITE_ONLY == 1)
#define write_vect access_vect
#else
static struct ext_flash_range write_vect[] =
{
  { S_APPLI_SECONDARY_OFFSET, S_APPLI_SECONDARY_OFFSET + S_APPLI_PARTITION_SIZE - 1},
  { S_APPLI_OFFSET + S_APPLI_PARTITION_SIZE - 32, S_APPLI_OFFSET + S_APPLI_PARTITION_SIZE - 16 - 1},
#if (S_DATA_IMAGE_NUMBER == 1)
  { S_DATA_SECONDARY_OFFSET, S_DATA_SECONDARY_OFFSET + S_DATA_PARTITION_SIZE - 1},
  { S_DATA_OFFSET + S_DATA_PARTITION_SIZE - 32, S_DATA_OFFSET + S_DATA_PARTITION_SIZE - 16 - 1},
#endif /* (NS_DATA_IMAGE_NUMBER == 1) */
};
#endif /* MCUBOOT_OVERWRITE_ONLY == 1 */

struct low_level_ext_flash_device EXT_FLASH0_DEV =
{
  .erase = { .nb = sizeof(access_vect) / sizeof(struct ext_flash_range), .range = access_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct ext_flash_range), .range = write_vect},
};
