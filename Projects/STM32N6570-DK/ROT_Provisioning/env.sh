#!/bin/bash -

# Absolute path to this script
if [ $# -ge 1 ] && [ -d $1 ]; then
    projectdir=$1
else
    projectdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi

#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)

echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo ""
  echo " =========================================="
  echo "| For Windows OS, please use .bat scripts. |"
  echo " =========================================="
  echo ""
  echo "       Press any key to continue..."
  echo
  read -p "" -n1 -s
  exit 0
fi

if [ "$OS" == "Linux" ]; then
  echo "HOST OS : Linux detected"
elif [ "$OS" == "Darwin" ]; then
  echo "HOST OS : MacOS detected"
else
  echo "!!!HOST OS not supported : >$OS<!!!"
  exit 1
fi

#=================================================================================================
# Managing HOST OS diversity : end
#=================================================================================================

#==============================================================================
#                                    General
#==============================================================================
# Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
  stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
  stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
	stm32keygencli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_KeyGen_CLI.exe"
	stm32signingtoolcli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_SigningTool_CLI.exe"
	stm32ExtLoaderFlash="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\ExternalLoader\MX66UW1G45G_STM32N6570-DK.stldr"
	stm32ExtOTPInterace="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\ExternalLoader\OTP_FUSES_STM32N6xx.stldr"
	imgtool="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\Utilities\Windows\imgtool.exe"
else
  stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
  PATH=$stm32programmercli_path:$PATH
  stm32programmercli="STM32_Programmer_CLI"
  stm32tpccli="STM32TrustedPackageCreator_CLI"
	stm32keygencli="STM32_KeyGen_CLI"
	stm32signingtoolcli="STM32_SigningTool_CLI"
	stm32ExtLoaderFlash="$stm32programmercli_path/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"
	stm32ExtOTPInterace="$stm32programmercli_path/ExternalLoader/OTP_FUSES_STM32N6xx.stldr"
	imgtool="Utilities/Linux/imgtool"
fi

#==============================================================================
#                               OEMuROT bootpath
#==============================================================================
# Select application project below
oemurot_boot_path_project=Applications/ROT/OEMuROT_Boot
oemurot_appli_path_project=Applications/ROT/OEMuROT_Appli_Isolation
#oemurot_appli_path_project=Applications/ROT/OEMuROT_Appli
#oemurot_appli_path_project=Templates_ROT/OEMuROT_Appli_Isolation
#oemurot_appli_path_project=Templates_ROT/OEMuROT_Appli
#==============================================================================

#==============================================================================
#               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
#==============================================================================
cube_fw_path=$projectdir/../../../
oemurot_appli_s=rot_tz_s_app_init_enc_sign.bin
oemurot_appli_ns=rot_tz_ns_app_init_enc_sign.bin
rot_provisioning_path=$projectdir