@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "projectdir=%~dp0"
pushd "%projectdir%\..\..\..\..\ROT_Provisioning"
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"
set "img_config=%provisioningdir%\OEMuROT\img_config.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file=%projectdir%postbuild.log
echo. > %current_log_file%

::=================================================================================================
:: Check if Python V3 is installed
::-------------------------------------------------------------------------------------------------
python --version >nul 2>&1
if %errorlevel% neq 0 (
  echo.
  echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
  echo.
  set "command=Python installation"
  goto :error
)
set "python=python "
:: If found, capture version string removing "Python "
for /f "tokens=2 delims= " %%A in ('python --version 2^>^&1') do (
    set "full_version=%%A"
)
:: extract version details
for /F "tokens=1,2,3 delims=." %%A in ("!full_version!") do (
  set MAJOR_VER=%%A
  set MINOR_VER=%%B
  set PATCH_VER=%%C
)
:: is v3
if not "%MAJOR_VER%" == "3" (
  python3 --version >nul 2>&1
  if !errorlevel! neq 0 (
    echo.
    echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
    echo.
    set "command=Python installation"
    goto :error
  )
  set "python=python3 "
)
::=================================================================================================

:: Environment variable for AppliCfg
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"

:postbuild
set encrypted=0
set appli_dir=..\..\..\..\%oemurot_appli_path_project%
set binary_file=%projectdir%..\Binary\OEMuROT_Boot.bin
set trusted_binary_file=%projectdir%..\Binary\OEMuROT_Boot_Trusted.bin

set bootrom_path=%provisioningdir%\BootROM
set pbk1=%bootrom_path%\Keys\publicKey00.pem
set pbk2=%bootrom_path%\Keys\publicKey01.pem
set pbk3=%bootrom_path%\Keys\publicKey02.pem
set pbk4=%bootrom_path%\Keys\publicKey03.pem
set pbk5=%bootrom_path%\Keys\publicKey04.pem
set pbk6=%bootrom_path%\Keys\publicKey05.pem
set pbk7=%bootrom_path%\Keys\publicKey06.pem
set pbk8=%bootrom_path%\Keys\publicKey07.pem

set pvk=%bootrom_path%\Keys\privateKey00.pem

set derivval=0x7c098af2
set enck=%bootrom_path%\Keys\OEM_SECRET.bin
set version=0x00000001

set scmd=-pubk "%pbk1%" "%pbk2%" "%pbk3%" "%pbk4%" "%pbk5%" "%pbk6%" "%pbk7%" "%pbk8%" -prvk "%pvk%" -pwd rot1
if "%encrypted%" == "1" (
set enccmd=-encdc %derivval% -enck "%enck%"
set optionflag=-t fsbl -iv %version% -la 0x34180000 -of 0x80000003
) else (
set enccmd=
set optionflag=-t fsbl -iv %version% -la 0x34180000 -of 0x80000001
)

:: Variable for AppliCfg
set preprocess_bl2_file=%projectdir%image_macros_preprocessed_bl2.c
set provisioning=%provisioningdir%\OEMuROT\provisioning.bat
set flash_programming=%provisioningdir%\OEMuROT\flash_programming.bat

set appli_flash_layout=%appli_dir%\Secure_nsclib\appli_flash_layout.h
set appli_region_def=%appli_dir%\Secure_nsclib\appli_region_def.h

:Update img_config
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m RE_APP_IMAGE_NUMBER --decimal %img_config% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER --decimal %img_config% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER --decimal %img_config% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_full_secure -m  RE_OEMUROT_APPLI_FULL_SECURE --decimal %img_config% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

call %img_config%
 
IF "%app_full_secure%" == "1" (
set appli_s_main_file=%appli_dir%\Src\main.c
set appli_s_linker_file=%appli_dir%\EWARM\stm32n657xx_s.icf
set appli_s_system_file=%appli_dir%\Src\system_stm32n6xx_s.c
) else (
 set appli_s_linker_file=%appli_dir%\EWARM\AppliSecure\stm32n657xx_s.icf
 set appli_s_system_file=%appli_dir%\AppliSecure\Src\system_stm32n6xx_s.c
 set appli_s_main_file=%appli_dir%\AppliSecure\Src\main.c
 set appli_ns_linker_file=%appli_dir%\EWARM\AppliNonSecure\stm32n657xx_ns.icf
 set appli_ns_system_file=%appli_dir%\AppliNonSecure\Src\system_stm32n6xx_ns.c
)

set s_code_init_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_S_Code_Init_Image.xml
set ns_code_init_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Code_Init_Image.xml
set s_data_init_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_S_Data_Init_Image.xml
set ns_data_init_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Data_Init_Image.xml

set s_code_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_S_Code_Image.xml
set ns_code_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Code_Image.xml
set s_data_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_S_Data_Image.xml
set ns_data_xml=%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Data_Image.xml

:imagesigning
set command="%stm32signingtoolcli%" -bin "%binary_file%" %scmd% %enccmd% %optionflag% -o "%trusted_binary_file%" -hv 2.3 -s -align
%command% > %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error

:provisioning
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:flashprogramming
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b appli_s_address -m RE_FLASH_AREA_2_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b data_s_address -m RE_FLASH_AREA_6_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b appli_ns_address -m RE_FLASH_AREA_3_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b data_ns_address -m RE_FLASH_AREA_7_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b last_sector -m RE_FLASH_END_SECTOR -d 0x10000 --decimal %flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:xml_files
set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -c S %s_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -c S %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_NS_PARTITION_SIZE -c S %ns_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_NS_PARTITION_SIZE -c S %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_S_DATA_PARTITION_SIZE -c S %s_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_S_DATA_PARTITION_SIZE -c S %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_NS_DATA_PARTITION_SIZE -c S %ns_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_NS_DATA_PARTITION_SIZE -c S %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout %preprocess_bl2_file% -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %s_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %s_code_init_xml% -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %s_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %s_code_xml% -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %ns_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %ns_code_init_xml% -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %ns_code_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %ns_code_xml% -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %s_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %s_data_init_xml% -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %s_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %s_data_xml% -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %ns_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %ns_data_init_xml% -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %ns_data_init_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %ns_data_xml% -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:appli_s
set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_APPLI_S_ADDRESS -n CODE_S_ADDRESS %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -n CODE_S_SIZE %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_DATA_S_ADDRESS -n DATA_S_ADDRESS %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_DATA_S_SIZE -n DATA_S_SIZE %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APPLI_S_ADDRESS -n VECT_TAB_BASE_ADDRESS %appli_s_system_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

IF "%app_full_secure%" EQU "0" goto run_definevalue
goto :flash_layout

:run_definevalue
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APPLI_NS_ADDRESS -n VTOR_TABLE_NS_START_ADDR %appli_s_main_file% --vb"
%command% >> "%current_log_file%" 2>&1

:appli_ns
set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_APPLI_NS_ADDRESS -n CODE_NS_ADDRESS %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_NS_PARTITION_SIZE -n CODE_NS_SIZE %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_DATA_NS_ADDRESS -n DATA_NS_ADDRESS %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_DATA_NS_SIZE -n DATA_NS_SIZE %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APPLI_NS_ADDRESS -n VECT_TAB_BASE_ADDRESS %appli_ns_system_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:flash_layout
:: Bypass configuration of appli flash layout file if not present
if not exist %appli_flash_layout% goto :exit

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DOWNLOAD_MENU -n DOWNLOAD_MENU %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_OVERWRITE -n MCUBOOT_OVERWRITE_ONLY %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_DATA_IMAGE_NUMBER -n S_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n NS_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA2_NS_ADDRESS -n NS_DATA_ADDRESS %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_0_OFFSET -n S_APPLI_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_1_OFFSET -n NS_APPLI_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_4_OFFSET -n S_DATA_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_5_OFFSET -n NS_DATA_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_2_OFFSET -n S_APPLI_SECONDARY_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_3_OFFSET -n NS_APPLI_SECONDARY_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_6_OFFSET -n S_DATA_SECONDARY_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_7_OFFSET -n NS_DATA_SECONDARY_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -n S_APPLI_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_NS_PARTITION_SIZE -n NS_APPLI_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_6_SIZE -n S_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_AREA_7_SIZE -n NS_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if not exist %appli_region_def% goto :exit

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APPLI_S_ADDRESS -n S_CODE_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -n S_CODE_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_S_ADDRESS -n S_RAM_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_CODE_START -n S_CODE_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_CODE_START -n NS_CODE_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_CODE_LIMIT -n S_CODE_LIMIT %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_CODE_LIMIT -n NS_CODE_LIMIT %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_CODE_SIZE -n S_CODE_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_CODE_SIZE -n NS_CODE_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_S_ADDRESS -n S_RAM_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_S_SIZE -n S_RAM_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_NS_ADDRESS -n NS_RAM_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_START -n NS_DATA_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_SIZE -n NS_DATA_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_LIMIT -n NS_DATA_LIMIT %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA2_START -n NS_DATA2_START %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA2_SIZE -n NS_DATA2_SIZE %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA2_LIMIT -n NS_DATA2_LIMIT %appli_region_def% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:exit
exit 0

:error
echo "%command% failed"
echo See %current_log_file% for details.
cmd /k
exit 1
