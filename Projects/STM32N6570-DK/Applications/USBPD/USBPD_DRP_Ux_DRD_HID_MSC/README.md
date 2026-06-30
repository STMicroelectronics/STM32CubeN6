
## <b>USBPD_DRP_Ux_DRD_HID_MSC Application Description</b>

This application is a USBPD type C DRP (supporting Data Role Swap) using Azure RTOS. USBPD DRP: This application initializes the Type C port 1 in DRP mode, allowing toggling between SNK and SRC roles.

In SNK role, the application is designed to emulate an USB HID mouse device, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB HID mouse device.

At the beginning ThreadX call the entry function tx_application_define(), at this stage, all USBx resources
are initialized, the HID Class driver is registered and the application creates 2 threads with the same priorities :

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB_OTG HAL PCD driver and start the device.
  - usbx_hid_thread_entry (Prio : 20; PreemptionPrio : 20) used to send HID reports to move automatically the PC host machine cursor.
To customize the HID application by sending the mouse position step by step every 10ms.
For each 10ms, the application calls the GetPointerData() API to update the mouse position (x, y) and send
the report buffer through the ux_device_class_hid_event_set() API.

In SRC role, the application is designed to behave as a USB MSC Host able to operate with an USB flash disk using the Bulk Only Transfer (BOT) and Small Computer System Interface (SCSI) transparent commands combined with a file system Azure RTOS FileX.

The main entry function tx_application_define() is then called by ThreadX during kernel start, at this stage, all USBx resources are initialized, the MSC class driver is registered.
The application creates two threads :

  - app_ux_host_thread_entry (Priority : 10; Preemption threshold : 10) used to initialize USB_OTG HAL HCD driver and start the Host.
  - msc_process_thread_entry (Priority : 30; Preemption threshold : 30) used to proceed to file operations once the device is properly enumerated.

####  <b>Expected success behavior</b>
Connect UCPD cube Monitor on the VCP associated to our board (only available if USB cable is connected)
The UCPD Cube Monitor will be used to trace all the messages exchange between the ports partner.

  When plugged to PC host, the STM32N6570-DK must be properly enumerated as an USB HID mouse device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the STM32N6570-DK USB device successfully completed the enumeration phase, the device sends a HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

  When a USB flash disk is plugged to STM32N6570-DK board, a message will be displayed on the UCPD Cube Monitor showing the Vendor ID and the Product ID of the attached device.
After enumeration phase, the host proceeds to file operations :

  - Create a "Test.txt" file.
  - Write a small text in the created file.
  - Read the written text and check data integrity.
  - Close the File.

During the file operations process a message will be displayed on the UCPD Cube Monitor to indicates the outcome of each operation  (Create/Write/Read/Close) .
If all operations were successful a message will be displayed on the UCPD Cube Monitor to indicates the end of operations.

#### <b>Error behaviors</b>

Errors are detected and the corresponding message is displayed on the UCPD Cube Monitor.

#### <b>Assumptions if any</b>

User is familiar with USB-PD 3.0 "Universal Serial BUS Power Delivery" Specification.

#### <b>Known limitations</b>

The remote wakeup feature for HID is not yet implemented (used to bring the USB suspended bus back to the active condition).

### <b>Notes</b>

- In main(), power domains are enabled early using:
  HAL_PWREx_EnableVddA(), HAL_PWREx_EnableVddIO2(), HAL_PWREx_EnableVddIO3(),
  HAL_PWREx_EnableVddIO4(), HAL_PWREx_EnableVddIO5() and HAL_PWREx_EnableVddUSB().
  This helps ensure required analog/IO/USB supplies are available before peripheral initialization.

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 1000 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
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
       } >RAM_D1 AT> RAM_D1
    ```

       The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
       In the example above the ThreadX heap size is set to 64KBytes.
       The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
       Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
       Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

Connectivity, USBPD, ThreadX, UART/USART, USB_PD_Lib, UCPD, Type C, DRD, DRP, USBX Device, USBX Host, MSC, Mass Storage, Removable drive, FileX, HID, Mouse.

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices
  - This application has been tested with STMicroelectronics STM32N6570-DK boards revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.

- STM32N6570-DK Set-up
    - Plug the USB key into the STM32N657xx board through 'Type C' cable to the connector:
      - CN18 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port to display data on the CubeMx Monitor.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of USBPD_DRP_Ux_DRD_HID_MSC.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
  - Open the configured uart HyperTerminal in order to display debug messages.
  - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary USBPD_DRP_Ux_DRD_HID_MSC.bin with the following command
      - *STM32_SigningTool_CLI.exe -bin USBPD_DRP_Ux_DRD_HID_MSC.bin -nk -of 0x80000000 -t fsbl -o USBPD_DRP_Ux_DRD_HID_MSC-trusted.bin -hv 2.3 -dump USBPD_DRP_Ux_DRD_HID_MSC-trusted.bin*
   - The resulting binary is USBPD_DRP_Ux_DRD_HID_MSC-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (USBPD_DRP_Ux_DRD_HID_MSC-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
