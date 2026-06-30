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
  echo ">>> Running ../postbuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../
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
echo "=================================="
echo ">>> Running $0 $@"
echo ""

# Getting ROT provisioning path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`

cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source $provisioningdir/env.sh "$provisioningdir"
cd "$project_dir/../Binary"
binarydir=$(pwd)
cd $project_dir

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
> $current_log_file

# arg1 is the config type (Debug, Release)
config=$1

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
# Environment variable for AppliCfg
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"


error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

encrypted=0
appli_dir=../../../../$oemurot_appli_path_project
binary_file=$binarydir/STM32N6570-DK_OEMuROT_Boot.bin
trusted_binary_file=$binarydir/OEMuROT_Boot_Trusted.bin

bootrom_path=$provisioningdir/BootROM
pbk1=$bootrom_path/Keys/publicKey00.pem
pbk2=$bootrom_path/Keys/publicKey01.pem
pbk3=$bootrom_path/Keys/publicKey02.pem
pbk4=$bootrom_path/Keys/publicKey03.pem
pbk5=$bootrom_path/Keys/publicKey04.pem
pbk6=$bootrom_path/Keys/publicKey05.pem
pbk7=$bootrom_path/Keys/publicKey06.pem
pbk8=$bootrom_path/Keys/publicKey07.pem

pvk=$bootrom_path/Keys/privateKey00.pem

derivval=0x7c098af2
enck=$bootrom_path/Keys/OEM_SECRET.bin
version=0x00000001

scmd="-pubk $pbk1 $pbk2 $pbk3 $pbk4 $pbk5 $pbk6 $pbk7 $pbk8 -prvk $pvk -pwd rot1"
if [ $encrypted = 1 ]; then
  enccmd="-encdc $derivval -enck $enck"
  optionflag="-t fsbl -iv $version -la 0x34180000 -of 0x80000003"
else
  enccmd=
  optionflag="-t fsbl -iv $version -la 0x34180000 -of 0x80000001"
fi

# Variable for AppliCfg
preprocess_bl2_file=$project_dir/image_macros_preprocessed_bl2.c
provisioning=$provisioningdir/OEMuROT/provisioning.sh
flash_programming=$provisioningdir/OEMuROT/flash_programming.sh

appli_flash_layout=$appli_dir/Secure_nsclib/appli_flash_layout.h

appli_s_ld_file=$appli_dir/STM32CubeIDE/AppliSecure/stm32n657xx_s.ld
appli_s_system_file=$appli_dir/AppliSecure/Src/system_stm32n6xx_s.c
appli_s_main_file=$appli_dir/AppliSecure/Src/main.c
appli_ns_ld_file=$appli_dir/STM32CubeIDE/AppliNonSecure/stm32n657xx_ns.ld
appli_ns_system_file=$appli_dir/AppliNonSecure/Src/system_stm32n6xx_ns.c

s_code_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Init_Image.xml
ns_code_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Init_Image.xml
s_data_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Data_Init_Image.xml
ns_data_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Data_Init_Image.xml

s_code_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Image.xml
ns_code_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Image.xml
s_data_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Data_Image.xml
ns_data_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Data_Image.xml

# Image signing
"$stm32signingtoolcli" -bin "$binary_file" $scmd $enccmd $optionflag -o "$trusted_binary_file" -hv 2.3 -s -align >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# Provisioning
$python$applicfg flash --layout "$preprocess_bl2_file" -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER "$provisioning" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER "$provisioning" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# Flash programming
$python$applicfg flash --layout "$preprocess_bl2_file" -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b appli_s_address -m RE_FLASH_AREA_2_ADDRESS "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b data_s_address -m RE_FLASH_AREA_6_ADDRESS "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b appli_ns_address -m RE_FLASH_AREA_3_ADDRESS "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b data_ns_address -m RE_FLASH_AREA_7_ADDRESS "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout "$preprocess_bl2_file" -b last_sector -m RE_FLASH_END_SECTOR -d 0x10000 --decimal "$flash_programming" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# XML files
$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_S_PARTITION_SIZE -c S "$s_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_S_PARTITION_SIZE -c S "$s_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_NS_PARTITION_SIZE -c S "$ns_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_NS_PARTITION_SIZE -c S "$ns_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_S_DATA_PARTITION_SIZE -c S "$s_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_S_DATA_PARTITION_SIZE -c S "$s_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_NS_DATA_PARTITION_SIZE -c S "$ns_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_NS_DATA_PARTITION_SIZE -c S "$ns_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$s_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$s_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$ns_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$ns_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$s_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$s_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$ns_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$ns_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$s_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$s_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$s_code_init_xml" -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$s_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$s_code_xml" -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$s_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$ns_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$ns_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$ns_code_init_xml" -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$ns_code_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$ns_code_xml" -nxml "Firmware download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$ns_code_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$s_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$s_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$s_data_init_xml" -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$s_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$s_data_xml" -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$s_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$ns_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "Number of scratch sectors" --decimal "$ns_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$ns_data_init_xml" -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$ns_data_init_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -xml "$ns_data_xml" -nxml "Data download slot size" -nxml "Number of scratch sectors" --decimal -e "(((val1+1)/val2)+1)" -cond val2 -c M "$ns_data_xml" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# Application secure
$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_APPLI_S_ADDRESS -n CODE_S_ADDRESS "$appli_s_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_FLASH_S_PARTITION_SIZE -n CODE_S_SIZE "$appli_s_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_DATA_S_ADDRESS -n DATA_S_ADDRESS "$appli_s_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_DATA_S_SIZE -n DATA_S_SIZE "$appli_s_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_APPLI_S_ADDRESS -n VECT_TAB_BASE_ADDRESS "$appli_s_system_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_APPLI_NS_ADDRESS -n VTOR_TABLE_NS_START_ADDR "$appli_s_main_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# Application non secure
$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_APPLI_NS_ADDRESS -n CODE_NS_ADDRESS "$appli_ns_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_FLASH_NS_PARTITION_SIZE -n CODE_NS_SIZE "$appli_ns_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_DATA_NS_ADDRESS -n DATA_NS_ADDRESS "$appli_ns_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout "$preprocess_bl2_file" -m RE_DATA_NS_SIZE -n DATA_NS_SIZE "$appli_ns_ld_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_APPLI_NS_ADDRESS -n VECT_TAB_BASE_ADDRESS "$appli_ns_system_file" --vb >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# Application flash layout
# Bypass configuration of appli flash layout file if not present
if [ -f "$appli_flash_layout" ]; then
  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_DOWNLOAD_MENU -n DOWNLOAD_MENU "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_OVERWRITE -n MCUBOOT_OVERWRITE_ONLY "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_S_DATA_IMAGE_NUMBER -n S_DATA_IMAGE_NUMBER "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_NS_DATA_IMAGE_NUMBER -n NS_DATA_IMAGE_NUMBER "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_DATA2_NS_ADDRESS -n NS_DATA_ADDRESS "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_0_OFFSET -n S_APPLI_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_1_OFFSET -n NS_APPLI_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_4_OFFSET -n S_DATA_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_5_OFFSET -n NS_DATA_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_2_OFFSET -n S_APPLI_SECONDARY_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_3_OFFSET -n NS_APPLI_SECONDARY_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_6_OFFSET -n S_DATA_SECONDARY_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_7_OFFSET -n NS_DATA_SECONDARY_OFFSET "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_S_PARTITION_SIZE -n S_APPLI_PARTITION_SIZE "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_NS_PARTITION_SIZE -n NS_APPLI_PARTITION_SIZE "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_6_SIZE -n S_DATA_PARTITION_SIZE "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_7_SIZE -n NS_DATA_PARTITION_SIZE "$appli_flash_layout" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
fi

exit 0
