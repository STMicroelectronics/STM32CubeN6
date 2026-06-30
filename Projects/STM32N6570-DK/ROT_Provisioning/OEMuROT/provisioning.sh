#!/bin/bash -

# Getting the CubeProgammer_CLI path
source ../env.sh
source img_config.sh

# Absolute path to this script
if [ $# -ge 1 ] && [ -d $1 ]; then
  projectdir=$1
else
  projectdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi

s_data_image_number=0x1
ns_data_image_number=0x1

s_data_init_xml=$projectdir/Images/OEMuROT_S_Data_Init_Image.xml
ns_data_init_xml=$projectdir/Images/OEMuROT_NS_Data_Init_Image.xml
s_data_xml=$projectdir/Images/OEMuROT_S_Data_Image.xml
ns_data_xml=$projectdir/Images/OEMuROT_NS_Data_Image.xml
flash_programming=flash_programming.sh

provisioning_log=provisioning.log

appli_dir=../../$oemurot_appli_path_project

prov()
{
  echo "====="
  echo "===== Provisioning of OEMuRoT boot path"
  echo "===== Application selected through env.sh:"
  echo "=====   $oemurot_appli_path_project"
  echo "====="
  echo

  # Check if application path exists
  if [ ! -d "$appli_dir" ]; then
    echo "====="
    echo "===== Wrong Boot path: $oemurot_appli_path_project"
    echo "===== please modify the env.sh to set the right path"
    step_error
  fi

	configuration_management
  image_generation
  provisioning
  final_step
}

configuration_management()
{
    echo "Step 1 : Configuration management"
	
    echo "    * BootROM keys configuration"
    echo "        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys"
    echo "        Update keys located in ./ROT_Provisioning/BootROM/Keys"
    echo "        You can use ./ROT_Provisioning/BootROM/keys_generation.sh to generate new keys"
	  echo "        Press any key to continue..."
    echo
    read -p "" -n1 -s

    echo "    * OEMuROT keys configuration"
    echo "        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys"
    echo "        Update keys located in ./ROT_Provisioning/OEMuROT/Keys"
    echo "        You can use ./ROT_Provisioning/OEMuROT/keys_generation.sh to generate new keys"
    echo "        Press any key to continue..."
    echo
    read -p "" -n1 -s

    echo "    * OEMuROT DA password configuration"
    echo "        Warning: Default password must NOT be used. Make sure to choose your own"
    echo "        Update the password located in ./ROT_Provisioning/OEMuROT/Config/DA_password.bin"
    echo "        Press any key to continue..."
    echo
    read -p "" -n1 -s
}

image_generation()
{
  echo "Step 2 : Images generation"
  echo "    * OEMuROT image generation (BootROM format)"
  echo "        Open the OEMuROT_Boot project with preferred toolchain and rebuild all files."
  echo "        ./ROT/OEMuROT_Boot/Binary/OEMuROT_Boot_Trusted.bin is generated at this step"
  echo "        Press any key to continue..."
  echo
  read -p "" -n1 -s

  if [ "$app_full_secure" == "1" ]; then
    echo "    * Application firmware image generation (MCUboot format^)"
    echo "        Open the OEMuROT_Appli project with preferred toolchain and rebuild all files."
    echo "        ./$oemurot_appli_path_project/Binary/rot_tz_xx_app_enc_sign.bin is generated at this step"
  else
    echo "    * Application firmware image generation (MCUboot format)"
    echo "        Open the OEMuROT_Appli project with preferred toolchain and rebuild all files."
    echo "        Build must be done in this order Secure > NonSecure in case of trust zone application."
    echo "        ./$oemurot_appli_path_project/Binary/rot_tz_xx_app_enc_sign.bin is generated at this step"
  fi
	echo "        Press any key to continue..."
  echo
  read -p "" -n1 -s

  > $provisioning_log
  if [ "$s_data_image_number" = "0x1" ]; then
    echo "    * Data secure image generation"
    echo "        Select OEMuRoT_S_Data_Image.xml(Default path is ./ROT_Provisioning/OEMuROT/Images/OEMuROT_S_Data_Image.xml)"
    echo "        Press any key to continue..."
	  echo
    read -p "" -n1 -s

    action="Generating the ./Binary/s_data_enc_sign.bin image"
		echo "        $action..."
		echo

		"$stm32tpccli" -pb "$s_data_init_xml" >> "$provisioning_log"
    [ $? -ne 0 ] && step_error
    "$stm32tpccli" -pb "$s_data_xml" >> "$provisioning_log"
    [ $? -ne 0 ] && step_error
  fi

  if [ "$ns_data_image_number" = "0x1" ]; then
    echo "    * Data non secure image generation"
    echo "        Select OEMuRoT_NS_Data_Image.xml(Default path is ./ROT_Provisioning/OEMuROT/Images/OEMuROT_NS_Data_Image.xml)"
    echo "        Press any key to continue..."
    echo
    read -p "" -n1 -s

    action="Generating the ./Binary/ns_data_enc_sign.bin image"
		echo "        $action..."
		echo

		"$stm32tpccli" -pb "$ns_data_init_xml" >> "$provisioning_log"
    [ $? -ne 0 ] && step_error
    "$stm32tpccli" -pb "$ns_data_xml" >> "$provisioning_log"
    [ $? -ne 0 ] && step_error
  fi
}

provisioning()
{
  echo "Step 3 : Provisioning"

  echo "    * Boot mode should be set to development mode"
  echo "        (BOOT1 switch position at 1-3)"
  echo "        Press any key to continue..."
  echo
  read -p "" -n1 -s

  echo "    * OTP programming"
  echo "        Warning : The OTP programming must be done only once."
  echo "        The OTP configuration data are :"
	echo "             - BootROM keys"
  echo "             - OEMuROT keys"
  echo "             - OEMuROT DA password hash"
  echo "        The following scripts can be used to program those configuration data :"
  echo "             - ./ROT_Provisioning/BootROM/keys_programming.sh"
  echo "             - ./ROT_Provisioning/OEMuROT/keys_programming.sh"
  echo "             - ./ROT_Provisioning/OEMuROT/da_programming.sh"
	echo
  echo "        Press any key to continue..."
  echo
  read -p "" -n1 -s

  action="Flashing the images"
	echo "    * $action..."

	(source "$flash_programming") >> $provisioning_log
  [ $? -ne 0 ] && step_error

  echo "        Successful images flashing"
  echo "        (see $provisioning_log for details)"
  echo
}

final_step()
{
  echo "    * Boot mode should be set to flash mode"
  echo "        (BOOT1 switch position at 1-2 and BOOT0 switch position at 1-2)"
  echo "        Press any key to continue..."
  echo
  read -p "" -n1 -s

  echo "====="
  echo "===== The board is correctly configured."
  echo "===== Connect UART console (115200 baudrate) and press Reset to start application."
  echo "====="

  # Keep the window open
  exec "$SHELL"
}

step_error()
{
  echo
  echo "====="
  echo "===== Error while $action."
  echo "===== See $provisioning_log for details. Then try again."
  echo "====="
  exec "$SHELL"
  exit 1
}

prov "$@"