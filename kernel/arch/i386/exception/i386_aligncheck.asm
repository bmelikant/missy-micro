; i386_aligncheck.asm - Handle unaligned address access
; occurs when the align bit in CR0 is set and an unaligned access is attempted
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_aligncheck]
i386_aligncheck:

	pushad

	push str_AlignmentCheck
	push dword [int_ErrorNumber]
	push str_SystemException

	call terminal_printf
	add esp,12

	popad
	iretd

[section .data]

str_AlignmentCheck 	db "Alignment check: Unaligned address was accessed in user mode",0
int_ErrorNumber		dd 0x11
