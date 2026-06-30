/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 * Modifications are Copyright (c) 2019 Arm Limited.
 * Copyright (c) 2024 STMicroelectronics.
 */

#include <string.h>
#include "mcuboot_config/mcuboot_config.h"
#include "sysflash/sysflash.h"
#include <bootutil/sign_key.h>
#include "flash_layout.h"
#include "low_level_otp.h"

#if defined(MCUBOOT_SIGN_EC256)
extern uint8_t Secure_Authentication_Public_Key_HASH[];
extern uint8_t NonSecure_Authentication_Public_Key_HASH[];
unsigned int pub_key_len = SHA256_LENGTH;
struct bootutil_key bootutil_keys[] = {
{
  .key = Secure_Authentication_Public_Key_HASH,
  .len = &pub_key_len,
},
{
  .key = NonSecure_Authentication_Public_Key_HASH,
  .len = &pub_key_len,
},
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
{
  .key = Secure_Authentication_Public_Key_HASH,
  .len = &pub_key_len,
},
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
{
  .key = NonSecure_Authentication_Public_Key_HASH,
  .len = &pub_key_len,
},
#endif
};
#endif /* MCUBOOT_SIGN_EC256 */
const int bootutil_key_cnt = MCUBOOT_IMAGE_NUMBER;
#if defined(MCUBOOT_ENC_IMAGES)
#if defined(MCUBOOT_ENCRYPT_EC256)
extern uint8_t Encryption_Private_Key[];
unsigned int Encryption_Private_Key_Length = ECDSA_256_PRIV_KEY_LENGTH;
struct bootutil_key bootutil_enc_key =
{
  .key = Encryption_Private_Key,
  .len = (unsigned int*)&Encryption_Private_Key_Length,
};
#endif /* MCUBOOT_ENCRYPT_EC256 */
#endif /* MCUBOOT_ENC_IMAGES */

/**
  * @brief Retrieve the hash of the corresponding public key.
  * @param image_index Index of the image.
  * @param public_key_hash Buffer to store the key-hash in.
  * @param key_hash_size Size of the buffer
  * @return 0 on success; nonzero on failure.
  */
int boot_retrieve_public_key_hash(uint8_t image_index,
                                  uint8_t *public_key_hash,
                                  size_t *key_hash_size)
{
  int status = 0;

  switch(FLASH_AREA_IMAGE_PRIMARY(image_index))
  {
    case FLASH_AREA_0_ID:
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
    case FLASH_AREA_4_ID:
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
      memcpy(public_key_hash, Secure_Authentication_Public_Key_HASH, *key_hash_size);
    break;
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
    case FLASH_AREA_1_ID:
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
    case FLASH_AREA_5_ID:
#endif
#if (MCUBOOT_APP_IMAGE_NUMBER == 2) || (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
        memcpy(public_key_hash, NonSecure_Authentication_Public_Key_HASH, *key_hash_size);
        break;
#endif

    default:
      status = 1;
    break;
  }

  return status;
}
