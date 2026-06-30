
## <b>Ux_Device_MSC Application Description</b>

This application provides an example of Azure RTOS USBX stack usage on STM32N6570-DK board, it shows how to develop USB device mass storage class based application.
The application is designed to emulate a USB MSC device, the code provides all required device descriptors framework and the associated class
descriptor report to build a compliant USB MSC device.

At the beginning ThreadX calls the entry function tx_application_define(), at this stage, all USBx resources are initialized, the mass storage class driver is
registered and the application creates one thread:

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB OTG HAL PCD driver and start the device.

In addition two callback functions are needed for the USBX mass storage class device:

  - USBD_STORAGE_Read used to read data through DMA from the mass storage device.
  - USBD_STORAGE_Write used to write data through DMA into the mass storage device.

### <b>Expected success behavior</b>

When plugged to PC host, the STM32N6570-DK should enumerate as a USB MSC device. During the enumeration phase, device must provide host with the requested
descriptors (device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by the host driver to identify the device capabilities.
Once the STM32N6570-DK USB device successfully completed the enumeration phase, a new removable drive appears in the system window and write/read/format
operations can be performed as with any other removable drive.

### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (MSC enumeration fails, the new removable drive appears but read, write or format operations fail) and the red LED should start blinking.

### <b>Assumptions if any</b>

  - USB cable should not be unplugged during enumeration and driver installation.
  - SD card should be inserted before application is started.

### <b>Known limitations</b>

The Eject operation is not supported yet by MSC class.

### <b>Notes</b>

- In FSBL main(), power domains are enabled early using:
  HAL_PWREx_EnableVddA(), HAL_PWREx_EnableVddIO2(), HAL_PWREx_EnableVddIO3(),
  HAL_PWREx_EnableVddIO4(), HAL_PWREx_EnableVddIO5() and HAL_PWREx_EnableVddUSB().
  This is done before HAL_Init() to ensure required analog/IO/USB supplies are available
  before USBPD/UCPD and USB-related initialization.

### <b>ThreadX usage hints</b>

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

### <b>USBX usage hints</b>

- None

### <b>Keywords</b>

RTOS, ThreadX, USBX, Device, USBPD, USB_OTG, High Speed, MSC, Mass Storage, SD Card, DMA, SDMMC

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657X0H3QU devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.

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

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Device_MSC.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Ux_Device_MSC.bin -nk -of 0x80000000 -t fsbl -o Ux_Device_MSC-trusted.bin -hv 2.3 -dump Ux_Device_MSC-trusted.bin*
   - The resulting binary is Ux_Device_MSC-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Device_MSC-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
