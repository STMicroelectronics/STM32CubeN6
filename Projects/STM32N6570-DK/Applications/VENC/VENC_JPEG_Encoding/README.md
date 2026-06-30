## <b>VENC_JPEG_Encoding Description</b>

This project demonstrates the use of the STM32N6 video encoder peripheral for JPEG encoding.<br>It is targeted to run on STM32N657xx devices on the STM32N6570-DK board from STMicroelectronics.

This application is meant to run in internal SRAM. It is provided for illustration purposes and can only be executed in development mode.<br>

### <b> Example behavior </b>

This application demonstrates the use of the STM32N6 for encoding JPEG using the VENC peripheral. Images and streams must be placed in non-cached memory aligned to 16 bytes. 
The application takes an image in RGB565 format using the flip row and advanced modes to generate a 16-bit BMP. 
The full BMP is converted to C code. After encoding, the application saves the JPEG file in the current folder using standard I/O FILE functions.
At the end, the green or red LED indicates success or error.


#### <b>Error Handling</b><br>

Upon successful execution of the example, the green LED blinks with a 1-second period. In case of error, the red LED blinks with a 1-second period.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

Graphics, VENC, Encoding, JPEG, Hardware Encoding

### <b>Directory contents</b>

#### <b>Project</b>

    - VENC_JPEG_Encoding/FSBL/Inc/ewl_conf.h                Video Encoder Wrapper Layer header file
    - VENC_JPEG_Encoding/FSBL/Inc/main.h                    Header for main.c module
    - VENC_JPEG_Encoding/FSBL/Inc/stm32n6xx_hal_conf.h      HAL configuration file
    - VENC_JPEG_Encoding/FSBL/Inc/stm32n6xx_it.h            Interrupt handlers header file
    - VENC_JPEG_Encoding/FSBL/Inc/stm32n6570_discovery_conf.h BSP configuration file
    - VENC_JPEG_Encoding/FSBL/Src/main.c                    Main program
    - VENC_JPEG_Encoding/FSBL/Src/imageEncode.c             Code to encode the image
    - VENC_JPEG_Encoding/FSBL/Src/stm32n6xx_hal_msp.c       HAL MSP module
    - VENC_JPEG_Encoding/FSBL/Src/stm32n6xx_it.c            Interrupt handlers
    - VENC_JPEG_Encoding/FSBL/Src/refImage.bmp              Input image in bmp format
    - VENC_JPEG_Encoding/FSBL/Src/refImage_bmp_raw.c        Input image converted in C file
    - VENC_JPEG_Encoding/FSBL/Src/system_stm32n6xx_fsbl.c   STM32N6xx system source file    

### <b>Hardware and Software environment</b>

  - This application runs on STM32N6 devices.

  - This application has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

  - On STM32N6570-DK board, the BOOT1 mechanical slide switch must be set to position 1-3.


**WARNING**

  - The example can be run only in development mode.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follows:
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
