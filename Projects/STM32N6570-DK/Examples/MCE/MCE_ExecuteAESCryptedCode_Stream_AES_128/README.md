
## <b>MCE_ExecuteAESCryptedCode_Stream_AES_128 Example Description</b>

his project provides a description of how to run encrypted application code from external flash. The decryption is performed on the fly using the MCE IP(AES128 encryption algorithm).
  It boots from internal RAM and jumps to the encrypted application code in external flash. The user can provide their own application and encrypt it using the AES algorithm. In this case, the encryption key must be updated in the main.c file. The encrypted code consists of a basic application that toggles the user LD1 each 250 ms.

- This project is targeted to run on STM32N657X0H3QU device on STM32N6570-DK board from STMicroelectronics.  
- The FSBL project configures the maximum system clock frequency at 600 MHz.
- Once the application runs, the green LED (GPIO PO.01) toggles in an infinite loop with a 250-ms period.
STM32N6570-DK board's LED can be used to monitor the execution status:

 - [**LD1**]{style="color: GREEN"} toggles when the the encrypted code is successfully executed.
 - [**LD2**]{style="color: RED"}  Toggles as soon as an error occurs.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

MCE, Decryption, Instructions, AES, Basics

### <b>Sub-project FSBL directory contents</b>

  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/main.h                       Header for main.c module
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/extmem.h                     Header for extmem.c module
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/partition_stm32n657xx.h      Partition file
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Inc/stm32_extmem_conf.h          External memory manager Configuration file
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/boot/stm32_boot_xip.h            Header for stm32_boot_xip.c file
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/sal/stm32_sal_xspi_api.h         software adaptation layer XSPI functions prototypes
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/sal/stm32_sal_xspi_type.h        software adaptation layer XSPI functions prototypes
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Src/main.c                       Main program
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Src/extmem.c                     Code to initialize external memory
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/boot/stm32_boot_xip.c            Manages the boot in the mode execute in place
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/nor_sfdp/stm32_sfpd.c            SFPD support
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/nor_sfdp/stm32_sfdp_driver.c     SFPD support
  - MCE/MCE_ExecuteAESCryptedCode_Stream_AES_128/FSBL/sal/stm32_sal_xspi.c             software adaptation layer for XSPI

### <b>Hardware and Software environment</b>

  - This template runs on STM32N6 devices.

  - This template has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

  - The following OTP fuses are set in this template:

    - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled


  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn’t matter).
 - Resort to CubeProgrammer, load the Appli binary and its header (AESEncryptedAppli_Trusted_bin.bin) in DK board external Flash at address 0x7010’0000.
 - Load the FSBL binary in internal RAM using the IDE.
 - Run the example. 

#### <b>Notes</b>
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*






**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
