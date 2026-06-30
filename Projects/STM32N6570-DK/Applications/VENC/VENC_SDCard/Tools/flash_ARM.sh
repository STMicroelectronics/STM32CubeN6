export PATH_PROG="/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
export PATH=$PATH_PROG:$PATH
LOADER_EXT="-el $PATH_PROG/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"
export OFFSET_IMAGE=0x70100000
export OFFSET_FSBL=0x70000000

SIGN_ONLY=0
if [ "$1" == "--sign-only" ] || [ "$1" == "--dry-run" ]; then
	SIGN_ONLY=1
	echo "Sign-only mode enabled: flashing steps will be skipped."
fi

# MDK-ARM output root path
export APth="../MDK-ARM"

function checkKey 
{
	for i in "${listKey[@]}"
	do
		if [ "$i" == "$1" ] ; then
			echo "1"
			return
		fi
	done	
	echo "0"

}

function waitType
{
	val_waitType=""
	IFS=':' read -ra listKey <<< "$1"
	string=""
	while [ $(checkKey "$string") == "0" ]
	
	do
		if [ "$string" == "quit" ] ; then
			exit 
		fi
	
		if [ "$3" != "" ] ;
		then
			read -t $2 -p "$2  or wait $3 seconds > "  string
			if [ "$?" -gt "128" ] ;
			then 
			 string=$1
			 echo -n $string
			fi
		else
			read   -p "$2"  string
			if [ "$string" == "" ]; then
				if [ "$4" != "" ]; then
					string=$4
				fi
			fi
			 
		fi
	done
   export val_waitType=$string
}
 
if [ ! -d "tmp" ]
then
	waitType "yes:no" "Create tmp dir ? (Yes/no)> " "" "yes"
	mkdir "tmp"
else
	waitType "yes:no" "Cleanup tmp dir ? (Yes/no)> " "" "yes"
    chmod -R a+w tmp
	rm -f tmp/* 
fi

echo "Boot switches must be in 1-3"

APP_NAME="VENC_RTSP"

BIN_PATH="$APth/Appli/VENC_SDCard_LRUN/Project.bin"
FSBL_PATH="$APth/FSBL/VENC_SDCard_FSBL/Project.bin"

if [ ! -f "$BIN_PATH" ]; then
	echo "Application binary not found: $BIN_PATH"
	echo "Build MDK-ARM application target first."
	exit 1
fi

if [ ! -f "$FSBL_PATH" ]; then
	echo "FSBL binary not found: $FSBL_PATH"
	echo "Build MDK-ARM FSBL target first."
	exit 1
fi

waitType "yes:no" "Ready to sign ? (Yes/no)> " "" "yes"
echo Signing .....
STM32_SigningTool_CLI.exe --version

STM32_SigningTool_CLI.exe  -s  -bin "$BIN_PATH"  -nk -of 0x80000000 -t fsbl -o tmp/firmware-trusted.bin -hv 2.3 -align -dump tmp/firmware-trusted.bin
STM32_SigningTool_CLI.exe  -s  -bin "$FSBL_PATH" -nk -of 0x80000000 -t fsbl -o tmp/FSBL-trusted.bin     -hv 2.3 -align -dump tmp/FSBL-trusted.bin

if [ "$SIGN_ONLY" == "1" ]; then
	echo "Sign-only mode: skipping bootloader/application flashing."
else
	waitType "yes:no" "Do you want to flash bootloader? (Yes/no)> " "" "yes"
	if [ "$val_waitType" == "yes" ]; then
		echo Flashing bootloader .....
		STM32_Programmer_CLI.exe -c port=swd -hardRst $LOADER_EXT   -d  tmp/FSBL-trusted.bin $OFFSET_FSBL
	fi

	waitType "yes:no" "Do you want to flash application? (Yes/no)> " "" "yes"
	if [ "$val_waitType" == "yes" ]; then
		echo Flashing application .....
		STM32_Programmer_CLI.exe -c port=swd -hardRst $LOADER_EXT   -d  tmp/firmware-trusted.bin $OFFSET_IMAGE
	fi
fi

read
