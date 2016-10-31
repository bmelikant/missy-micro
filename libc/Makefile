#
# MISSY Microsystem LIBC makefile
# Outline taken from OSDev.org Meaty Skeleton project, modified to meet the needs
# of MISSY Microsystem Alpha 3.0 build
#
# Ben Melikant, 8/2016
#

HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra -fbuiltin
CPPFLAGS:=$(CPPFLAGS) -D__is_missy_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_missy_kernel -D__arch_i386 -I../kernel/include -I../kernel/arch -I../kernel/kernel

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

STRINGHDROBJS:=\
string/memcmp.o \
string/memcpy.o \
string/memset.o \
string/strlen.o \
string/strcmp.o \
string/strcpy.o

STDIOHDROBJS:=\
stdio/putchar.o \
stdio/puts.o \
stdio/printf.o

STDLIBHDROBJS:= \
stdlib/malloc.o \
stdlib/itoa_s.o \
stdlib/strtol.o

FREEOBJS:=\
$(ARCH_FREEOBJS) \
$(STRINGHDROBJS) \
$(STDIOHDROBJS) \
$(STDLIBHDROBJS) \
ctype/ctype.o

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS)

OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS)

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)

BINARIES=libk.a

all: $(BINARIES)

.PHONY: all clean install install-headers install-libs

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.asm
	$(AS) $< -o $@ -felf

%.libk.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.S
	$(AS) $< -o $@ -felf

clean:
	rm -f $(BINARIES) $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)