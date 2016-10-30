#!/bin/bash
set -e
. ./headers.sh

if [ "$1" != "" ]; then
	if [ "$1" == "clean" ]; then

		for PROJECT in $PROJECTS; do
			$MAKE -C $PROJECT clean
		done

		rm -rvf sysroot
		rm -rvf isodir
		rm -rvf missy.iso

	elif [ "$1" == "all" ]; then
		for PROJECT in $PROJECTS; do
			DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install
		done
	fi
else
	for PROJECT in $PROJECTS; do
		DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install
	done
fi