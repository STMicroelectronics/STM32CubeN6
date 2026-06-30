## <b>OEMuROT_Appli application description</b>

This project provides an OEMuROT application example. This application will display a menu on the console that allows
the download of a new versions of applications and data images.

At next reset, the OEMuROT will decrypt and install these new versions after successfully checking the authenticity
and integrity of the images.

The system clock frequency is the one configured by OEMuROT.

### <b>Keywords</b>

OEMuROT, Root Of Trust, MPU

### <b>Directory contents</b>

File | Description
 --- | ---
  ROT/OEMuROT_Appli/AppliSecure/Src/low_level_device.c       | Secure low level device configuration
  ROT/OEMuROT_Appli/AppliSecure/Src/low_level_ext_flash.c    | Secure low level external flash driver
  ROT/OEMuROT_Appli/AppliSecure/Src/main.c                   | Secure main program
  ROT/OEMuROT_Appli/AppliSecure/Src/secure_nsc.c             | Secure Non-Secure Callable (NSC) module
  ROT/OEMuROT_Appli/AppliSecure/Src/stm32n6xx_hal_msp.c      | Secure HAL MSP module
  ROT/OEMuROT_Appli/AppliSecure/Src/stm32n6xx_it.c           | Secure interrupt handlers |
  ROT/OEMuROT_Appli/AppliSecure/Src/system_stm32n6xx_s.c     | Secure STM32N6xx system clock configuration file
  ROT/OEMuROT_Appli/AppliSecure/Inc/main.h                   | Secure main program header file
  ROT/OEMuROT_Appli/AppliSecure/Inc/stm32_extmem_conf.h      | Secure header configuration for extmem module
  ROT/OEMuROT_Appli/AppliSecure/Inc/stm32n6xx_hal_conf.h     | Secure HAL Configuration file
  ROT/OEMuROT_Appli/AppliSecure/Inc/stm32n6xx_it.h           | Secure unterrupt handlers header file
  ROT/OEMuROT_Appli/AppliSecure/nsclib/appli_flash_layout.h  | Flash layout header file
  ROT/OEMuROT_Appli/AppliSecure/nsclib/low_level_ext_flash.h | Device definition for low_level_flash_ext driver
  ROT/OEMuROT_Appli/AppliSecure/nsclib/secure_nsc.h          | Secure Non-Secure Callable (NSC) module header file
  ROT/OEMuROT_Appli/AppliNonSecure/Src/com.c                 | Non-secure communication
  ROT/OEMuROT_Appli/AppliNonSecure/Src/common.c              | Non-secure common
  ROT/OEMuROT_Appli/AppliNonSecure/Src/fw_update_app.c       | Non-Secure firmware update service
  ROT/OEMuROT_Appli/AppliNonSecure/Src/main.c                | Non-secure main program
  ROT/OEMuROT_Appli/AppliNonSecure/Src/ns_data.c             | Non-secure data display module
  ROT/OEMuROT_Appli/AppliNonSecure/Src/stm32n6xx_it.c        | Non-secure interrupt handlers
  ROT/OEMuROT_Appli/AppliNonSecure/Src/system/stm32n6xx_ns.c | Non-secure STM32N6xx system clock configuration file
  ROT/OEMuROT_Appli/AppliNonSecure/Src/ymodem.c              | Non-secure YModem module
  ROT/OEMuROT_Appli/AppliNonSecure/Src/com.h                 | Non-secure communication header file
  ROT/OEMuROT_Appli/AppliNonSecure/Src/common.h              | Non-secure common header file
  ROT/OEMuROT_Appli/AppliNonSecure/Src/fw_update_app.h       | Non-Secure definitions for Firmware Update module
  ROT/OEMuROT_Appli/AppliNonSecure/Inc/main.h                | Non-secure main program header file
  ROT/OEMuROT_Appli/AppliNonSecure/Inc/ns_data.h             | Non-secure definitions for ns_data example
  ROT/OEMuROT_Appli/AppliNonSecure/Inc/stm32n6xx_hal_conf.h  | Non-secure HAL Configuration file
  ROT/OEMuROT_Appli/AppliNonSecure/Inc/stm32n6xx_it.h        | Non-secure interrupt handlers header file
  ROT/OEMuROT_Appli/AppliNonSecure/Inc/ymodem.h              | Non-secure definitions for Ymodem functionalities
  ROT/OEMuROT_Appli/Binary                                   | Application firmware image (application binary + MCUBoot header and metadata)


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657xx devices.

  - This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be easily tailored for
    STM32N6xx devices by following these steps
    (more details in the wiki article [<b>How to create ROT examples for STM32N6</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32N6)).

  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemurot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/OEMuROT, and follow its instructions.
Refer to ROT_Provisioning/OEMuROT readme for more information on the provisioning process.

After application startup, check in your "UART console" the menu is well displayed:

```
=================== Main Menu ============================
 Non-Secure Data --------------------------------------- 1
 New Fw Image ------------------------------------------ 2
 Selection :
```

To update the application firmware and/or the application data image version, you must:

  - Select the function "New Fw Image"
  - Select the type of image to update
  - Send the signed binary by using the menu : File > Transfer > YMODEM > Send...
  - Reset the board
  - After authenticity and intergrity checks the new images are decrypted and installed.

For more details, refer to STM32N6 Wiki articles:

  - [OEMuRoT for STM32N6](https://wiki.st.com/stm32mcu/wiki/Security:OEMuRoT_for_STM32N6).
  - [How to start with OEMuRoT on STM32N6](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMuRoT_on_STM32N6_MCUs).

#### <b>Note:</b>

Be aware that a default MPU configuration is already applied by OEMuRoT when jumping in your application. To ensure the security of the
device, the MPU is configured to allow only the user application code area to be executed, minimizing the risk of unauthorized code execution.
It is the user application responsibility to reconfigure the MPU to fit with its security needs.