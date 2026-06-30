## <b>RIF_Peripheral Example Description</b>

This example describes how to use the RIF HAL API to configure the RISC in order to protect a peripheral from illegal accesses.
The FSBL software sets the SPI1 RISC configuration for Secure accesses only then enables the illegal access controller interrupt for the SPI1 peripheral before switching to the non secure context.
The non secure then tries to write to the configuration registers of SPI1, triggering an illegal access interrupt in the secure context.

After startup, the HAL_Init() function is called to reset all the peripherals and initialize the systick used as 1ms HAL timebase. The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 400 Mhz and the CPU clock at 800 MHz.

The Example project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may integrate in his application in order to enhance the performance.

STM32 board LEDs are used to monitor the example status:

  - Green LED (LED3) is ON when the example was executed correctly.
  - Blue  LED (LED1) is ON when there was an an error in initialization or if the RISC has not reported the illegal access.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 3. By default, the VDDIO3_HSLV bit in OTP124 is not set. The external memory therefore runs slower than optimal.
     To take advantage of the full performance of the external flash, this bit should be set.

### <b>Keywords</b>

Resource isolation framework, RIF, Resource Isolation slave unit for peripherals, RIFSC, RISC, Security

### <b>Directory contents</b>

File | Description
 --- | ---
      - RIF/RIF_Peripheral/FSBL/Inc/stm32n6xx_nucleo_conf.h               | Secure BSP configuration file
      - RIF/RIF_Peripheral/FSBL/Inc/stm32n6xx_hal_conf.h                  | Secure HAL configuration file
      - RIF/RIF_Peripheral/FSBL/Inc/stm32n6xx_it.h                        | Secure Interrupt handlers header file
      - RIF/RIF_Peripheral/FSBL/Inc/main.h                                | Secure Header for main.c module
      - RIF/RIF_Peripheral/FSBL/Src/stm32n6xx_it.c                        | Secure Interrupt handlers
      - RIF/RIF_Peripheral/FSBL/Src/stm32n6xx_hal_msp.c                   | Secure HAL MSP file
      - RIF/RIF_Peripheral/FSBL/Src/main.c                                | Secure Main program
      - RIF/RIF_Peripheral/FSBL/Src/system_stm32n6xx_fsbl.c               | Secure STM32N6xx system source file
      - RIF/RIF_Peripheral/AppliNonSecure/Inc/stm32n6xx_hal_conf.h        | Non Secure HAL configuration file
      - RIF/RIF_Peripheral/AppliNonSecure/Inc/stm32n6xx_it.h              | Non Secure Interrupt handlers header file
      - RIF/RIF_Peripheral/AppliNonSecure/Inc/main.h                      | Non Secure Header for main.c module
      - RIF/RIF_Peripheral/AppliNonSecure/Src/stm32n6xx_it.c              | Non Secure Interrupt handlers
      - RIF/RIF_Peripheral/AppliNonSecure/Src/stm32n6xx_hal_msp.c         | Non Secure HAL MSP file
      - RIF/RIF_Peripheral/AppliNonSecure/Src/main.c                      | Non Secure Main program
      - RIF/RIF_Peripheral/AppliNonSecure/Src/system_stm32n6xx_ns.c       | Non Secure STM32N6xx system source file
      - RIF/RIF_Peripheral/Secure_nsclib/secure_nsc.h                     | Non Secure Callable library

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6 devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it?</b>

To run this example in development mode :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files (both FSBL and non-secure application)

**EWARM**, **MDK-ARM**:

 - launch the debug with the FSBL configuration. The non secure application is loaded into RAM automatically.


 This program can be run only with boot mode set in development mode.




**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
