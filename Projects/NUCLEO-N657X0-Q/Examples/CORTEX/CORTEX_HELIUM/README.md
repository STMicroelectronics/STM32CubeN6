## <b>CORTEX_HELIUM Example Description</b>

This project describes how to use the SIMD Helium instructions and the performance gained.

It is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

The system clock runs at 600 MHz and external memory interface at the highest speed.

This example shows how the use of SIMD Helium instructions can dramatically improve performance in calculations done on large data sets.
This basic example takes two 65536-byte arrays, adds its individual elements and stores them into a third array.
This operation is done twice : once using a regular loop compiled into standard thumb instructions and again using SIMD instructions.

The program displays in the terminal the amount of cycles elapsed for each method.

Depending of IDE, to watch content of Terminal I/O note that:

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In the Debug configuration, there are two possible options:
   
   - Option 1:
      - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
      - Window\Debugger, select the Semihosting: "mon gdb_breakpoint_override hard" and "monitor arm semihosting enable"
   - Option 2:
      - Window\Debugger, select the Debug probe : ST-LINK(ST-LINK GDB server)
      - Window\Startup,add the following commands: Enable terminal mode  

For CubeIDE, the logs can also be written to UART using 115200 baud, 8 data bits, 1 stop bits and no parity.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

CORTEX, MPU, CACHE, DMA, SRAM

### <b>Directory contents</b>

File | Description
 --- | ---
  CORTEX/CORTEX_HELIUM/FSBL/Inc/main.h                   | Header for main.c module
  CORTEX/CORTEX_HELIUM/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL Configuration file
  CORTEX/CORTEX_HELIUM/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
  CORTEX/CORTEX_HELIUM/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP Configuration file
  CORTEX/CORTEX_HELIUM/FSBL/Src/main.c                   | Main program
  CORTEX/CORTEX_HELIUM/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
  CORTEX/CORTEX_HELIUM/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
  CORTEX/CORTEX_HELIUM/FSBL/Src/system_stm32n6xx.c       | STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.



**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
