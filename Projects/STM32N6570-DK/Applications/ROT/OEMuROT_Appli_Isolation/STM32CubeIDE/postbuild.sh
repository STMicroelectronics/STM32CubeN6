#!/bin/bash -
#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)

echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo "=================================="
  echo "HOST OS : Windows detected"
  echo ""
  echo ">>> Running ../../postbuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../../
  cmd.exe /C postbuild.bat $@
  # Return OK if no error detected during .bat script
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
# Environment variable for log file
current_log_file=postbuild.log

# Check if Python is installed
if ! python3 --version > /dev/null 2>&1; then
  if ! python --version > /dev/null 2>&1; then
    echo "Python installation missing. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md"
    step_error;
  fi
  python="python "
else
  python="python3 "
fi

echo "=================================="
echo ">>> Running $0 $@"
echo ""

# arg1 is the security type (nonsecure, secure)
signing=$1
# arg2 is the config type (Debug, Release)
config=$2

# Getting ROT provisioning path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd "$project_dir"
source "$provisioningdir/env.sh" $provisioningdir
#
error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}
#=================================================================================================

# Environment variable for AppliCfg
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"

#
#=================================================================================================
#image xml configuration files
#=================================================================================================
s_app_init_xml_field=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Init_Image.xml
ns_app_init_xml_field=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Init_Image.xml

s_app_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Image.xml
ns_app_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Image.xml

#=================================================================================================
#Variables for image xml configuration(ROT_Provisioning/%bootpath%/Images)
#relative path from ROT_Provisioning/%bootpath%/Images directory to retrieve binary files
#=================================================================================================
bin_path_xml_field="../../../Applications/ROT/OEMuROT_Appli_Isolation/Binary"
s_app_bin_xml_field=$bin_path_xml_field/rot_tz_s_app.bin
s_app_init_sign_bin_xml_field=$bin_path_xml_field/rot_tz_s_app_init_enc_sign.bin
s_app_sign_bin_xml_field=$bin_path_xml_field/rot_tz_s_app_enc_sign.bin

ns_app_bin_xml_field=$bin_path_xml_field/rot_tz_ns_app.bin
ns_app_init_sign_bin_xml_field=$bin_path_xml_field/rot_tz_ns_app_init_enc_sign.bin
ns_app_sign_bin_xml_field=$bin_path_xml_field/rot_tz_ns_app_enc_sign.bin

if  [ $signing == "secure" ]; then
  echo "Creating secure signed image"  >> "$current_log_file"

  # update xml file : input file
  $python$applicfg xmlval -v "$s_app_bin_xml_field" --string -n "Firmware binary input file" "$s_app_init_xml_field" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : input file
  $python$applicfg xmlval -v "$s_app_bin_xml_field" --string -n "Firmware binary input file" "$s_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : output file
  $python$applicfg xmlval -v "$s_app_init_sign_bin_xml_field" --string -n "Image output file" "$s_app_init_xml_field" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : output file
  $python$applicfg xmlval -v "$s_app_sign_bin_xml_field" --string -n "Image output file" "$s_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # Creating signed/encrypted image
  "$stm32tpccli" -pb "$s_app_init_xml_field" >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
    "$stm32tpccli" -pb "$s_app_xml" >> "$current_log_file" 2>&1
    if [ $? != 0 ]; then error; fi
fi

if  [ $signing == "nonsecure" ]; then
  echo "Creating non secure signed image"  >> "$current_log_file"

  # Update xml file : input file
  $python$applicfg xmlval -v "$ns_app_bin_xml_field" --string -n "Firmware binary input file" "$ns_app_init_xml_field" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : input file
  $python$applicfg xmlval -v "$ns_app_bin_xml_field" --string -n "Firmware binary input file" "$ns_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : output file
  $python$applicfg xmlval -v "$ns_app_init_sign_bin_xml_field" --string -n "Image output file" "$ns_app_init_xml_field" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : output file
  $python$applicfg xmlval -v "$ns_app_sign_bin_xml_field" --string -n "Image output file" "$ns_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  # Creating signed/encrypted image
  "$stm32tpccli" -pb "$ns_app_init_xml_field" >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
    "$stm32tpccli" -pb "$ns_app_xml" >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
fi

exit 0

#
#
#
#
#
#
#
