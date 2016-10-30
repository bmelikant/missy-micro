;-------------------------------------------------------------------------------------;
;	syscall.asm: Provide a mechanism to call the operating system from userspace
;	System calls are provided on interrupt 0x80 (same as Linux kernel)
;
;	Ben Melikant, 10/2016
;-------------------------------------------------------------------------------------;

[bits 32]
[section .text]
align 4

[global system_call]
[extern syscall_table]

; system_call: Dispatch requests on interrupt 80 (syscall interrupt)
; system call number should be in eax, other arguments as needed
system_call:

	; we should already be in supervisor mode; handle the system call request
	; eax is the index of the system call in the table
	push ebx
	mov ebx,syscall_table

	; multiply the index by four, then add it to ebx
	mov edx,0x04
	mul edx
	add ebx,eax

	; finally, jump to the installed system call!
	call [ebx]
	pop ebx

	iret
