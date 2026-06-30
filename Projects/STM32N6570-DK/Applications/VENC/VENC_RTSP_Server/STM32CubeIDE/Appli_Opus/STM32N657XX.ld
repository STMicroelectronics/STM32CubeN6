/*
******************************************************************************
**
** @file        : STM32N657XX_LRUN.ld
**
** @author      : GPM Application Team
**
** @brief       : Linker script for STM32N657XX Device from STM32N6 series
**                      2048 KBytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
******************************************************************************
** @attention
**
** Copyright (c) 2023 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
******************************************************************************
 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(DTCM) + LENGTH(DTCM); /* end of "RAM" Ram type memory */
_sstack = _estack - _Min_Stack_Size;

_Min_Heap_Size  = 0x25800;   /* 150K */
_Min_Stack_Size = LENGTH(DTCM); /* required amount of stack */

/* Stack and heap sizes */
/* export DTCM bounds for initialization */
_sDTCM = ORIGIN(DTCM);
_eDTCM = ORIGIN(DTCM) + LENGTH(DTCM);

/* Export regions noudaries for MPU config*/
__ro_region_start__ 	=  ORIGIN(RO_Region);
__ro_region_end__ 		=  ORIGIN(RO_Region) + LENGTH(RO_Region);

__rw_region_start__ 	=  ORIGIN(RW_Region);
__rw_region_end__  		=  ORIGIN(RW_Region) + LENGTH(RW_Region);

__nocache_region_start__ =  ORIGIN(NOCACHE_Region);
__nocache_region_end__ 	 =  ORIGIN(NOCACHE_Region) + LENGTH(NOCACHE_Region);

__psram_region_start__ 	=  ORIGIN(PSRAM);
__psram_region_end__ 	=  ORIGIN(PSRAM) + LENGTH(PSRAM);
 

/* Memory regions */
MEMORY
{
  ITCM     (rx)  		: ORIGIN = 0x10000000, LENGTH = 128K
  DTCM     (rw)  		: ORIGIN = 0x30000000, LENGTH = 128K
  RO_Region (rx) 		: ORIGIN = 0x34000400, LENGTH = 500K
	RW_Region (rw) 		: ORIGIN = 0x3407D400, LENGTH = 695K - 64
	NOCACHE_Region (rw) 	: ORIGIN = 0x3412AFC0, LENGTH = 2644K + 64
  PSRAM    (rw)  		: ORIGIN = 0x90000000, LENGTH = 16M
}

/* Sections */
SECTIONS
{
  /* Interrupt vector table */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >RO_Region

  /* The program code and other data into "RAM" Ram type memory */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >RO_Region

  /* Constant data into "RAM" Ram type memory */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >RO_Region

  .ARM.extab (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >RO_Region

  .ARM (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >RO_Region

  .preinit_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >RO_Region

  .init_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >RO_Region

  .fini_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >RO_Region

  .gnu.sgstubs :
  {
    . = ALIGN(4);
    *(.gnu.sgstubs*)   /* Secure Gateway stubs */
    . = ALIGN(4);
  } >RO_Region


  
  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */

  } >RW_Region AT> RO_Region

  /* Non-cacheable section */
  .noncacheable (NOLOAD) :
  {
    . = ALIGN(8);
    __snoncacheable = .;/* create symbol for start of section */
    KEEP(*(.noncacheable))
    . = ALIGN(8);
    __enoncacheable = .;  /* create symbol for start of section */
  } >NOCACHE_Region

  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RW_Region

  ._threadx_heap :
  {
    . = ALIGN(8);
    __RAM_segment_used_end__ = .;
    . = . + 32K;
    . = ALIGN(8);
  } >RW_Region AT> RW_Region

  /* Heap in RW_Region */
  ._user_heap (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = ALIGN(8);
  } >RW_Region

  ._user_stack :
  {
    . = ALIGN(8);
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >DTCM

  /* PSRAM section */
  .psram_section (NOLOAD):
  {
    . = ALIGN(32);
    *(.psram_bss)
    . = ALIGN(32);
  } >PSRAM

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
