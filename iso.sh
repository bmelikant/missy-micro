#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/missy_os isodir/boot/missy_os
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "MISSY Microsystem" {
	multiboot /boot/missy_os
}
menuentry "MISSY Microsystem (multiboot2)" {
	multiboot2 /boot/missy_os
}
EOF
grub-mkrescue -o missy.iso isodir
