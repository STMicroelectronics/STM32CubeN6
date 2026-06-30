@ECHO OFF

:: arg1 is the binary type (1 nonsecure, 2 secure)
set "signing=%1"

:: Getting the Trusted Package Creator CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

:exe
::line for window executable
set applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe
set python=
if exist %applicfg% (
echo run config Appli with windows executable
goto postbuild
)
:py
::line for python
echo run config Appli with python script
set applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py
set "python= "

:postbuild
set s_app_init_xml="%provisioningdir%\OEMuROT\Images\OEMuROT_S_Code_Init_Image.xml"
set ns_app_init_xml="%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Code_Init_Image.xml"

set s_app_xml="%provisioningdir%\OEMuROT\Images\OEMuROT_S_Code_Image.xml"
set ns_app_xml="%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Code_Image.xml"

set s_app_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_s_app.bin"
set s_app_init_sign_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_s_app_init_enc_sign.bin"
set s_app_sign_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_s_app_enc_sign.bin"

set ns_app_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_ns_app.bin"
set ns_app_init_sign_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_ns_app_init_enc_sign.bin"
set ns_app_sign_bin="..\..\..\%oemurot_appli_path_project%\Binary\rot_tz_ns_app_enc_sign.bin"

if %signing% == "secure" (
echo Creating secure image >> %current_log_file% 2>&1

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin% --string -n "Firmware binary input file" %s_app_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin% --string -n "Firmware binary input file" %s_app_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_init_sign_bin% --string -n "Image output file" %s_app_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_sign_bin% --string -n "Image output file" %s_app_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

:: Creating signed/encrypted image
"%stm32tpccli%" -pb %s_app_init_xml% >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
"%stm32tpccli%" -pb %s_app_xml% >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
)

if %signing% == "nonsecure" (
echo Creating non-secure image >> %current_log_file% 2>&1

::update xml file : input file
%python%%applicfg% xmlval -v %ns_app_bin% --string -n "Firmware binary input file" %ns_app_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %ns_app_bin% --string -n "Firmware binary input file" %ns_app_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %ns_app_init_sign_bin% --string -n "Image output file" %ns_app_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %ns_app_sign_bin% --string -n "Image output file" %ns_app_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

:: Creating signed/encrypted image
"%stm32tpccli%" -pb %ns_app_init_xml% >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
"%stm32tpccli%" -pb %ns_app_xml% >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
)

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
