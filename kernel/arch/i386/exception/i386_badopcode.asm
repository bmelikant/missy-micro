; i386_badopcode.asm: Handle bad opcode errors
; Called when the CPU detects a bad opcode
; Ben Melikant, 11/2/16

%include "arch/i386/exception/exception.inc"

[global i386_badopcode]
i386_badopcode:

	pushad

	call i386_exception_set_disp

	push str_BadOpcode
	push dword [int_ExceptionNumber]
	push str_SystemException

	call terminal_printf
	add esp,12

	call i386_system_stop

	popad
	iretd

[section .data]

str_BadOpcode 		db "Bad opcode detected",0
int_ExceptionNumber dd 0x06
