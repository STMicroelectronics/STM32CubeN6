# SAI Audio Loopback DMA Example Description

This example demonstrates how to configure the Serial Audio Interface (SAI) to transmit and receive audio data in a master/slave loopback topology using Direct Memory Access (GPDMA) on the STM32N6 microcontroller.

To ensure maximum performance and bypass Cortex-M55 D-Cache coherency issues, the transmit and receive audio double-buffers (`sai\\\\\\\\\\\\\\\_tx\\\\\\\\\\\\\\\_double\\\\\\\\\\\\\\\_buffer` and `sai\\\\\\\\\\\\\\\_rx\\\\\\\\\\\\\\\_double\\\\\\\\\\\\\\\_buffer`) are placed in a dedicated `\\\\\\\\\\\\\\\_\\\\\\\\\\\\\\\_NON\\\\\\\\\\\\\\\_CACHEABLE` memory region. The Memory Protection Unit (MPU) is specifically configured to enforce this policy.

The main loop initializes the transmit buffer with an incremental dummy data pattern. The GPDMA1 is then started to continuously transfer this data out of SAI1 and receive it into SAI2.

During the Half-Transfer and Full-Transfer DMA receiver callbacks, an optimized, loop-unrolled `memcmp32` function verifies that the received data perfectly matches the transmitted data.

## Audio Protocol Modes

The application can be configured to operate in one of two audio modes by modifying the `APP\\\\\\\\\\\\\\\_SAI\\\\\\\\\\\\\\\_MODE` macro definition located in `main.c`. The size of the DMA buffers is automatically scaled based on the selected mode.

* **TDM Mode (`APP\\\\\\\\\\\\\\\_SAI\\\\\\\\\\\\\\\_MODE\\\\\\\\\\\\\\\_TDM`):** This is the default configuration. The SAI blocks are configured to use a Free Protocol for Time Division Multiplexing (TDM). It processes 8 audio slots per frame at a 96 kHz sampling frequency.
* **I2S Mode (`APP\\\\\\\\\\\\\\\_SAI\\\\\\\\\\\\\\\_MODE\\\\\\\\\\\\\\\_I2S`):** The SAI blocks are configured to use the standard I2S protocol. It processes 2 audio slots (stereo left and right channels) per frame at a 96 kHz sampling frequency.

## Hardware Connections (SAI Loopback)

As this is a hardware-level loopback, the internal signals are not routed internally on the silicon. Physical connections must be made between the output pins of the Master (SAI1 Block B) and the input pins of the Slave (SAI2 Block B) using jumper wires.

Since the Master generates the clocks for the protocol, the wiring maps the outputs to the inputs based on the assigned GPIO pins:

|Signal Name|Description|Master (SAI1 Block B)|Slave (SAI2 Block B)|Connection|
|-|-|-|-|-|
|**SCK**|Serial Clock / Bit Clock|PG1|PE12|Connect PG1 to PE12|
|**FS**|Frame Sync / L-R Word|PG2|PE13|Connect PG2 to PE13|
|**SD**|Serial Data|PA3 (Transmit)|PE11 (Receive)|Connect PA3 to PE11|

*Note: The Master Clock (MCLK) is disabled on the Master in this configuration, making a 3-wire connection sufficient.*

## Debugging and Signaling

This example utilizes specific GPIOs for timing verification and error flagging. It is highly recommended to connect a logic analyzer to the debugging pins to profile the callback execution times.

**Debug Pins (Timing \& Profiling):**

* **D1 (PC4):** Toggles low on TX Complete Callback, high on TX Half-Complete Callback.
* **D2 (PC5):** Pulses during the execution of the RX Complete and RX Half-Complete Callbacks to profile the duration of the memory comparison routine.
* **D3 (PC0):** Pulses high if a data mismatch is detected during the memory comparison, or goes high permanently on a SAI HAL Error.

**LED Indicators:**

* **LED\_BLUE (PG8):** Toggles every 200ms in the main `while(1)` loop to indicate the system is running smoothly.
* **LED\_RED (PG10):** Turns ON and stays ON if a data corruption/mismatch occurs or if a SAI DMA error is triggered.
* **LED\_GREEN (PG0):** Available on the board but unused in this specific verification flow.

### **Keywords**

Audio, SAI, I2S, TDM, DMA, Loopback, Master, Slave, Non-Cacheable, MPU, Transmission, Reception

### **Directory contents**

* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Inc/stm32n6xx\_hal\_conf.h    HAL configuration file
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Inc/stm32n6xx\_it.h          Interrupt handlers header file
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Inc/main.h                  Header for main.c module
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Src/stm32n6xx\_it.c          Interrupt handlers
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Src/main.c                  Main program
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Src/system\_stm32n6xx\_fsbl.c stm32n6xx system source file
* SAI\_I2S\_Loopback\_DMA/FSBL/Core/Src/stm32n6xx\_hal\_msp.c     HAL MSP file

### **Hardware and Software Environment**

* This example runs on STM32N6 Series devices (Cortex-M55).
* This example has been designed to be easily tailored to any supported development board.
* **EWARM**: To monitor a variable in the live watch window, it is necessary to proceed as follows:

  * Start a debugging session.
  * Open the View > Images.
  * Double-click to deselect the second instance of project.out.
* **MDK-ARM**: To monitor a variable in the live watch window, the `SCB\\\\\\\\\\\\\\\_EnableDCache()` call in the `main()` function must be commented out.



### **How to use it ?**

In order to make the program work, you must do the following :

* Ensure the hardware loopback jumpers (SCK, FS, SD) are connected between the specified SAI1 and SAI2 peripheral pins.
* Observe the Blue LED toggling. If the Red LED illuminates, physical connections should be checked, and it must be verified that the MPU non-cacheable
  region is correctly aligned with the linker script.
* Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
* Open your preferred toolchain
* Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This is done by following the instructions below:

* Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command

  * *STM32\_SigningTool\_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
  * The resulting binary is Project-trusted.bin.
* Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
* Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
* Press the reset button. The code then executes in boot from external Flash mode.

**Warning** If using CubeProgrammer v2.21 version or more recent, add *-align* option in the command line.
