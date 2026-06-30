## <b>I3C_Sensor_Direct_Command_DMA Example Description</b>

How to handle a Direct Command procedure between STM32N6xx Nucleo and X-NUCLEO-IKS01A3,
using DMA.

Goal of this example is to demonstrate a simple communication with the sensor LSM6DSO present on X-NUCLEO-IKS01A3

      - Board: NUCLEO-N657X0-Q (embeds a STM32N657X0H3Q device)
      - SCL Pin: PH9 (Arduino SCL/D15 CN14 pin10)
      - SDA Pin: PC1 (Arduino SDA/D14 CN14 pin9)

      - Plug a X-NUCLEO-IKS01A3 (https://www.st.com/en/ecosystems/x-nucleo-iks01a3.html) expansion board
        on arduino connector


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The system clock runs at 600 MHz and external memory interface at the highest speed).

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit() function which core is implementing
the configuration of the needed I3C resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change I3C configuration.
To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

Note: To manage properly the data transfer, the value HAL_I3C_TXFIFO_THRESHOLD_4_4 in FIFO configuration
is not allowed when there is multiple transmit in the same descriptor.

The I3C communication is then initiated with a I3C Bus timing at 1Mhz as LSM6DSO start in I2C mode at Reset.

In a first communication step, the HAL_I3C_Ctrl_TransmitCCC_IT() function allow the Controller to transfer
the aBroadcast_CCC descriptor, which consist to disable the LSM6DSO interrupt then Reset the Dynamic address.

The user press the USER push-button on the Controller Board to start the communication ,
the HAL_I3C_Ctrl_TransmitCCC_IT() function allow the Controller to transfer
the aDirect_CCC descriptor, which consist to assign a Dynamic address assignment through the LSM6DSO Static address.

After a dynamic address has been assigned, the sensor is recognized as an I3C device,
the controller checks the presence of the sensor by sending the new dynamic address through the
HAL_I3C_Ctrl_IsDeviceI3C_Ready() function.

Then controller waiting the ACK of address.

At this step, the I3C_CtrlTimingComputation() and HAL_I3C_Ctrl_BusCharacteristicConfig() function allow the I3C
Controller to set bus speed to 5MHz as on IKS01A3 is the maximum allowed speed due to the level shifters.


Then controller waiting user action.

Then, in an infinite loop, the user press the USER push-button on the Controller Board to start the communication
by sending the all CCC element of the aCCCList through HAL_I3C_Ctrl_MultipleTransfer_DMA()
to I3C Targets which receive the Command and treat it by sending and receiving the associated data.

The end of each transfer are monitored through the HAL_I3C_GetState() function
result.

The Controller is informed at fully transfer of CCC element through HAL_I3C_CtrlMultipleXferCpltCallback().

At this step, Controller compute the data receive through aRxBuffer and print it through Terminal I/O
through DisplayCCCValue().

At next USER push-button press, the Controller repeat the last actions.
Each time is repeated, the value of SETMRL and SETMWL are increased by one unit and can be verify at display result.

NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:

 - LED1 toggles at each transmission/reception process is complete.
 - LED2 toggle slowly when there is an error in transmission/reception process.

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.

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
      
#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, DMA, Controller, Target, Dynamic Address Assignment, Direct Command,
Transmission, Reception, Multiple transfer.

### <b>Directory contents</b>

  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Inc/stm32n6xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Inc/stm32n6xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Inc/stm32n6xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Inc/main.h                    Header for main.c module
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Src/stm32n6xx_it.c            I3C interrupt handlers
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Src/main.c                    Main program
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Src/system_stm32n6xx.c        STM32N6xx system source file
  - I3C/I3C_Sensor_Direct_Command_DMA/FSBL/Src/stm32n6xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0H3Q devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up

    - Plug a X-NUCLEO-IKS01A3 (https://www.st.com/en/ecosystems/x-nucleo-iks01a3.html) expansion board
      on arduino connector.    

  - Launch the program in debug mode on Controller board side to benefit of Terminal I/O information on Controller side.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.
	
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.





**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
