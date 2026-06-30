# Middleware STM32 External Memory Manager

![latest tag](https://img.shields.io/github/v/tag/STMicroelectronics/stm32_mw_extmem.svg?color=brightgreen)

## Overview

**STM32Cube** is an STMicroelectronics original initiative to ease developers' life by reducing efforts, time and cost.

**STM32Cube** covers the overall STM32 products portfolio. It includes a comprehensive embedded software platform delivered for each STM32 series.
   * The CMSIS modules (core and device) corresponding to the ARM(tm) core implemented in this STM32 product.
   * The STM32 HAL-LL drivers, an abstraction layer offering a set of APIs ensuring maximized portability across the STM32 portfolio.
   * The BSP drivers of each evaluation, demonstration or nucleo board provided for this STM32 series.
   * A consistent set of middleware libraries such as RTOS, USB, FatFS, graphics, touch sensing library...
   * A full set of software projects (basic examples, applications, and demonstrations) for each board provided for this STM32 series.

Two models of publication are proposed for the STM32Cube embedded software:
   * The monolithic **MCU Package**: all STM32Cube software modules of one STM32 series are present (Drivers, Middleware, Projects, Utilities) in the repository (usual name **STM32Cubexx**, xx corresponding to the STM32 series).
   * The **MCU component**: each STM32Cube software module being part of the STM32Cube MCU Package, is delivered as an individual repository, allowing the user to select and get only the required software functions.

## Description

This **stm32_ExtMem_Manager** middleware component repository is one element **common to** STM32Cube MCU embedded software packages, providing the **External Memory Manager** middleware  part.

It represents ST offer to ensure the support of different External Memories (Serial NOR flash, PsRam, SD Card) on STM32 MCUs. It consists of several different drivers :
   * NOR_FLASH driver : this driver is dedicated to Nor Flash memories, and allows to initializes and configure memory access, based on information stored in SFDP tables of the memory
   * PSRAM driver : this driver is dedicated to PSRAM memories. Memory access is performed in Memory Map mode only.
   * SDCARD driver : this driver is dedicated to SD Card memories.
   * CUSTOM Driver : this driver is designed to allow memory configuration and content access, based on a configuration file (information used for memory configuration and access is stored in the configuration, and does not rely on what is present in memory SFDP tables for instance). This driver could be used to drive Nor flash, PsRam, EEPROM memories.
   * USER Driver : this driver is left for user implementation. Only APIs are provided, with empty function bodies (user implementation required)

Below table provides a summary of available **External Memory Manager** APIs per driver :

| API                      | NOR FLASH driver | PSRAM Driver |SDCARD driver | CUSTOM driver | USER Driver |
|:-------------------------|:----------------:|:------------:|:------------:|:-------------:|:-----------:|
| EXTMEM_Init              | Yes              | Yes          |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_DeInit            | Yes              | Yes          |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_Read              | Yes              | -            |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_Write             | Yes              | -            |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_WriteInMappedMode | Yes              | -            |-             | Yes           | -                         |
| EXTMEM_EraseSector       | Yes              | -            |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_EraseAll          | Yes              | -            |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_GetInfo           | Yes              | -            |Yes           | Yes           | Yes (User Implementation) |
| EXTMEM_MemoryMappedMode  | Yes              | Yes          |-             | Yes           | Yes (User Implementation) |
| EXTMEM_GetMapAddress     | Yes              | Yes          |-             | Yes           | Yes (User Implementation) |


## Release note

Details about the content of this release are available in the release note.

## Troubleshooting

Please refer to the [CONTRIBUTING.md](CONTRIBUTING.md) guide.