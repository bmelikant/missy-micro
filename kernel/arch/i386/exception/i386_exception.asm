; i386_exception.asm - Functions common to most exceptions
; Ben Melikant, 11/2/16

[bits 32]
[section .text]
align 4

; define the color attribute for exception handling print
%define I386_EXCEPTION_COLOR 0x4F

; external functions, needed by most handlers
[extern terminal_puts]
[extern terminal_clrscr]
[extern terminal_printf]
[extern terminal_setcolor]

[global i386_exception_set_disp]
[global i386_system_stop]
[global str_SystemException]

; i386_exception_set_disp: set the display up to show an exception condition
i386_exception_set_disp:

	push byte I386_EXCEPTION_COLOR
	call terminal_setcolor
	add esp,4						; I don't understand this; should be add esp,1 to fix the stack! *see note at bottom

	call terminal_clrscr
	ret


; i386_system_stop: called to stop the system in the event of irrecoverable error
i386_system_stop:

	; print the first part of our halt message
	push str_SystemMustHalt1
	call terminal_puts

	add esp,0x04

	push str_SystemMustHalt2
	call terminal_puts

	add esp,0x04

	; halt the system!
	cli
	jmp $

[section .data]

str_SystemException db "[System Exception 0x%x]: %s",10,10,0
str_SystemMustHalt1 db 10,"An irrecoverable system exception has occured. Your system has been",0
str_SystemMustHalt2 db "shut down to prevent damage. Please power off and restart your machine.",0

; * Note (for my reference): x86 processors in 32-bit mode will store on the stack at minimum a size of 32 bits
; When a byte is pushed onto the stack, it is actually pushed as a 32 bit value regardless of the qualifier
