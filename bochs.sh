#!/bin/bash
set -e
. ./iso.sh

# valid command line parameters:
# -mem 'value': Set the size of the memory for bochs

memory_sz="memory: guest=512, host=256"

if [ "$1" != "" ]; then
	if [ "$1" == "-mem" ]; then
		if [ "$2" != "" ]; then
			memory_sz="memory: guest=$2, host=512"
		else
			echo "Usage: -mem 'sz'"
		fi
	else
		echo "Unrecognized command line option $1"
	fi
fi

bochs "boot:cdrom" "ata0-master: type=cdrom, path=missy.iso, status=inserted" "$memory_sz"