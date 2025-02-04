## <b>I2C_TwoBoards_ComPolling Example Description</b>

How to handle I2C data buffer transmission/reception between two boards, 
in polling mode.

    Board: NUCLEO-N657X0-Q (embeds a STM32N657X0H3Q device)
    SCL Pin: PE5 (CN15, pin4)
    SDA Pin: PE6 (CN15, pin2)
    
At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

The system clock runs at 600 MHz and external memory interface at the highest speed.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO). You may update this function to change I2C configuration.

The I2C communication is then initiated.

The project is split in two parts: the Master Board and the Slave Board.

- Master Board: 
  The HAL_I2C_Master_Receive() and the HAL_I2C_Master_Transmit() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Master mode using polling.

- Slave Board: 
  The HAL_I2C_Slave_Receive() and the HAL_I2C_Slave_Transmit() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Slave mode using polling.

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:

- If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
- If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Master Board,
I2C Master starts the communication by sending aTxBuffer through HAL_I2C_Master_Transmit()
to I2C Slave which receives aRxBuffer through HAL_I2C_Slave_Receive(). 

The second step starts when the user press the USER push-button on the Master Board,
the I2C Slave sends aTxBuffer through HAL_I2C_Slave_Transmit()
to the I2C Master which receives aRxBuffer through HAL_I2C_Master_Receive().

The end of this two steps are monitored through the HAL_I2C_GetState() function
result.

Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:

 - LED1 is ON when the transmission process is complete.
 - LED1 is OFF when the reception process is complete.
 - LED2 is ON when there is an error in transmission/reception process.  

#### <b>Notes</b>

 1. Timeout is set to 10 Seconds which means that if no communication occurs 
    during 10 Seconds, a Timeout Error will be generated.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I2C, Polling, Master, Slave, Transmission, Reception, Fast mode plus

### <b>Directory contents</b> 

  - I2C/I2C_TwoBoards_ComPolling/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Inc/main.h                      Header for main.c module  
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Src/stm32n6xx_it.c              Interrupt handlers
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Src/main.c                      Main program
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Src/system_stm32n6xx.c          STM32N6xx system source file
  - I2C/I2C_TwoBoards_ComPolling/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file    

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6XX devices.
    
  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.    

  - NUCLEO-N657X0-Q Set-up

    - Connect I2C_SCL line of Master board (PE5, CN15, pin4) to I2C_SCL line of Slave Board (PE5, CN15, pin4).
    - Connect I2C_SDA line of Master board (PE6, CN15, pin2) to I2C_SDA line of Slave Board (PE6, CN15, pin2).
    - Connect GND of Master board to GND of Slave Board.

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

