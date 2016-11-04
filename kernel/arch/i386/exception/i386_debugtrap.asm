; i386_debugtrap.asm - Catch debugging breakpoints
; Notifies user and returns
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_debugtrap]
i386_debugtrap:

	pushad

	push str_DebugTrap
	push dword [int_ExceptionNumber]
	push str_SystemException

	call terminal_printf
	add esp,12

	popad
	iretd

[section .data]

str_DebugTrap 		db "Debug Trap",0
int_ExceptionNumber dd 0x01
