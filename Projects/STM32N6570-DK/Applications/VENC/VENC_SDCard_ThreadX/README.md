## <b>VENC_SDCard_ThreadX Description</b>

This project demonstrates the use of the STM32N6 video encoder and the camera pipeline.

It is targeted to run on STM32N657xx devices on the STM32N6570-DK board from STMicroelectronics.

It allows easy testing of the following use cases:

  - Frame mode
  - 720p30, 480p30

This application is meant to run in internal SRAM via the Load & Run method. 

The application is stored in external flash and then loaded in internal RAM for execution.<br>

It can also be run in development-mode debug.

For information on those modes, see the "How to use" section.

### <b> Example behaviour </b>

This example takes a camera (IMX335) input and transfers it to the video encoder through the camera pipeline. 

The camera image is also displayed on the board's LCD screen for user feedback of what is being recorded.

The video encoder output stream is saved to the SD card.

For information on how to get data from the SD card after the execution of the example, see the "How to use" section.

This example uses ThreadX for its RTOS functionality. 

The ewl_conf.h file is set up accordingly to take advantage of ThreadX's memory management and synchronization features within the video encoder software.

The example does the following :

  - Enable all internal RAMs
  - Configure all necessary clocks
  - Configure all necessary security attributes (RISC and RIMC)
  - Enable the SD card via the BSP
  - Enable the camera via the BSP
  - Enable the LCD via the BSP
  - Initialize the encoder
  - Encode each 600 frames into a single file (`xxx.h264` will be created containing 600 frames = 10 sec @ 30fps)

Depending on the execution mode, some of the initialization will be performed in the FSBL.

#### <b>Error Handling</b><br>

Upon successful execution of the example, both LEDs on the board will turn on after initialization.

If an error occurs during initialization, the red LED will blink indefinitely (this can for example be caused by the SD card not being present).


### <b>Keywords</b>

Graphics, VENC, Encoding,  Hardware Encoding

### <b>Directory contents</b>

#### <b>Sub-project FSBL</b>

    - FSBL/Inc/main.h                    Header for main.c module
    - FSBL/Inc/extmem.h                  Header for extmem.c module
    - FSBL/Inc/partition_stm32n657xx.h   SAU partition configuration
    - FSBL/Inc/stm32n6xx_hal_conf.h      HAL Configuration file
    - FSBL/Inc/stm32n6xx_it.h            Interrupt handlers header file
    - FSBL/Inc/stm32_extmem_conf.h       External memory manager Configuration file
    - FSBL/Src/main.c                    Main program
    - FSBL/Src/extmem.c                  Code to initialize external memory
    - FSBL/Src/stm32n6xx_hal_msp.c       HAL MSP module
    - FSBL/Src/stm32n6xx_it.c            Interrupt handlers
    - FSBL/Src/system_stm32n6xx_fsbl.c   STM32N6xx system source file


#### <b>Sub-project Appli</b>

    - Appli/Core/Inc/app_filex.h                    FileX app header
    - Appli/Core/Inc/app_threadx.h                  ThreadX app header
    - Appli/Core/Inc/aps256xx_conf.h                APS256XX config
    - Appli/Core/Inc/dcmipp_app.h                   DCMI-PP app header
    - Appli/Core/Inc/ewl_conf.h                     EWL configuration file
    - Appli/Core/Inc/fx_stm32_sd_driver.h           FileX SD driver header
    - Appli/Core/Inc/main.h                         Header for main.c module
    - Appli/Core/Inc/partition_stm32n657xx.h        SAU partition configuration
    - Appli/Core/Inc/stm32_assert.h                 Assert function definition
    - Appli/Core/Inc/stm32n6570_discovery_conf.h    BSP configuration file
    - Appli/Core/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
    - Appli/Core/Inc/stm32n6xx_it.h                 Interrupt handlers header file
    - Appli/Core/Inc/utils.h                        Utility helpers
    - Appli/Core/Inc/venc_app.h                     VENC app API
    - Appli/Core/Inc/venc_h264_config.h             H.264 encoder config
    - Appli/Core/Inc/venc_h264_config_480p_Frame.h  H.264 480p frame config
    - Appli/Core/Inc/venc_h264_config_720p_Frame.h  H.264 720p frame config
    - Appli/Core/Src/app_filex.c                    FileX app source
    - Appli/Core/Src/app_threadx.c                  ThreadX app source
    - Appli/Core/Src/dcmipp_app.c                   DCMI-PP app source
    - Appli/Core/Src/fx_stm32_sd_driver_glue.c      FileX SD driver glue
    - Appli/Core/Src/lcd_app.c                      LCD app source
    - Appli/Core/Src/main.c                         Main program
    - Appli/Core/Src/sdcard_app.c                   SDCard recording logic
    - Appli/Core/Src/stm32n6xx_hal_timebase_tim.c   HAL timer timebase source file
    - Appli/Core/Src/stm32n6xx_it.c                 Interrupt handlers
    - Appli/Core/Src/SystemClock_Config_600MHz.c    System clock config 600MHz
    - Appli/Core/Src/SystemClock_Config_800MHz.c    System clock config 800MHz
    - Appli/Core/Src/system_stm32n6xx_s.c           STM32N6xx system source file (secure)
    - Appli/Core/Src/tx_initialize_low_level.S      ThreadX low-level init (ASM)
    - Appli/Core/Src/venc_app.c                     VENC app source
    - Appli/Core/Src/venc_h264_config.c             H.264 encoder config source 
    - Appli/Core/Inc/fx_user.h                      FileX user config


### <b>Hardware and Software environment</b>

  - This application runs on STM32N6 devices.

  - This application has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.
    
  - This application uses USART1 to display logs. The HyperTerminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

  - On STM32N6570-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - In order to use the full XSPI speed, the following OTP fuses need to be set 

  - VDDIO2_HSLV=1     I/O XSPIM_P1 High speed option enabled
  - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled
	
  - To ensure the project runs properly, **the SD card must be formatted in FAT32.** 



### <b>How to use it ?</b>

The example can be run either in development mode or in Load & Run mode.

#### <b> Development mode </b>

Make sure that BOOT1 switch position is 1-3. BOOT0 position does not matter. Then, plug in your board via the ST-LINK USB port.

Select the "Appli" tab in IAR and select the "DK_debug" configuration. This enables the development mode configuration.<br>
Then, simply click the execute button and the program will run in debug mode.

#### <b> Load & Run </b>

This mode enables execution without having to connect through an IDE. The application will be stored in external memory and therefore will not require any external tools once loaded onto the board.

It is expected that a command line environment is configured with the CubeProgrammer in its PATH.

 - Compile the FSBL project and the Appli project
 - Using the command line, add the header to the Appli and the FSBL (see "Annex : adding a header" section of this README)
 - Make sure that the board is in development mode (BOOT1 switch position is 1-3)
 - Program the external flash using CubeProgrammer with the FSBL (with header) at address 0x7000'0000 and the Application (with header) at address 0x7010'0000
 - Switch to boot from external flash mode : BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2
 - Press the reset button
 - The example should execute

__Note__: 2 scripts are provided as examples for signing and flashing IAR or CubeIDE builds:

  - VENC_SDCard_ThreadX/Tools/flash_IAR.sh
  - VENC_SDCard_ThreadX/Tools/flash_cubeIDE.sh

Please adapt the scripts to your environment.

#### <b> After execution </b>

After each 600 frames of video have been encoded, a new xxx.h264 file is completed. 
To play them back, the SD card should be plugged into a computer, and the xxx.h264 files should appear.

To read back encoded video, it can be converted from raw bytestream to mp4 using  **ffmpeg**  with the following command : `ffmpeg -f h264 -framerate 30 -i xxxx.h264 -c copy xxx.mp4`

The file can then be read using **ffplay** or any other regular video player.

[Windows ffmpeg can be found here ](https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full.7z)


#### <b> Adding a header </b>

 - Resort to CubeProgrammer to add a header to a binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.



**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
