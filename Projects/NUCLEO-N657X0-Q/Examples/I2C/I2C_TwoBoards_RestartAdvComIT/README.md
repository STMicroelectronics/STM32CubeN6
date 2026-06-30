
## <b>I2C_TwoBoards_RestartAdvComIT Example Description</b>

How to perform multiple I2C data buffer transmission/reception between two boards, 
in interrupt mode and with restart condition.

       Board: NUCLEO-N657X0-Q (embeds a STM32N657X0H3Q device)
       - SCL Pin: PH9 (Arduino D15 CN14 pin 10, Morpho CN15 pin 3)
       - SDA Pin: PC1 (Arduino D14 CN14 pin 9, Morpho CN15 pin 5)

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

The system clock runs at 600 MHz and external memory interface at the highest speed.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2C configuration.

The USER push-button is used to initiate a communication between Master device to Slave.
User can initiate a new communication after each previous transfer completed.

The I2C communication is then initiated.
The project is split in two parts the Master Board and the Slave Board :

 - Master Board :
   The HAL_I2C_Master_Sequential_Transmit_IT() and the HAL_I2C_Master_Sequential_Receive_IT() functions 
   allow respectively the transmission and the reception of a predefined data buffer
   in Master mode.

 - Slave Board :
   The HAL_I2C_EnableListen_IT(), HAL_I2C_Slave_Sequential_Receive_IT() and the HAL_I2C_Slave_Sequential_Transmit_IT() functions 
   allow respectively the "Listen" the I2C bus for address match code event, reception and the transmission of a predefined data buffer
   in Slave mode.

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file.
If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
If the Slave board is used the "#define MASTER_BOARD" must be commented.

Example execution:

On Master board side:

 - Wait USER push-button to be pressed.

This action initiate a write request by Master through HAL_I2C_Master_Sequential_Transmit_IT() or a write then read request
through HAL_I2C_Master_Sequential_Transmit_IT() then HAL_I2C_Master_Sequential_Receive_IT() routine depends on Command Code type.
Initially at power on Slave device through Interrupt "Listen" the I2C bus to perform an acknowledge of Match Address when necessary.
This "Listen" action is initiated by calling HAL_I2C_EnableListen_IT().

Command code type is decomposed in two categories :

 1. Action Command code

    a. Type of command which need an internal action from Slave Device without sending any specific answer to Master.
    b. I2C sequence is composed like that :
     _____________________________________________________________________________________
    |_START_|_Slave_Address_|_Wr_|_A_|_Command_Code_BYTE_1_|_A_|_Command_Code_BYTE_2_|_A_|....
     ________________________________
    |_Command_Code_BYTE_M_|_A_|_STOP_|

First of all, through HAL_I2C_Master_Sequential_Transmit_IT() routine, Master device generate an I2C start condition
with the Slave address and a write bit condition.
In Slave side, when address Slave match code is received on I2C1, an event interrupt (ADDR) occurs.
I2C1 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Write (Transmit)
to call the correct HAL_I2C_Slave_Sequential_Receive_IT() function.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the Command code bytes through Interrupt.
The Command code data is received and treated byte per byte through HAL_I2C_SlaveRxCpltCallback() in Slave side until a STOP condition.

And so in Master side, each time the Slave acknowledge the byte received,
Master transfer the next data from flash memory buffer to I2C1 TXDR register until "Action Command code" Transfer completed.
Master auto-generate a Stop condition when transfer is achieved.

The STOP condition generate a STOP interrupt and initiate the end of reception on Slave side.
Thanks to HAL_I2C_ListenCpltCallback(), Slave is informed of the end of Communication with Master
and "Listen" mode is also terminated.

NUCLEO-N657X0-Q board's LEDs can be used to monitor the transfer status :

Slave board side only :

 - LED1 is turned ON when the reception process is completed.

Master board side only :

 - LED1 is turned ON when the transmission process is completed.
 
Both sides :

 - LED2 is slowly blinking (1 sec. period) when there is an error in communication process.(communication is stopped if any, using infinite loop)

These LEDs status are keeped at same value during 1 Second and then clear, this will allow to monitor a next transfer status.
 
Also only on Master board side, Terminal I/O can be used to watch the Action Command Code sent by Master and associated Slave action with IDE in debug mode.

Depending of IDE, to watch content of Terminal I/O note that :

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code could not be displayed on debugger but user can use the Virtual Com port of the NUCLEO-N657X0-Q
   to display Command Code on PC side using an hyperterminal. See below, information related to this possibility.

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.

   In the Debug configuration, there are two possible options:
   
   - Option 1:
      - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
      - Window\Debugger, select the Semihosting: "mon gdb_breakpoint_override hard" and "monitor arm semihosting enable"
   - Option 2:
      - Window\Debugger, select the Debug probe : ST-LINK(ST-LINK GDB server)
      - Window\Startup,add the following commands: Enable terminal mode   
	  
Other proposal to retrieve display of Command Code for all IDE is to use the Virtual Com.

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration :
   - remove the "__DBG_ITM" from MDK-ARM Settings --> C/C++ (AC6) --> Preprocessor Symbols --> Define
   - uncheck "Trace Enable" from MDK-ARM Settings --> Debug --> Settings --> Trace
   - exclude "Retarget.c" from MDK-ARM Project

 - When resorting to STM32CubeIDE (Debug and Release) :
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration :
   - uncheck "exclude resource from build" for "syscall.c" file
   - remove "-specs=rdimon.specs -lc -lrdimon" from from cubeIDE Properties --> C/C++ Build --> Settings
   --> Tool Settings --> MCU GCC linker --> Miscellaneous

In order to select use of Virtual Com port feature of STLINK for connection between NUCLEO-N657X0-Q and PC,
User has to set USE_VCP_CONNECTION define to 1 in main.h file.
If so, please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled 
on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

 2. Request Command code :

    a. Type of command which need a specific data answer from Slave Device.
    b. I2C sequence is composed like that :
     _____________________________________________________________________________________
    |_START_|_Slave_Address_|_Wr_|_A_|_Command_Code_BYTE_1_|_A_|_Command_Code_BYTE_2_|_A_|....
     ______________________________________________________________________________
    |_Command_Code_BYTE_M_|_A_|_RESTART_|_Slave_Address_|_Rd_|_A_|_Data_BYTE_1_|_A_|...
     ___________________________________________
    |_Data_BYTE_2_|_A_|_Data_BYTE_N_|_NA_|_STOP_|

First of all, through HAL_I2C_Master_Sequential_Transmit_IT() routine, Master device generate an I2C start condition
with the Slave address and a write bit condition.
In Slave side, when address Slave match code is received on I2C1, an event interrupt (ADDR) occurs.
I2C1 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Write (Transmit)
to call the correct HAL_I2C_Slave_Sequential_Receive_IT() function.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the Command code bytes through Interrupt.
The Command code data is received and treated byte per byte through HAL_I2C_SlaveRxCpltCallback() in Slave side.
If data received match with a Internal Command Code, set the associated index, which will use for Transmission process when requested by Master

And so in Master side, each time the Slave acknowledge the byte received,
Master transfer the next data from flash memory buffer to I2C1 TXDR register until "Request Command code" transfer completed.

Then through HAL_I2C_Master_Sequential_Receive_IT() routine, Master device generate a RESTART condition
with Slave address and a read bit condition.
In Slave side, when address Slave match code is received on I2C1, an event interrupt (ADDR) occurs.
I2C1 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Read (Reception)
to call the correct HAL_I2C_Slave_Sequential_Transmit_IT() function.
Slave enter in transmitter mode and send send answer bytes through interrupt until end of transfer size.
Master auto-generate a NACK and STOP condition to inform the Slave that the transfer and communication are finished.

The STOP condition generate a STOP interrupt and initiate the end of reception on Slave side.
Thanks to HAL_I2C_ListenCpltCallback(), Slave is informed of the end of Communication with Master
and "Listen" mode is also terminated.

NUCLEO-N657X0-Q board's LEDs can be used to monitor the transfer status in both sides :

Slave board side :

 - LED1 is turned ON when the reception process is completed.
 - LED1 is turned OFF when the transmission process is completed.

Master board side :

 - LED1 is turned ON when the transmission process is completed.
 - LED1 is turned OFF when the reception process is completed.
 
Both sides :

 - LED2 is slowly blinking (1 sec. period) when there is an error in communication process.(communication is stopped if any, using infinite loop)

These LEDs status are keeped at same value during 1 Second and then clear, this will allow to monitor a next transfer status.

Also only on Master board side, Terminal I/O can be used to watch the Request Command Code sent by Master and associated Slave answer with IDE in debug mode.

Depending of IDE, to watch content of Terminal I/O note that :

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code could not be displayed on debugger but user can use the Virtual Com port of the NUCLEO-N657X0-Q
   to display Command Code on PC side using an hyperterminal. See below, information related to this possibility.
 
 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.

   In the Debug configuration, there are two possible options:
   
   - Option 1:
      - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
      - Window\Debugger, select the Semihosting: "mon gdb_breakpoint_override hard" and "monitor arm semihosting enable"
   - Option 2:
      - Window\Debugger, select the Debug probe : ST-LINK(ST-LINK GDB server)
      - Window\Startup,add the following commands: Enable terminal mode   
	  
Other proposal to retrieve display of Command Code for all IDE is to use the Virtual Com.

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration :
   - remove the "__DBG_ITM" from MDK-ARM Settings --> C/C++ (AC6) --> Preprocessor Symbols --> Define
   - uncheck "Trace Enable" from MDK-ARM Settings --> Debug --> Settings --> Trace
   - exclude "Retarget.c" from MDK-ARM Project

 - When resorting to STM32CubeIDE (Debug and Release) :
   Command Code is displayed on PC (as HyperTerminal or TeraTerm) with proper configuration :
   - uncheck "exclude resource from build" for "syscall.c" file
   - remove "-specs=rdimon.specs -lc -lrdimon" from from cubeIDE Properties --> C/C++ Build --> Settings
   --> Tool Settings --> MCU GCC linker --> Miscellaneous
   
In order to select use of Virtual Com port feature of STLINK for connection between NUCLEO-N657X0-Q and PC,
User has to set USE_VCP_CONNECTION define to 1 in main.h file.
If so, please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled 
on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, I2C, Communication, Transmission, Reception, SCL, SDA, Interrupt, Request, Sequential

### <b>Directory contents</b> 

  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Inc/stm32n6xx_it.h              I2C interrupt handlers header file
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Inc/main.h                      Header for main.c module  
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Src/stm32n6xx_it.c              I2C interrupt handlers
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Src/main.c                      Main program
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Src/system_stm32n6xx.c          STM32N6xx system source file
  - I2C/I2C_TwoBoards_RestartAdvComIT/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file    

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6XX devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.    

  - NUCLEO-N657X0-Q Set-up

    - Connect I2C_SCL line of Master board (PH9, Arduino D15 CN14 pin 10, Morpho CN15 pin 3) to I2C_SCL line of Slave Board (PH9, Arduino D15 CN14 pin 10, Morpho CN15 pin 3).
    - Connect I2C_SDA line of Master board (PC1, Arduino D14 CN14 pin 9, Morpho CN15 pin 5) to I2C_SDA line of Slave Board (PC1, Arduino D14 CN14 pin 9, Morpho CN15 pin 5).
    - Connect GND of Master board to GND of Slave Board.

  - Launch the program in debug mode on Master board side, and in normal mode on Slave side
    to benefit of Terminal I/O information on Master side.

    Or

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control) connected with the com port number of STLINK.
    Then Launch the program on both side.

  - Press USER push-button to initiate a communication by Master device to Slave device.
    User can initiate a new communication after each previous transfer completed.

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
    - Comment "#define MASTER_BOARD" and load the project in Slave Board
    - Uncomment "#define MASTER_BOARD" and load the project in Master Board
 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.






**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.


