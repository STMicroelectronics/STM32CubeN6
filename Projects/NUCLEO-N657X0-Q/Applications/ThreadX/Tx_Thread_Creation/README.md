
## <b>Tx_Thread_Creation Application Description</b>

This application provides an example of Azure RTOS ThreadX stack usage. It shows how to develop an application using the ThreadX thread management APIs.
It demonstrates how to create and destroy multiple threads using Azure RTOS ThreadX APIs. In addition, it shows how to use the preemption threshold between threads and change priorities on the fly.
The main entry function tx_application_define() is then called by ThreadX during kernel start. At this stage, the application creates 3 threads with different priorities :

  - tx_app_thread (Prio : 5; Preemption Threshold : 5)
  - ThreadOne (Prio : 10; Preemption Threshold : 10)
  - ThreadTwo (Prio : 10; Preemption Threshold : 9)

Once started, the <b>tx_app_thread</b> is suspended while waiting for the event flag.
The *ThreadOne* starts to toggle the *LED_BLUE* every 500ms while <b>ThreadTwo</b> cannot as its priority is less than the *ThreadOne* threshold.
After 5 seconds, it sends an event *THREAD_ONE_EVT* to the <b>tx_app_thread</b>.

After receiving the *THREAD_ONE_EVT*, the <b>tx_app_thread</b> changes the <b>ThreadTwo</b> priority to 8 and its preemption threshold to 8 to be higher than the *ThreadOne* threshold then waits for an event.

Now, the <b>ThreadTwo</b> can preempt the <b>ThreadOne</b> and start toggling the *LED_BLUE* every 200ms for 5 seconds. Once done, it sends the <b>THREAD_TWO_EVT</b> to <b>tx_app_thread</b>.
Once *ThreadTwo_Evt* is received, the <b>tx_app_thread</b> resets the <b>ThreadTwo</b> priority and preemption threshold to their original values (10, 10), <b>ThreadOne</b> is rescheduled and the above scenario is repeated.
After repeating the sequence above 3 times, the <b>tx_app_thread</b> should destroy <b>ThreadOne</b> and <b>ThreadTwo</b> and toggle the *LED_BLUE* each 1 second forever.

####  <b>Expected success behavior</b>

  - LED_BLUE toggles every 500ms for 5 seconds
  - LED_BLUE toggles every 200ms for 5 seconds
  - Success status (After 3 times) :  'LED_BLUE' toggles every 1 second forever.

#### <b>Error behaviors</b>

LED_RED is ON.
Error message is printed on UART.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", via the "TX_TIMER_TICKS_PER_SECOND" define, but this should be reflected in "tx_initialize_low_level.S" file too.
 - TrustZone is enabled by default on STM32N6 devices. When running ThreadX in the Secure world, the `TX_SINGLE_MODE_SECURE` flag must be added to the compiler/assembler's defined preprocessor symbols.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, which is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all the application's ThreadX related resources (threads, semaphores, memory pools...), but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
	 place in RAM_region    { last section FREE_MEM };
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

RTOS, ThreadX, Threading, Event flags, Preemption threshold


### <b>Hardware and Software environment</b>

  - This application runs on STM32N657X0H3QU devices
  - This application has been tested with STMicroelectronics NUCLEO-N657X0 boards revision MB1940-N657X0-C01 and can be easily tailored to any other supported device and development board.
  - This application uses USART1 to display output, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = none
      - Flow control = None

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Tx_Thread_Creation.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Tx_Thread_Creation.bin -nk -of 0x80000000 -t fsbl -o Tx_Thread_Creation-trusted.bin -hv 2.3 -dump Tx_Thread_Creation-trusted.bin*
   - The resulting binary is Tx_Thread_Creation-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Tx_Thread_Creation-trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Unplug the board then plug it again. The code then executes in boot from external Flash mode.