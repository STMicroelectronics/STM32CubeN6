@echo off
call ../env.bat
call img_config.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Data updated with the postbuild of OEMuROT-Boot
set bootaddress=0x70000000
set appli_s_address=0x700B0000
set data_s_address=0x700D0000
set appli_ns_address=0x70000000
set data_ns_address=0x0
set s_data_image_number=0x1
set ns_data_image_number=0x0
set last_sector=14

set signedbootpath=..\..\%oemurot_boot_path_project%\Binary\OEMuROT_Boot_Trusted.bin
set appli_s_path=..\..\%oemurot_appli_path_project%\Binary\%oemurot_appli_s%
set appli_ns_path=..\..\%oemurot_appli_path_project%\Binary\%oemurot_appli_ns%
set data_s_path=Binary\s_data_init_enc_sign.bin
set data_ns_path=Binary\ns_data_init_enc_sign.bin

:: CubeProgrammer connection
set connect_reset=-c port=SWD ap=1

:: ============================================================ Download images ============================================================

set "action=Reset the target"
echo %action%
set command="%stm32programmercli%" "%connect_reset%"
!command!

set "action=Erase OEMuROT sectors"
echo %action%
set command="%stm32programmercli%" "%connect_reset%" -el "%stm32ExtLoaderFlash%" -e [0 %last_sector%]
!command!
IF !errorlevel! NEQ 0 goto :error
echo "Mass done"

set "action=Write OEMuROT_Boot"
echo %action%
set command="%stm32programmercli%" "%connect_reset%" -el "%stm32ExtLoaderFlash%" -d "%signedbootpath%" %bootaddress% -v
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Boot Written"

echo "Application images programming in download slots"

if "%app_full_secure%" == "1" (
    :: Flash only secure application and secure data if present
    set "action=Write OEMuROT_Appli Secure"
    echo !action!
    set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %appli_s_path% %appli_s_address% -v"
    !command!
    IF !errorlevel! NEQ 0 goto :error
    echo "OEMuROT_Appli Secure Written"

    if "%s_data_image_number%"=="0x1" (
        set "action=Write OEMuROT Data Secure"
        echo !action!
        set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %data_s_path% %data_s_address% -v"
        !command!
        IF !errorlevel! NEQ 0 goto :error
        echo "OEMuROT Data Secure Written"
    )
) else (
    :: Flash secure application and data
    set "action=Write OEMuROT_Appli Secure"
    echo !action!
    set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %appli_s_path% %appli_s_address% -v"
    !command!
    IF !errorlevel! NEQ 0 goto :error
    echo "OEMuROT_Appli Secure Written"

    if "%s_data_image_number%"=="0x1" (
        set "action=Write OEMuROT Data Secure"
        echo !action!
        set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %data_s_path% %data_s_address% -v"
        !command!
        IF !errorlevel! NEQ 0 goto :error
        echo "OEMuROT Data Secure Written"
    )

    :: Flash non-secure application and data
    set "action=Write OEMuROT_Appli Non-Secure"
    echo !action!
    set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %appli_ns_path% %appli_ns_address% -v"
    !command!
    IF !errorlevel! NEQ 0 goto :error
    echo "OEMuROT_Appli Non-Secure Written"

    if "%ns_data_image_number%"=="0x1" (
        set "action=Write OEMuROT Data Non-Secure"
        echo !action!
        set command="%stm32programmercli%" %connect_reset% -el %stm32ExtLoaderFlash% -d %data_ns_path% %data_ns_address% -v"
        !command!
        IF !errorlevel! NEQ 0 goto :error
        echo "OEMuROT Data Non-Secure Written"
    )
)

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
