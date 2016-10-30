;----------------------------------------------------------------------------;
;	i386_cpu_save_state: Save the current state of the CPU into a structure
;	MISSY Microsystem Alpha 3.0
;
;	Created 8/18/16, Ben Melikant
;----------------------------------------------------------------------------;

[bits 32]
[section .text]
align 4

[global i386_cpu_save_state]
i386_cpu_save_state:

	; save the caller's stack frame
	push ebp
	mov esp,ebp



	; restore the caller's stack frame
	mov ebp,esp
	pop ebp

	; return to caller
	ret
