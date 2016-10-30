#
# MISSY Microsystem kernel makefile
# Outline taken from OSDev.org Meaty Skeleton project, modified to meet the needs
# of MISSY Microsystem Alpha 3.0 build
#
# Ben Melikant, 8/2016
#

HOST?=(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_missy_kernel -DDEBUG_BUILD -D__build_i386 -Iinclude -Iarch -Ikernel
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL_OBJS:=\
kernel/memory/kalloc.o \
kernel/memory/memory.o \
kernel/process/process.o \
kernel/process/syscall.o \
kernel/etc/splashlogo.o \
kernel/kernel.o

OBJS:=\
$(KERNEL_ARCH_OBJS) \
$(KERNEL_OBJS)

CRTI_OBJ:=$(ARCHDIR)/asm/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/asm/crtn.o

ALL_MY_OBJS:=\
$(CRTI_OBJ) \
$(OBJS) \
$(CRTN_OBJ)

OBJ_LINK_LIST:=\
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(OBJS) \
$(CRTEND_OBJ) \
$(CRTN_OBJ)

all: missy_os

.PHONY: all clean install install-headers install-kernel

missy_os: $(OBJ_LINK_LIST) $(ARCHDIR)/link.ld
	$(CC) -T $(ARCHDIR)/link.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.asm
	nasm $< -o $@ -felf

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f missy_os $(OBJS) $(ALL_OUR_OBJS) *.o */*.o */*/*.o

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)

install-kernel: missy_os
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp missy_os $(DESTDIR)$(BOOTDIR)
