
## <b>Ux_Host_Audio_2.0 Application Description</b>

This application provides an example of Azure RTOS USBX stack usage. It shows how to develop USB Host Audio able to enumerate and communicate with a device audio speaker 1.0/2.0.

The main entry function tx_application_define() is then called by ThreadX during kernel start, at this stage, all USBx resources are initialized, the AUDIO Class driver is registered.
The application creates two threads :

  - usbx_app_thread_entry       (Priority : 10; Preemption threshold : 10) used to initialize USB OTG HAL HCD driver and start the Host.
  - audio_playback_thread_entry (Priority : 30; Preemption threshold : 30) used to proceed to audio once the device is properly enumerated.



####  <b>Expected success behavior</b>

When an usb speaker is plugged to STM32N6570-DK board, a Message will be displayed on the uart HyperTerminal showing  the Vendor ID and the Product ID of the attached device.
After enumeration phase , the host proceed to file operations :

  - "Explore audio file" operation searches for all .Wav files in the uSD and displays it on the terminal.
  - "Once user press user button Start audio Player" operation starts playing the song from the uSD. The files are read using the FileX file system.

#### <b>Error behaviors</b>

Errors are detected such as (Unsupported device, Enumeration Fail, File operations fail)and the corresponding message is displayed on the HyperTerminal.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and audio2.0/audio1.0 class Specification.

#### <b>Known limitations</b>

None

#### <b>Notes</b>

- In main(), power domains are enabled early using:
  HAL_PWREx_EnableVddA(), HAL_PWREx_EnableVddIO2(), HAL_PWREx_EnableVddIO3(),
  HAL_PWREx_EnableVddIO4(), and HAL_PWREx_EnableVddIO5().
  This helps ensure required analog/IO supplies are available before peripheral initialization.

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This require changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```
    + For MDK-ARM:
    ```
    either define the RW_IRAM1 region in the ".sct" file
    or modify the line below in "tx_initialize_low_level.S to match the memory region being used
        LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
    ```
    + For STM32CubeIDE add the following section into the .ld file:
    ```
    ._threadx_heap :
      {
         . = ALIGN(8);
         __RAM_segment_used_end__ = .;
         . = . + 64K;
         . = ALIGN(8);
       } >RAM AT> RAM
    ```

       The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
       In the example above the ThreadX heap size is set to 64KBytes.
       The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
       Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
       Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

Connectivity, USBXHost, USBPD, FILEX, ThreadX, AUDIO 2.0, Audio 1.0, UAC, Speaker, UART/USART

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0H3QU devices.
  - This example has been tested with STMicroelectronics STM32N6570-DK boards revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.

- STM32N6570-DK Set-up
    - Plug the USB key into the STM32N657xx board through 'Type C' cable to the connector:
      - CN18 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

  - A virtual COM port will then appear in the HyperTerminal:
     - Hyperterminal configuration
       - Data Length = 8 Bits
       - One Stop Bit
       - No parity
       - BaudRate = 115200 baud
       - Flow control: None

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
  - Open the configured uart HyperTerminal in order to display debug messages.
  - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Host_Audio.bin with the following command
    - *STM32_SigningTool_CLI.exe -bin Ux_Host_Audio.bin -nk -of 0x80000000 -t fsbl -o Ux_Host_Audio-trusted.bin -hv 2.3 -dump Ux_Host_Audio-trusted.bin*
 - The resulting binary is Ux_Host_Audio-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Host_Audio-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

### <b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the Stlink VCP.

To support audio class 2.0 this flag "UX_HOST_CLASS_AUDIO_2_SUPPORT" should be defined in ux_user.h.

Please note that audio class 1.0 works only in FS mode.

**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
