## <b>OEMuROT_Appli Template description</b>

This project provides a OEMuROT boot path application example.

This project template is based on the STM32Cube HAL API. It highlights the ROT specificies:

  - Specific memory regions, defines automatically updated in the linker file.
  - Postbuild script (postbuild.bat (.sh)) called from the project files.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

The SystemClock_Config() function is used to configure the system clock (SYSCLK).

The system clock frequency is the one configured by OEMuROT.

### <b>Keywords</b>

Reference, Template, OEMuROT, Root Of Trust

### <b>Directory contents</b>

#### <b>Sub-project OEMuROT_Appli</b>

File | Description
 --- | ---
  Templates_ROT/AppliSecure/Inc/main.h                         | Header file for main.c
  Templates_ROT/AppliSecure/Inc/stm32n6xx_hal_conf.h           | HAL configuration file
  Templates_ROT/AppliSecure/Inc/stm32n6xx_it.h                 | Header file for stm32n6xx_it.h
  Templates_ROT/AppliSecure/Src/main.c                         | Secure Main program
  Templates_ROT/AppliSecure/Src/secure_nsc.c                   | Secure nonsecure callable functions
  Templates_ROT/AppliSecure/Src/stm32n6xx_it.c                 | Secure Interrupt handlers
  Templates_ROT/AppliSecure/Src/system_stm32n6xx_s.c           | Secure STM32n6xx system clock configuration file
  Templates_ROT/Secure_nsclib/secure_nsc.h                     | Header file for nonsecure callable functions
  Templates_ROT/AppliNonSecure/Inc/main.h                      | Header file for main.c
  Templates_ROT/AppliNonSecure/Inc/stm32n6xx_hal_conf.h        | HAL configuration file
  Templates_ROT/AppliNonSecure/Inc/stm32n6xx_it.h              | Header file for stm32n6xx_it.c
  Templates_ROT/AppliNonSecure/Inc/stm32n6570_discovery_conf.h | Header file for stm32n6570_discovery_conf.c
  Templates_ROT/AppliNonSecure/Src/main.c                      | Non secure Main application file
  Templates_ROT/AppliNonSecure/Src/stm32n6xx_hal_msp.c         | Non secure HAL MSP file
  Templates_ROT/AppliNonSecure/Src/stm32n6xx_it.c              | Non secure Interrupt handlers
  Templates_ROT/AppliNonSecure/Src/system_stm32n6xx.c          | Non secure System init file
  Templates_ROT/EWARM/startup_stm32n6xx.s                      | Startup file
  Templates_ROT/Binary                                         | Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657xx devices.

  - This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be easily tailored for
    STM32N6xx devices by following these steps
    (more details in the wiki article [<b>How to create ROT examples for STM32N6</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32N6)).

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemurot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/OEMuROT, and follow its instructions.
Refer to ROT_Provisioning/OEMuROT readme for more information on the provisioning process.

For more details, refer to STM32N6 Wiki articles:

  - [OEMuRoT for STM32N6](https://wiki.st.com/stm32mcu/wiki/Security:OEMuRoT_for_STM32N6).
  - [How to start with OEMuRoT on STM32N6 MCUs](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMuRoT_on_STM32N6_MCUs).

#### <b>Notes:</b>

1. Be aware that a default MPU configuration is already applied by OEMuRoT when jumping in your application. To ensure the security of the
device, the MPU is configured to allow only the user application code area to be executed, minimizing the risk of unauthorized code execution.
It is the user application responsibility to reconfigure the MPU to fit with its security needs.

2. **AppliCfg.py** (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning
     process. It is needed to have python and some python packages installed in your
     environment. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.