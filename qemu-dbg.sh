#!/bin/bash
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -gdb tcp::1234 -cdrom missy.iso