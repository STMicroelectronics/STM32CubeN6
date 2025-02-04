## <b>RAMCFG_ECC_Error_Generation Example Description</b>

How to configure and use the RAMCFG HAL API to manage ECC errors via RAMCFG peripheral.

This project is targeted to run on STM32N657X0HxQ devices on NUCLEO-N657X0-Q board from STMicroelectronics.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 600 MHz.

When the RAMCFG controller is disabled, the ECC calculation when writing BKPSRAM memory is not performed. So, when enabling
the RAMCFG after writing to BKPSRAM, the ECC and data don't match and the RAMCFG controller generates interrupts
according to ECC error type (single or double).
By default, interrupts are disabled. After enabling them (single error and double error interrupts), the
RAMCFG generates interrupts on each read access to a corrupted data.
The RAMCFG controller is able to detect and correct ECC single error data and only detect ECC double error data.

NUCLEO-N657X0-Q board's LED is used to monitor the project operation status:

 - LED1 toggles when no error detected.
 - LED2 is ON when any project error has occurred.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

RAMCFG, ECC, SRAM, Single error, Double error

### <b>Directory contents</b> 

File | Description
 --- | ---
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Src/main.c                  | Main program
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Src/system_stm32n6xx.c      | stm32n6xx system clock configuration file
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Src/stm32n6xx_it.c          | Interrupt handlers
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Src/stm32n6xx_hal_msp.c     | HAL MSP module
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Inc/main.h                  | Main program header file
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Inc/stm32n6xx_nucleo_conf.h | BSP Configuration file
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Inc/stm32n6xx_hal_conf.h    | HAL Configuration file
RAMCFG/RAMCFG_ECC_Error_Generation/FSBL/Inc/stm32n6xx_it.h          | Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q 
    board and can be easily tailored to any other supported device
    and development board.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out. 

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
 


