#!/bin/bash

set -e

uname_S="$(uname -s)"
if [[ "$uname_S" == "Darwin" ]]; then
	sysctl -n machdep.cpu.brand_string
elif [[ "$uname_S" == "Linux" ]]; then
	lscpu -J | jq -r '.lscpu.[] | select(.field == "Model name:") | .data'
elif [[ "$uname_S" =~ CYGWIN.* ]] || [[ "$uname_S" =~ MINGW.* ]]; then
	powershell.exe -Command "(Get-ItemProperty -Path HKLM:HARDWARE\DESCRIPTION\System\CentralProcessor\0 -Name ProcessorNameString).ProcessorNameString" | sed -r 's/^\s+//;s/\s+$//g'
else
	echo "Unknown"
fi
