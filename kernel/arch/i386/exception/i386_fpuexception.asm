; i386_fpuexception.asm - Handle exceptions with the x87 floating point unit
; can happen when the unit is not present or has not been initialized
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_fpuexception]
i386_fpuexception:

	pushad

	push str_FPUException
	push dword [int_ErrorNumber]
	push str_SystemException

	call terminal_printf
	add esp,12

	popad
	iretd

[section .data]

str_FPUException 	db "Floating point unit exception",0
int_ErrorNumber		dd 0x10
