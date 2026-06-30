
## <b>VENC_USB Application Description</b>

This application provides an example of VENC usage on the STM32N6570-DK board, and shows how to develop a USB video device using the camera pipeline and VENC IP.

It is designed to be easily configurable:

  - Hardware handshake or frame mode
  - 1080p15, 720p30, 480p30

The application is designed to emulate a USB video device.

The code provides the required device descriptor framework
and associated class descriptor report to build a compliant USB video device.

At the beginning ThreadX calls the entry function tx_application_define(), at this stage, all USBx resources
are initialized, the video class driver is registered and the application creates one thread:

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB_OTG HAL PCD driver and start the device.

#### <b>Expected success behavior</b>

When plugged to PC host, the STM32N6570-DK must be properly enumerated as a USB video device.

During the enumeration phase, device provides host with the requested descriptors (device, configuration, string).

Those descriptors are used by host driver to identify the device capabilities.

Once the STM32N6570-DK USB device successfully completed the enumeration phase:

Use a camera tool supporting USB H.264.

The application was tested with ffplay under Windows with the command `ffplay -f dshow -i video="STM32 Video Device"`.

[Windows ffmpeg can be found here ](https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full.7z)


#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (video device enumeration failed).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" specification and video class specification.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

None

#### <b>ThreadX usage hints</b>

 - ThreadX uses SysTick as the time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

RTOS, ThreadX, USBX Device, USB_OTG, Full Speed, High Speed, Video, MJPEG.

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards Revision MB1939-N6570-A03 and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs. The HyperTerminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

  - In order to use the full XSPI speed, the following OTP fuses need to be set:
      - VDDIO2_HSLV=1     I/O XSPIM_P1 High speed option enabled
      - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled


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

  - VENC_USB/Tools/flash_IAR.sh
  - VENC_USB/Tools/flash_cubeIDE.sh

Please adapt the scripts to your environment.

#### <b> Adding a header </b>

 - Resort to CubeProgrammer to add a header to a binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.


**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
