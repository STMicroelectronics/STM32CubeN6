## <b>I2C_Sensor_Private_Command_IT Example Description</b>

How to handle I2C data buffer transmission/reception between STM32N6xx Nucleo and X-NUCLEO-IKS4A1,
using an interrupt.

Goal of this example is to demonstrate a simple communication with the sensor LSM6DSV16X present on X-NUCLEO-IKS4A1.

       Board: NUCLEO-N657X0-Q (embeds a STM32N6XX device)
       - SCL Pin: PH9 ( Arduino CN14 pin1 )
       - SDA Pin: PC1 ( Arduino CN14 pin2 )
       - Plug a X-NUCLEO-IKS4A1 (https://www.st.com/en/ecosystems/x-nucleo-iks4a1.html) expansion board
         on arduino connector


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

Then the SystemClock_Config() function is used to configure the
 CPU clock to run at 600 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.

This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change I2C configuration.

In a first communication step, the I2C master initiate the communication by writing in CTRL1_XL_Register address the
aLSM6DSV16X_Config through HAL_I2C_Mem_Write_IT() function to config the LSM6DSV16X.

The end of this step is monitored through the HAL_I3C_GetState() function result.

Then master waiting user action.

The user press the USER push-button on the master Board.

Then, in an infinite loop, the I2C master intiate sensor communication to retrieve the LSM6DSV16X Temperature,
Gyroscope and Accelerometer data.

The HAL_I2C_Mem_Read_IT() function allow the master to send register address and receive an amount of data
from the LSM6DSV16X Temperature, Gyroscope and Accelerometer.

The end of this step are monitored through the HAL_I3C_GetState() function result.

At this step, Controller compute the data receive through aRxBuffer and
print it through Terminal I/O through DisplayValue() at each display refresh time.


NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:

 - LED1 is ON when the transmission process is complete.
 - LED1 is OFF when the reception process is complete.
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

Connectivity, Communication, I2C, Interrupt, Master, Slave, Transmission, Reception, Fast mode plus, Sensor,
LSM6DSV16X

### <b>Directory contents</b>

  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Inc/stm32n6xx_it.h              I2C interrupt handlers header file
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Inc/main.h                      Header for main.c module
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Src/stm32n6xx_it.c              I2C interrupt handlers
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Src/main.c                      Main program
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Src/system_stm32n6xx.c          STM32N6xx system source file
  - I2C/I2C_Sensor_Private_Command_IT/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6XX devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up


  - Plug a X-NUCLEO-IKS4A1 (https://www.st.com/en/ecosystems/x-nucleo-iks4a1.html) expansion board
      on arduino connector.

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
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.



**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
