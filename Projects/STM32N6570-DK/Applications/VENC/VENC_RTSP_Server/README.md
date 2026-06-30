
##  <b>VENC_RTSP_Server Application Description</b>

This application demonstrates the H.264 video encoder streaming through Azure RTOS NetX/NetXDuo on the STM32N6570-DK board.

It enables testing of the following use cases:

- H.264 encoding in 1080p, 720p, or 480p, in Frame mode or Hardware Handshake mode (also known as Slice mode or Streaming mode)
- H.264 encoding with optional uncompressed audio (PCM)

The application streams encoded H.264 video with optional audio.

- H.264 encoding with optional compressed audio (Opus)

The application streams encoded H.264 video with optional compressed audio.

Frames are transmitted through the Ethernet peripheral using the RTP (Real-time Transport Protocol) to a remote client, such as VLC media player or ffmpeg.

The RTSP (Real-Time Streaming Protocol) controls media sessions, including SETUP, PLAY, and TEARDOWN commands.


### Thread Overview

The main entry function tx_application_define() is called by ThreadX during kernel start.
The following threads are created.

**User Threads:**

- **Monitor App Thread** (`monitor_thread_func`, priority 63):  
  Monitors system performance metrics such as CPU load and bitrates.

- **VENC App Thread** (`venc_thread_func`, priority 12):  
  Handles video encoding operations, manages the video capture pipeline, and streams encoded video frames over the network.

**NetXDuo Threads:**

- **App Main Thread** (`App_Main_Thread_Entry`, priority 10):  
  Manages the main application logic, coordinating initialization, control flow, and high-level events for the RTSP server application.

- **App Link Thread** (`App_Link_Thread_Entry`, priority 11):  
  Handles the initialization and management of the application’s network link status and related events.

- **NetX DHCP Client** (`_nx_dhcp_thread_entry`, priority 3):  
  Manages the DHCP client operations, including requesting and renewing the IP address for the network interface.

- **RTSP Server** (`_nx_rtsp_server_thread_entry`, priority 3):  
  Manages the RTSP server operations, handling client connections, session control, and streaming setup for real-time audio/video transmission.

- **Test Thread** (`app_server_entry`, priority 4):  
  Manages the RTSP server thread, handling client connections, session control, and coordinating video/audio streaming operations.

- **Main IP Instance** (`_nx_ip_thread_entry`, priority 1):  
  Manages the IP stack operations, including packet processing, routing, and network interface management for the application.

####  <b>Expected success behavior</b>

- The board IP address is printed on HyperTerminal at 115200 baud

#####  <b>Playback using ffplay</b>
Playback with low latency can be done using ffplay:
```sh
ffplay -flags low_delay rtsp://[IP]
```
Note: This application was tested with ffplay version 6.1.1-3ubuntu5

#####  <b>Playback using VLC</b>
Enter rtsp://[IP]:554 in the Media/Open Network Stream window.
 
 - Streaming video is displayed correctly in VLC after entering the URL rtsp://[IP]:554 in the Media/Open Network Stream/Open Media window
   (example rtsp://192.168.0.27:554)
 - It is recommended to set the caching value to 500 ms in VLC to keep the stream running smoothly
   (Media/Open Network Stream/Open Media window => Show more options), then press Play.
 - The response messages sent by the server are printed on HyperTerminal.

Note: This application was tested with VLC version 3.0.19  **using Live555 plugin**


#### <b>Expected error behavior</b>

- The red LED toggles every 250 ms to indicate that an error has occurred.
- In case the message exchange is not completed, HyperTerminal does not print the received messages.

#### <b>Assumptions if any</b>

- The application uses DHCP to acquire an IP address; thus, a DHCP server should be reachable by the board in the RTL used to test the application.
- The application is configuring the Ethernet IP with a static predefined <i>MAC Address</i>. Ensure to change it if multiple boards are connected on the same RTL to avoid any potential network traffic issues.
- The <i>MAC Address</i> is defined in the `main.c` file.

```
void MX_ETH_Init(void)
{
  heth.Instance = ETH1;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE0;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x10;
  MACAddr[5] = 0x00;
```
#### <b>Known limitations</b>

The RTSP server is minimalist and does not support multiple client connections/disconnections.

#### <b>ThreadX usage hints</b>

- ThreadX uses SysTick as its time base, thus it is mandatory that the HAL uses a separate time base through a TIM peripheral.
- ThreadX is configured with 100 ticks/sec by default. This should be considered when using delays or timeouts in the application. It can be reconfigured by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in the "tx_initialize_low_level.S" file too.
- ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
- ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
  It is highly recommended to use it to create all application ThreadX-related resources (threads, semaphores, memory pools...), but it should not contain any system API call (HAL or BSP).
- Using dynamic memory allocation requires applying some changes to the linker file.
  ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function using the first_unused_memory argument.
  This requires changes in the linker files to expose this memory location.
  - For EWARM add the following section into the .icf file:

     ```
     place in RAM_region    { last section FREE_MEM };
     ```


#### <b>NetX Duo usage hints</b>

- Depending on the application scenario, the total TX and RX descriptors may need to be increased by updating respectively  the "ETH_TX_DESC_CNT" and "ETH_RX_DESC_CNT" in the "stm32n6xx_hal_conf.h", to ensure the application correct behaviour, but this will cost extra memory to allocate.
- The NetXDuo application needs to allocate a specific pool in an uncached section.
Below is an example of the declaration of the nx pool.

```
  #if (USE_STATIC_ALLOCATION == 1)
  static UCHAR nx_byte_pool_buffer[NX_APP_MEM_POOL_SIZE] __ALIGN_END __NON_CACHEABLE;
  static TX_BYTE_POOL nx_app_byte_pool;
  #endif
```

with 
```
#define __NON_CACHEABLE __attribute__((section(".noncacheable")))
```

The definition of the uncached section must be consistent between the linker file and the MPU configuration.


For more details about the MPU configuration please refer to the [AN4838](https://www.st.com/resource/en/application_note/dm00272912-managing-memory-protection-unit-in-stm32-mcus-stmicroelectronics.pdf)

### <b>Keywords</b>

RTOS, Network, ThreadX, NetXDuo, RTP, RTSP, TCP, UDP, UART

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

  - VENC_RTSP_Server_v1.4.0/Tools/flash_IAR.sh
  - VENC_RTSP_Server_v1.4.0/Tools/flash_cubeIDE.sh

Please adapt the scripts to your environment.

#### <b> Adding a header </b>

 - Resort to CubeProgrammer to add a header to a binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.


**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
