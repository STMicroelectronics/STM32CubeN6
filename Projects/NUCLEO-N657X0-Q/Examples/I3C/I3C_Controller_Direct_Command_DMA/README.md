## <b>I3C_Controller_Direct_Command_DMA Example Description</b>

How to handle a Direct Command procedure between an I3C Controller and an I3C Target,
using DMA.

      - Board: NUCLEO-N657X0-Q (embeds a STM32N657X0H3Q device)
      - SCL Pin: PH9 (Arduino SCL/D15 CN14 pin10)
      - SDA Pin: PC1 (Arduino SDA/D14 CN14 pin9)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The system clock runs at 600 MHz and external memory interface at the highest speed).

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit()function which core is implementing
the configuration of the needed I3C resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change I3C configuration.
To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_Direct_Command_DMA) and the Target Board (I3C_Target_Direct_Command_DMA)

- Controller Board
    The HAL_I3C_Ctrl_DynAddrAssign_IT() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 150MHz.

  The HAL_I3C_SetConfigFifo(), the HAL_I3C_Ctrl_ReceiveCCC_DMA() and the HAL_I3C_Ctrl_TransmitCCC_DMA() functions
  allow respectively the configuration of the internal hardware FIFOs, the reception of associated Command value
  and the transmission of associated Command value using DMA mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 150MHz.

For this example the aSET_CCCList and aGET_CCCList are predefined related to Common Command Code descriptor treated in this example.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through HAL_I3C_Ctrl_DynAddrAssign_IT().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

Then controller waiting user action.

The user press the USER push-button on the Controller Board to start the communication by sending the first
then all Get CCC element of the aGET_CCCList through HAL_I3C_Ctrl_ReceiveCCC_DMA()
to I3C Targets which receive the Command and treat it by sending the associated data.

At the end, Controller is informed at fully reception of Get CCC element through HAL_I3C_CtrlRxCpltCallback().

At this step, Controller compute the data receive through aRxBuffer and print it through Terminal I/O through DisplayCCCValue().

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
	  
At next USER push-button press, the Controller switch to sending the first then all Set CCC element of the aSet_CCCList
through HAL_I3C_Ctrl_TransmitCCC_DMA() to I3C Target which receive the Command and treat it by receiving the associated data.
At the end, Controller is informed at fully transmission of Set CCC element through HAL_I3C_CtrlTxCpltCallback().

NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:
 - LED1 is toggle at ENTDAA reception and each time the Command Code process is completed.
 - LED2 is toggle slowly when there is an error in Command Code process.

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
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Inc/stm32n6xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Inc/stm32n6xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Inc/stm32n6xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Src/stm32n6xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Src/main.c                    Main program
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Src/system_stm32n6xx.c        STM32N6xx system source file
  - I3C/I3C_Controller_Direct_Command_DMA/FSBL/Src/stm32n6xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0H3Q devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PH9 Arduino SCL/D15 CN14 pin10) to I3C_SCL line of Target Board (PH9 Arduino SCL/D15 CN14 pin10).
    - Connect I3C_SDA line of Controller board (PC1 Arduino SDA/D14 CN14 pin9) to I3C_SDA line of Target Board (PC1 Arduino SDA/D14 CN14 pin9).
    - Connect GND of Controller board to GND of Target Board.

  - Launch the program in debug mode on Controller board side, and in normal mode on Target side
    to benefit of Terminal I/O information on Controller side.
	
  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Direct_Command_DMA). Code can be executed in this mode for debugging purposes.
 - Then rebuild all files and load your image into Target memory (I3C_Target_Direct_Command_DMA).
  - Run the Controller in debug mode before run the Target in normal mode.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
 
 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
