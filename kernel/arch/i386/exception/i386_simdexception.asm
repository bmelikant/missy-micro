; i386_simdexception.asm
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_simdexception]
i386_simdexception:

	pushad

	call i386_exception_set_disp

	push str_SIMDException
	push dword [int_ErrorNumber]
	push str_SystemException

	call terminal_printf
	add esp,12

	call i386_system_stop

	popad
	iretd

[section .data]

str_SIMDException 	db "SIMD Floating Point Exception",0
int_ErrorNumber		dd 0x13
