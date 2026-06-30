/**
******************************************************************************
* @file    appli_flash_layout.h
* @author  GPM Application Team
* @brief   Application configuration file
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

#ifndef __APPLI_FLASH_LAYOUT_H__
#define __APPLI_FLASH_LAYOUT_H__

/* ########### HEADER GENERATED AUTOMATICALLY, DO NOT EDIT ########### */


#define MCUBOOT_OVERWRITE_ONLY 0x1  /* Overwrite option */
#define S_DATA_IMAGE_NUMBER    0x1  /* Number of secure data */
#define NS_DATA_IMAGE_NUMBER   0x0  /* Number of non secure data */
#define DOWNLOAD_MENU          0x1  /* Download menu available */

#define NS_DATA_ADDRESS                0x0  /* Non secure data address */

#define S_APPLI_OFFSET                 0x80000  /* Secure application primary slot offset */
#define NS_APPLI_OFFSET                0x0  /* Non secure application primary slot offset */
#define S_DATA_OFFSET                  0xA0000  /* Secure data primary slot offset */
#define NS_DATA_OFFSET                 0x0  /* Non secure data primary slot offset */

#define S_APPLI_SECONDARY_OFFSET       0xB0000  /* Secure application secondary slot offset */
#define NS_APPLI_SECONDARY_OFFSET      0x0  /* Non secure application secondary slot offset */
#define S_DATA_SECONDARY_OFFSET        0xD0000  /* Secure data secondary slot offset */
#define NS_DATA_SECONDARY_OFFSET       0x0  /* Non secure data secondary slot offset */

#define S_APPLI_PARTITION_SIZE         0x20000  /* Secure application partition size */
#define NS_APPLI_PARTITION_SIZE        0x0  /* Non secure application partition size */
#define S_DATA_PARTITION_SIZE          0x10000  /* Secure data partition size */
#define NS_DATA_PARTITION_SIZE         0x0  /* Non secure data partition size */

#endif /* __APPLI_FLASH_LAYOUT_H__ */
