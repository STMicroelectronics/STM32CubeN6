
## <b>Ux_Device_CDC_ECM Application Description</b>

This application provides an example of Azure RTOS CDC_ECM stack usage on STM32N6570-DK board, it shows how to run web HTTP server based application stack
over USB interface.

The application is designed to load files and web pages stored in SD card using a web HTTP server through USB interface using CDC_ECM
class, the code provides all required features to build a compliant web HTTP Server. The main entry function tx_application_define() is called by ThreadX during
kernel start, at this stage, the USBX initializes the network layer through USBx class (CDC_ECM) also the FileX and the NetXDuo system are initialized,
the NX_IP instance and the web HTTP server are created and configured, then the application creates two main threads:

  - usbx_app_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB_OTG HAL PCD driver and start the device.
  - nx_server_thread_entry (Prio :10; PreemptionPrio :10) used to assign a dynamic IP address, open the SD card driver as a FileX media and start the web HTTP server.
Fetching a dynamic IP address to the STM32N6570-DK board is a step blocking until an IP address is obtained.
Once the server is started, the user's browser can load web pages as index.html and STM32N6xxLED.html.

#### <b>Expected success behavior</b>

When an SD card is inserted into the STM32N6570-DK SD card reader and the board is powered up and connected to DHCP enabled Ethernet network.
The board IP address is printed on the HyperTerminal,then the web page can be loaded on the web browser after entring the url http://@IP/index.html also the user can switch to STM32N6xxLED.html page to toggle the green LED.
An example of web pages is provided for testing the application that can be found under "USBX/Ux_Device_CDC_ECM/Web_Content/"

#### <b>Error behaviors</b>

If the web HTTP server is not successfully started, the red LED should start blinking.
In case of other errors, the web HTTP server does not operate as designed (Files stored in the SD card are not loaded in the web browser).

#### <b>Assumptions if any</b>

The uSD card must be plugged before starting the application.
The board must be in a DHCP Ethernet network.

#### <b>Known limitations</b>

Hotplug is not implemented for this example, that is, the SD card is expected to be inserted before application running.

### <b>Notes</b>

- In main(), power domains are enabled early using:
  HAL_PWREx_EnableVddA(), HAL_PWREx_EnableVddIO2(), HAL_PWREx_EnableVddIO3(),
  HAL_PWREx_EnableVddIO4(), HAL_PWREx_EnableVddIO5() and HAL_PWREx_EnableVddUSB().
  This helps ensure required analog/IO/USB supplies are available before peripheral initialization..

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
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
       } >RAM_D1 AT> RAM_D1
    ```

       The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
       In the example above the ThreadX heap size is set to 64KBytes.
       The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
       Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
       Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

RTOS, ThreadX, USBX, Device, CDC_ECM, Network, NetxDuo, FileX, File ,SDMMC, UART

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657X0H3QU devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards Revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.
  - This application uses USART3 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = none
      - Flow control = None

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
  - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Device_CDC_ECM.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Ux_Device_CDC_ECM.bin -nk -of 0x80000000 -t fsbl -o Ux_Device_CDC_ECM-trusted.bin -hv 2.3 -dump Ux_Device_CDC_ECM-trusted.bin*
   - The resulting binary is Ux_Device_CDC_ECM-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Device_CDC_ECM-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
