#!/bin/bash -

# Getting the CubeProgammer_CLI path
source ../env.sh
source img_config.sh

# Data updated with the postbuild of OEMuROT-Boot
bootaddress=0x70000000
appli_s_address=0x700D0000
data_s_address=0x700E0000
appli_ns_address=0x700F0000
data_ns_address=0x70110000
s_data_image_number=0x1
ns_data_image_number=0x1
last_sector=18

signedbootpath=../../$oemurot_boot_path_project/Binary/OEMuROT_Boot_Trusted.bin
appli_s_path=../../$oemurot_appli_path_project/Binary/$oemurot_appli_s
appli_ns_path=../../$oemurot_appli_path_project/Binary/$oemurot_appli_ns
data_s_path=Binary/s_data_init_enc_sign.bin
data_ns_path=Binary/ns_data_init_enc_sign.bin

flash_log=flash_programming.log

# CubeProgrammer connection
connect_reset="-c port=SWD ap=1"

error()
{
  echo "       Error when trying to $action" >> "$flash_log"
  echo "       flash Programming aborted" >> "$flash_log"
  echo "Error when trying to $action"
  echo "Flash Programming aborted"
  echo "See $flash_log for details. Then try again."
  echo
  exit 1
}


# ============================================================ Download images ============================================================

action="Reset the target"
echo $action
"$stm32programmercli" $connect_reset > "$flash_log"
if [ $? -ne 0 ]; then
	error
fi
echo "Reset done"

action="Erase OEMuROT sectors"
echo $action
"$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -e [0 $last_sector] >> "$flash_log"
if [ $? -ne 0 ]; then
	error
fi
echo "OEMuROT sectors erased"

action="Write OEMuROT_Boot"
echo $action
"$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$signedbootpath" $bootaddress -v >> "$flash_log"
if [ $? -ne 0 ]; then
	error
fi
echo "OEMuROT_Boot Written"

echo "Application images programming in download slots"

if [ "$app_full_secure" == "1" ]; then
  action="Write OEMuROT_Appli Secure"
  echo $action
  "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$appli_s_path" $appli_s_address -v >> "$flash_log"
  if [ $? -ne 0 ]; then
    error
  fi
  echo "OEMuROT_Appli Secure Written"
  if [ "$s_data_image_number" == "1" ]; then
    action="Write OEMuROT_Data Secure"
    echo $action
    "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$data_s_path" $data_s_address -v >> "$flash_log"
    if [ $? -ne 0 ]; then
      error
    fi
  fi
else
  echo "OEMuROT_Appli Secure Written"
  action="Write OEMuROT_Appli Secure"
  echo $action
  "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$appli_s_path" $appli_s_address -v >> "$flash_log"
  if [ $? -ne 0 ]; then
    error
  fi
  echo "OEMuROT_Appli Secure Written"

  if [ "$s_data_image_number" == "0x1" ];then
    action="Write OEMuROT Data Secure"
    echo $action
    "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$data_s_path" $data_s_address -v >> "$flash_log"
    if [ $? -ne 0 ]; then
      error
    fi
    echo "OEMuROT Data Secure Written"
  fi

  action="Write OEMuROT_Appli Non-Secure"
  echo $action
  "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$appli_ns_path" $appli_ns_address -v >> "$flash_log"
  if [ $? -ne 0 ]; then
    error
  fi
  echo "OEMuROT_Appli Non-Secure Written"


  if [ "$ns_data_image_number" == "0x1" ]; then
      action="Write OEMuROT Data Non-Secure"
      echo $action
      "$stm32programmercli" $connect_reset -el "$stm32ExtLoaderFlash" -d "$data_ns_path" $data_ns_address -v >> "$flash_log"
      if [ $? -ne 0 ]; then
        error
      fi
      echo "OEMuROT Data Non-Secure Written"
  fi
fi

echo "Programming success"
exit 0
