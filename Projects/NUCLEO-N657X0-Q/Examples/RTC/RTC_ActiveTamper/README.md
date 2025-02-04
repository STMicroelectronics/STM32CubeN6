## <b>RTC_ActiveTamper Example Description</b>

Configuration of the active tamper detection with backup registers erase.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The RTC peripheral configuration is ensured by the MX_RTC_Init() and MX_TAMP_RTC_Init functions.

HAL_RTC_MspInit()function which core is implementing the configuration of the needed RTC resources
according to the used hardware (CLOCK,PWR, RTC clock source and BackUp). 

You may update this function to change RTC configuration.

**Note** LSE oscillator clock is used as RTC clock source (32.768 kHz) by default.

This example performs the following:

1. Please connect the following pins together :
  - TAMP_IN1 / PC13 (pin 23 connector CN3)
  - TAMP_OUT3 / PH4 (pin 26 connector CN3)
  - (Optional) Oscilloscope probe to visualize the signal

2. Run the software

3. It configures the Active Tamper Input associated to an Output and enables the interrupt.

4. It writes  data to the RTC Backup registers, then check if the data are correctly written.

5. It updates the seed (optional).

6. Please disconnect the pins.

7. The RTC backup register are reset and the Tamper interrupt is generated.

   The firmware then checks if the RTC Backup register are cleared.
   
8. LED1 turns ON, Test is OK.
   LED1 blinks, Test is KO.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, RTC, Tamper, Reset, LSE, Backup, Active

### <b>Directory contents</b>

  - RTC/RTC_ActiveTamper/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - RTC/RTC_ActiveTamper/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - RTC/RTC_ActiveTamper/FSBL/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - RTC/RTC_ActiveTamper/FSBL/Inc/main.h                      Header for main.c module
  - RTC/RTC_ActiveTamper/FSBL/Src/stm32n6xx_it.c              Interrupt handlers
  - RTC/RTC_ActiveTamper/FSBL/Src/main.c                      Main program
  - RTC/RTC_ActiveTamper/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP module
  - RTC/RTC_ActiveTamper/FSBL/Src/system_stm32n6xx.c          STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

