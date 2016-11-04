; i386_defaulthandler.asm - Installed as the default interrupt
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_defaulthandler]
i386_defaulthandler:

	pushad

	push str_InterruptDefault
	call terminal_puts
	add esp,4

	popad
	iretd

[section .data]

str_InterruptDefault db "i386_defaulthandler called...",0
