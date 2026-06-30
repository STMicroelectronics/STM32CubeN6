/**
  ******************************************************************************
  * @file    stm32_custom_driver_private_define.h
  * @author  MCD Application Team
  * @brief   This file contains custom driver private define definitions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_CUSTOM_PRIVATE_DEFINE_H
#define __STM32_CUSTOM_PRIVATE_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup CUSTOM
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_CUSTOM == 1

/* Instruction defines -----------------------------------------------------*/
/**
  * @brief 1-line instruction, SDR mode (DTR disabled)
  */
#define INSTRUCTION_1S          (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_DTR_DISABLE)

/**
  * @brief 1-line instruction, DTR mode (DTR enabled)
  */
#define INSTRUCTION_1D          (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_DTR_ENABLE)

/**
  * @brief 2-line instruction, SDR mode (DTR disabled)
  */
#define INSTRUCTION_2S          (HAL_XSPI_INSTRUCTION_2_LINES | HAL_XSPI_INSTRUCTION_DTR_DISABLE)

/**
  * @brief 2-line instruction, DTR mode (DTR enabled)
  */
#define INSTRUCTION_2D          (HAL_XSPI_INSTRUCTION_2_LINES | HAL_XSPI_INSTRUCTION_DTR_ENABLE)

/**
  * @brief 4-line instruction, SDR mode (DTR disabled)
  */
#define INSTRUCTION_4S          (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_DTR_DISABLE)

/**
  * @brief 4-line instruction, DTR mode (DTR enabled)
  */
#define INSTRUCTION_4D          (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_DTR_ENABLE)

/**
  * @brief 8-line instruction, SDR mode (DTR disabled)
  */
#define INSTRUCTION_8S          (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_DTR_DISABLE)

/**
  * @brief 8-line instruction, DTR mode (DTR enabled)
  */
#define INSTRUCTION_8D          (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_DTR_ENABLE)

/**
  * @brief Mask for all supported instruction line modes.
  */
#define INSTRUCTION_MASK        (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_4_LINES |\
                                 HAL_XSPI_INSTRUCTION_2_LINES | HAL_XSPI_INSTRUCTION_1_LINE)

/* Address defines */
/**
  * @brief 1-line address, SDR mode (DTR disabled)
  */
#define ADDRESS_1S              (HAL_XSPI_ADDRESS_1_LINE  | HAL_XSPI_ADDRESS_DTR_DISABLE)

/**
  * @brief 1-line address, DTR mode (DTR enabled)
  */

#define ADDRESS_1D              (HAL_XSPI_ADDRESS_1_LINE  | HAL_XSPI_ADDRESS_DTR_ENABLE)
/**
  * @brief 2-line address, SDR mode (DTR disabled)
  */

#define ADDRESS_2S              (HAL_XSPI_ADDRESS_2_LINES | HAL_XSPI_ADDRESS_DTR_DISABLE)
/**
  * @brief 2-line address, DTR mode (DTR enabled)
  */

#define ADDRESS_2D              (HAL_XSPI_ADDRESS_2_LINES | HAL_XSPI_ADDRESS_DTR_ENABLE)
/**
  * @brief 4-line address, SDR mode (DTR disabled)
  */

#define ADDRESS_4S              (HAL_XSPI_ADDRESS_4_LINES | HAL_XSPI_ADDRESS_DTR_DISABLE)
/**
  * @brief 4-line address, DTR mode (DTR enabled)
  */

#define ADDRESS_4D              (HAL_XSPI_ADDRESS_4_LINES | HAL_XSPI_ADDRESS_DTR_ENABLE)
/**
  * @brief 8-line address, SDR mode (DTR disabled)
  */

#define ADDRESS_8S              (HAL_XSPI_ADDRESS_8_LINES | HAL_XSPI_ADDRESS_DTR_DISABLE)
/**
  * @brief 8-line address, DTR mode (DTR enabled)
  */

#define ADDRESS_8D              (HAL_XSPI_ADDRESS_8_LINES | HAL_XSPI_ADDRESS_DTR_ENABLE)

/**
  * @brief Mask for all supported address line modes.
  */
#define ADDRESS_MASK            (HAL_XSPI_ADDRESS_8_LINES | HAL_XSPI_ADDRESS_4_LINES |\
                                 HAL_XSPI_ADDRESS_2_LINES | HAL_XSPI_ADDRESS_1_LINE)

/* Data defines */
/**
  * @brief 1-line data, SDR mode (DTR disabled)
  */
#define DATA_1S                 (HAL_XSPI_DATA_1_LINE  | HAL_XSPI_DATA_DTR_DISABLE)

/**
  * @brief 1-line data, DTR mode (DTR enabled)
  */
#define DATA_1D                 (HAL_XSPI_DATA_1_LINE  | HAL_XSPI_DATA_DTR_ENABLE)

/**
  * @brief 2-line data, SDR mode (DTR disabled)
  */
#define DATA_2S                 (HAL_XSPI_DATA_2_LINES | HAL_XSPI_DATA_DTR_DISABLE)

/**
  * @brief 2-line data, DTR mode (DTR enabled)
  */
#define DATA_2D                 (HAL_XSPI_DATA_2_LINES | HAL_XSPI_DATA_DTR_ENABLE)

/**
  * @brief 4-line data, SDR mode (DTR disabled)
  */
#define DATA_4S                 (HAL_XSPI_DATA_4_LINES | HAL_XSPI_DATA_DTR_DISABLE)

/**
  * @brief 4-line data, DTR mode (DTR enabled)
  */
#define DATA_4D                 (HAL_XSPI_DATA_4_LINES | HAL_XSPI_DATA_DTR_ENABLE)

/**
  * @brief 8-line data, SDR mode (DTR disabled)
  */
#define DATA_8S                 (HAL_XSPI_DATA_8_LINES | HAL_XSPI_DATA_DTR_DISABLE)

/**
  * @brief 8-line data, DTR mode (DTR enabled)
  */
#define DATA_8D                 (HAL_XSPI_DATA_8_LINES | HAL_XSPI_DATA_DTR_ENABLE)

#if defined (HAL_XSPI_DATA_16_LINES)
/**
  * @brief 16-line data, SDR mode (DTR disabled)
  */
#define DATA_16S                (HAL_XSPI_DATA_16_LINES | HAL_XSPI_DATA_DTR_DISABLE)

/**
  * @brief 16-line data, DTR mode (DTR enabled)
  */
#define DATA_16D                (HAL_XSPI_DATA_16_LINES | HAL_XSPI_DATA_DTR_ENABLE)
#endif /* HAL_XSPI_DATA_16_LINES */

/**
  * @brief Mask for all supported data line modes.
  */
#define DATA_MASK               (HAL_XSPI_DATA_8_LINES | HAL_XSPI_DATA_4_LINES |\
                                 HAL_XSPI_DATA_2_LINES | HAL_XSPI_DATA_1_LINE)

/**
  * @brief Bit mask indicating access mode type.
  */
#define ACCESS_MODE_TYPE        0x40000000U

/**
  * @brief Mask for all access mode configuration bits.
  */
#define ACCESS_MODE_MASK        (ACCESS_MODE_TYPE | \
                                 INSTRUCTION_MASK | HAL_XSPI_INSTRUCTION_DTR_ENABLE | \
                                 ADDRESS_MASK     | HAL_XSPI_ADDRESS_DTR_ENABLE | \
                                 DATA_MASK        | HAL_XSPI_DATA_DTR_ENABLE)

/* Definition of possible values for Types of supported memories */
/**
  * @brief Mask for flash memory type.
  */
#define FLASH_TYPE_MASK                          0x1000

/**
  * @brief Mask for PSRAM memory type.
  */
#define PSRAM_TYPE_MASK                          0x2000

/**
  * @brief Mask for HyberBus memory type.
  */
#define HYPERBUS_TYPE_MASK                       0x4000

/**
  * @brief Value for NOR Flash memory type.
  */
#define CUSTOM_NOR_FLASH                         0x0001

/**
  * @brief Value for PSRAM memory type.
  */
#define CUSTOM_PSRAM                             0x0002

/**
  * @brief Value for HyperRam memory type.
  */
#define CUSTOM_HYPERRAM                          0x0004

/**
  * @brief Bit mask indicating Address is present inside configuration step.
  */
#define CFGSTEP_ADDRESS                          0x80000000U

/**
  * @brief Bit mask indicating DataSize access during RW operations is present inside configuration step.
  */
#define CFGSTEP_DATASIZE_MASK                    0x60000000U

/**
  * @brief Value for 8-bit data in configuration step.
  */
#define CFGSTEP_DATASIZE_8BIT                    0x00000000U

/**
  * @brief Value for 16-bit data in configuration step.
  */
#define CFGSTEP_DATASIZE_16BIT                   0x20000000U

/**
  * @brief Value for 24-bit data in configuration step.
  */
#define CFGSTEP_DATASIZE_24BIT                   0x40000000U

/**
  * @brief Bit mask used for checking type of configuration step.
  */
#define CFGSTEP_CONFIG_TYPE_MASK                 0x0F000000U

/**
  * @brief Value for Instruction type of configuration step.
  */
#define CFGSTEP_INST_CONFIG                      0x08000000U

/**
  * @brief Value for Register access type of configuration step.
  */
#define CFGSTEP_REG_CONFIG                       0x04000000U

/**
  * @brief Value for Execute Optional config during configuration step.
  */
#define CFGSTEP_EXEC_OPT_CONFIG                  0x02000000U

/**
  * @brief Value for Execute hyperbus register config during configuration step.
  */
#define CUSTOM_CFGSTEP_HYPERBUS_CONFIG           0x01000000U

/**
  * @brief Value for Execute user callback type of configuration step.
  */
#define CFGSTEP_USER_CALLBACK_CONFIG             0x03000000U

/**
  * @brief Bit mask used for checking type of Instruction configuration step.
  */
#define CFGSTEP_INST_TYPE_MASK                   0x0000000FU

/**
  * @brief Value for basic instruction to be sent.
  */
#define CFGSTEP_INST_BASIC                       0x00000001U

/**
  * @brief Bit mask used for checking type of operation of Register Access configuration step.
  */
#define CFGSTEP_REG_ACCESS_TYPE_MASK             0x00000FF0U

/**
  * @brief Value for Read register operation type.
  */
#define CFGSTEP_REG_R_TYPE                       0x00000010U

/**
  * @brief Value for Write register operation type.
  */
#define CFGSTEP_REG_W_TYPE                       0x00000020U

/**
  * @brief Value for ReadAfterWrite register operation type.
  */
#define CFGSTEP_REG_RAW_TYPE                     0x00000040U

/* Definition of possible values for Configuration step types */
/**
  * @brief Configuration step to send a basic instruction to memory. Instruction is present inside configuration.
  */
#define CUSTOM_CFGSTEP_BASIC_INSTRUCTION         (CFGSTEP_INST_CONFIG | CFGSTEP_INST_BASIC)

/**
  * @brief Configuration step to execute a user callback.
  */
#define CUSTOM_CFGSTEP_USER_CALLBACK             (CFGSTEP_USER_CALLBACK_CONFIG)

/**
  * @brief Configuration step for Read then Write operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_RW_REGISTER               (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_R_TYPE | CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step for Read, Write then Read (for check) operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_RWR_REGISTER              (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_R_TYPE | CFGSTEP_REG_W_TYPE | CFGSTEP_REG_RAW_TYPE)

/**
  * @brief Configuration step for Read operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_R_REGISTER                (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_8BIT | CFGSTEP_REG_R_TYPE)

/**
  * @brief Configuration step for Read operations on register after a Write operation
  *        (for checking value) inside configuration.
  */
#define CUSTOM_CFGSTEP_RAW_REGISTER              (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_8BIT | CFGSTEP_REG_RAW_TYPE)

/**
  * @brief Configuration step for Write operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_W_REGISTER                (CFGSTEP_REG_CONFIG | CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step for Read 2 bytes operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_RR_REGISTER               (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_16BIT | CFGSTEP_REG_R_TYPE)

/**
  * @brief Configuration step for Write 2 bytes operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_WW_REGISTER               (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_16BIT | CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step for Read 3 bytes operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_RRR_REGISTER              (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_24BIT | CFGSTEP_REG_R_TYPE)

/**
  * @brief Configuration step for Write 3 bytes operations on register inside configuration.
  */
#define CUSTOM_CFGSTEP_WWW_REGISTER              (CFGSTEP_REG_CONFIG | CFGSTEP_DATASIZE_24BIT | CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step for HyperBUS for Read, Write then Read (for check) on register inside configuration.
  */
#define CUSTOM_CFGSTEP_HYPERBUS_RWR_REGISTER     (CUSTOM_CFGSTEP_HYPERBUS_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_R_TYPE | CFGSTEP_REG_W_TYPE | CFGSTEP_REG_RAW_TYPE)

/**
  * @brief Configuration step for HyperBUS for Read then Write on register inside configuration.
  */
#define CUSTOM_CFGSTEP_HYPERBUS_RW_REGISTER      (CUSTOM_CFGSTEP_HYPERBUS_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_R_TYPE | CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step for HyperBUS for Read on register inside configuration.
  */
#define CUSTOM_CFGSTEP_HYPERBUS_R_REGISTER       (CUSTOM_CFGSTEP_HYPERBUS_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_R_TYPE)

/**
  * @brief Configuration step for HyperBUS for Write on register inside configuration.
  */
#define CUSTOM_CFGSTEP_HYPERBUS_W_REGISTER       (CUSTOM_CFGSTEP_HYPERBUS_CONFIG | CFGSTEP_DATASIZE_8BIT | \
                                                  CFGSTEP_REG_W_TYPE)

/**
  * @brief Configuration step to execute Optional configuration (New Freq, AccessMode, etc.).
  */
#define CUSTOM_CFGSTEP_EXEC_OPT_CFG              (CFGSTEP_EXEC_OPT_CONFIG)

/**
  * @brief Definition of specific setting (DHQC, Sample Shift, ...) handling method.
  */
#define CUSTOM_SETTING_CFG_UNDEFINED             0

/**
  * @brief Value indicating setting is defined.
  */
#define CUSTOM_SETTING_CFG_DEFINED               (0x80000000U)

/**
  * @brief Mask for extracting setting value.
  */
#define CUSTOM_SETTING_CFG_MASK                  (0x7FFFFFFFU)

/**
  * @brief Value indicating to use Address 0x00 for reading Status Register.
  */
#define CUSTOM_SR_ACCESS_ADD_0                   (0x00000000U)

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_CUSTOM_PRIVATE_DEFINE_H */
