## <b>Template Isolation LRUN (Load&Run) Description</b>

This project provides a reference TrustZone Isolation LRUN template based on the STM32Cube HAL API that can be used to build any firmware application made of a secure binary and of a non-secure binary to execute in internal SRAM.

#### <b>Description</b>

This project is composed of three sub-projects:

- one for the First Stage BootLoader (FSBL)
- one for the secure application part (Project_s)
- one for the non-secure application part (Project_ns).

Please remember that on system with security enabled, the system always boots in secure and the secure application is responsible for launching the non-secure application.


- FLEXRAM and SRAM1 are defined as fully secure
-       FLEXRAM (0x3400'0000 to 0x3406'3FFF) is defined as fully secure ROM area
-       SRAM1 (0x3406'4000 to 0x340F'FFFF) is defined as fully secure RAM area

- SRAM2 is defined as fully non-secure
-       (0x2410'0000 to 0x2417'FFFF) is defined as non-secure ROM area
-       (0x2418'0000 to 0x241F'FFFF) is defined as non-secure RAM area

\
After bootROM execution, the FSBL project executes in internal RAM, ensures proper MPU and caches setting and configures the maximum system clock frequency at 600 MHz. 

Next, it copies the secure application binary then the non-secure application binary from external Flash to internal RAM. When done, the program counter is set to the secure application entry point and the secure application in turn executes.

The secure application configures the RISAF peripheral to set FLEXRAM, SRAM1 and SRAM2 secure or non-secure attributes.

Finally, the system jumps in non-secure code and the green LED (GPIO PO.01) toggles in an infinite loop with a 200-ms period.



#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, Template, FSBL, LRUN, TrustZone, Isolation

### <b>Sub-project FSBL directory contents</b>

      - Templates/Template_Isolation_LRUN/FSBL/Inc/main.h                       Header for main.c module
      - Templates/Template_Isolation_LRUN/FSBL/Inc/extmem.h                     Header for extmem.c module
      - Templates/Template_Isolation_LRUN/FSBL/Inc/partition_stm32n657xx.h      Partition file
      - Templates/Template_Isolation_LRUN/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - Templates/Template_Isolation_LRUN/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - Templates/Template_Isolation_LRUN/FSBL/Inc/stm32_extmem_conf.h          External memory manager Configuration file
      - Templates/Template_Isolation_LRUN/FSBL/Src/main.c                       Main program
      - Templates/Template_Isolation_LRUN/FSBL/Src/extmem.c                     Code to initialize external memory
      - Templates/Template_Isolation_LRUN/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - Templates/Template_Isolation_LRUN/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - Templates/Template_Isolation_LRUN/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Sub-project AppliSecure directory contents</b>

      - Templates/Template_Isolation_LRUN/AppliSecure/Inc/main.h                      Header for main.c module
      - Templates/Template_Isolation_LRUN/AppliSecure/Inc/stm32n6xx_hal_conf.h        HAL Configuration file
      - Templates/Template_Isolation_LRUN/AppliSecure/Inc/stm32n6xx_it.h              Interrupt handlers header file
      - Templates/Template_Isolation_LRUN/AppliSecure/Inc/partition_stm32n657xx.h     Partition file
      - Templates/Template_Isolation_LRUN/AppliSecure/Src/main.c                      Main program
      - Templates/Template_Isolation_LRUN/AppliSecure/Src/secure_nsc.c                Secure Non-Secure callable functions implementation file
      - Templates/Template_Isolation_LRUN/AppliSecure/Src/stm32n6xx_it.c              Interrupt handlers
      - Templates/Template_Isolation_LRUN/AppliSecure/Src/system_stm32n6xx.c          STM32N6xx system source file

 ### <b>Sub-project AppliNonSecure directory contents</b>

      - Templates/Template_Isolation_LRUN/AppliNonSecure/Inc/main.h                      Header for main.c module
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Inc/stm32n6xx_hal_conf.h        HAL Configuration file
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Inc/stm32n6xx_it.h              Interrupt handlers header file
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Inc/stm32n6570_discovery_conf.h BSP Configuration file
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Src/main.c                      Main program
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Src/stm32n6xx_it.c              Interrupt handlers
      - Templates/Template_Isolation_LRUN/AppliNonSecure/Src/system_stm32n6xx_ns.c       STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This template runs on STM32N6 devices.

  - This template has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

  - The following OTP fuses are set in this template:

    - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled

**WARNING**

When OTP fuses are set, they can not be reset.

If the OTP fuseVDDIO3_HSLV is not already set, run the template once with boot mode set in development mode and NO_OTP_FUSE compilation switch disabled
(otherwise, it isn't possible to benefit from the maximum transfer rate to access the external flash).

For the following runs, select NO_OTP_FUSE option by default using the preprocessor definition in the IDE.


  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out. 

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Select first the FSBL workspace
 - Rebuild all files from sub-project FSBL (if no modification is done on FSBL project, this step can be done only once)
 - Select the Project_s workspace
 - Rebuild all files from sub-project Project_s
 - Select the Project_ns workspace
 - Rebuild all files from sub-project Project_ns
 - Resort to CubeProgrammer to add a header to the generated App_Secure binary Project_s.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project_s.bin -nk -of 0x80000000 -t fsbl -o Project_s-trusted.bin -hv 2.3 -dump Project_s-trusted.bin*
   - The resulting binary is Project_s-trusted.bin.
 - Do the same with App_NonSecure
   - *STM32_SigningTool_CLI.exe -bin Project_ns.bin -nk -of 0x80000000 -t fsbl -o Project_ns-trusted.bin -hv 2.3 -dump Project_ns-trusted.bin*
   - The resulting binary is Project_ns-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the secure application binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7010'0000 and the non-secure application binary and its header (Project_ns-trusted.bin) at address 0x7018'0000.

\
 To run the template with boot in development mode,

 - Load the FSBL binary in internal RAM using the IDE
 - Run the example

\
To run the template in boot from flash mode,

 - Resort to CubeProgrammer to add a header to the generated binary FSBL.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.3 -dump FSBL-trusted.bin*
   - The resulting binary is FSBL-trusted.bin. 
 - With CubeProgrammer, load the FSBL binary and its header (FSBL-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from Flash mode.
