
@ECHO OFF
:: Getting the CubeProgammer_cli path 
call ../env.bat
call "%~dp0img_config.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set "projectdir=%~dp0"

set s_data_image_number=0x1
set ns_data_image_number=0x0

set s_data_init_xml="%projectdir%Images\OEMuROT_S_Data_Init_Image.xml"
set ns_data_init_xml="%projectdir%Images\OEMuROT_NS_Data_Init_Image.xml"
set s_data_xml="%projectdir%Images\OEMuROT_S_Data_Image.xml"
set ns_data_xml="%projectdir%Images\OEMuROT_NS_Data_Image.xml"
set flash_programming="flash_programming.bat"

set flash_log="flash_programming.log"
set provisioning_log="provisioning.log"

set appli_dir="../../%oemurot_appli_path_project%"

:prov
echo =====
echo ===== Provisioning of OEMuRoT boot path
echo ===== Application selected through env.bat:
echo =====   %oemurot_appli_path_project%
echo =====
echo.

:: Check if application path exists
if not exist %appli_dir% (
echo =====
echo ===== Wrong Boot path: %oemurot_appli_path_project%
echo ===== please modify the env.bat to set the right path
goto step_error
)

:configuration_management
echo Step 1 : Configuration management

echo    * BootROM keys configuration
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys
echo        Update keys located in .\ROT_Provisioning\BootROM\Keys
echo        You can use .\ROT_Provisioning\BootROM\keys_generation.bat to generate new keys
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * OEMuROT keys configuration
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys
echo        Update keys located in .\ROT_Provisioning\OEMuROT\Keys
echo        You can use .\ROT_Provisioning\OEMuROT\keys_generation.bat to generate new keys
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * OEMuROT DA password configuration
echo        Warning: Default password must NOT be used. Make sure to choose your own
echo        Update the password located in .\ROT_Provisioning\OEMuROT\Config\DA_password.bin
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul


:: ======================================================== Images generation steps ========================================================
:image_generation
echo Step 2 : Images generation
echo    * OEMuROT image generation (BootROM format)
echo        Open the OEMuROT_Boot project with preferred toolchain and rebuild all files.
echo        .\ROT\OEMuROT_Boot\Binary\OEMuROT_Boot_Trusted.bin is generated at this step
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

if "%app_full_secure%" == "1" (
  echo    * Application firmware image generation (MCUboot format^)
  echo        Open the OEMuROT_Appli project with preferred toolchain and rebuild all files.
  echo        .\%oemurot_appli_path_project%\Binary\rot_tz_xx_app_enc_sign.bin is generated at this step
) else (
  echo    * Application firmware image generation (MCUboot format^)
  echo        Open the OEMuROT_Appli_Isolation project with preferred toolchain and rebuild all files.
  echo        Build must be done in this order Secure ^> NonSecure in case of trust zone application.
)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

if "%s_data_image_number%" == "0x1" (
echo    * Data secure image generation
echo        Select OEMuRoT_S_Data_Image.xml(Default path is .\ROT_Provisioning\OEMuROT\Images\OEMuROT_S_Data_Image.xml^)
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul
echo.
echo        Generating the ./Binary/s_data_enc_sign.bin image...
echo.
"%stm32tpccli%" -pb "%s_data_init_xml%" > %provisioning_log%
if !errorlevel! neq 0 goto :step_error
"%stm32tpccli%" -pb "%s_data_xml%" > %provisioning_log%
if !errorlevel! neq 0 goto :step_error
)

if "%app_full_secure%" == "1" (goto :no_ns_data)
if "%ns_data_image_number%" == "0x1" (
echo    * Data non secure image generation
echo        Select OEMuRoT_NS_Data_Image.xml(Default path is .\ROT_Provisioning\OEMuROT\Images\OEMuROT_NS_Data_Image.xml^)
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul
echo.
echo        Generating the ./Binary/ns_data_enc_sign.bin image...
echo.
"%stm32tpccli%" -pb "%ns_data_init_xml%" >> %provisioning_log%
if !errorlevel! neq 0 goto :step_error
"%stm32tpccli%" -pb "%ns_data_xml%" >> %provisioning_log%
if !errorlevel! neq 0 goto :step_error
)
:no_ns_data

:: ================================================== Flash programming ===================================================
:provisioning
echo Step 3 : Provisioning

echo    * Boot mode should be set to development mode
echo        (BOOT1 switch position at 1-3)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * OTP programming
echo        Warning : The OTP programming must be done only once.
echo        The OTP configuration data are :
echo             - BootROM keys
echo             - OEMuROT keys
echo             - OEMuROT DA password hash
echo        The following scripts can be used to program those configuration data :
echo             - .\ROT_Provisioning\BootROM\keys_programming.bat
echo             - .\ROT_Provisioning\OEMuROT\keys_programming.bat
echo             - .\ROT_Provisioning\OEMuROT\da_programming.bat
echo.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

set "action=Flashing the images..."
set current_log_file=%flash_log%
set "command=start /w /b call %flash_programming% AUTO"
echo    * %action%
%command% > %flash_log%

if !errorlevel! neq 0 goto :step_error

echo        Successful images flashing
echo        (see %flash_log% for details)
echo.

:: ============================================================= End functions =============================================================

echo    * Boot mode should be set to flash mode
echo        (BOOT1 switch position at 1-2 and BOOT0 switch position at 1-2)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

:: All the steps to set the STM32N6 product were executed correctly
:final_step
echo =====
echo ===== The board is correctly configured.
echo ===== Connect UART console (115200 baudrate) and press Reset to start application.
echo =====

:: Keep the window open
cmd /k

exit 0

:: Error when a command fails
:step_error
echo.
echo =====
echo ===== Error while executing %action%.
echo ===== See %current_log_file% for details. Then try again.
echo =====
cmd /k
exit 1
