;-----------------------------------------------------------------------------;
;	interrupt.asm: Define interrupt handlers for MISSY Microsystem v0.1b  ;
;	Written by Ben Melikant 10/2014                                       ;
;-----------------------------------------------------------------------------;

[bits 32]
[section .text]
align 4

; constant definitions

%define X86_EXCEPTION_COLOR_ATTRIB 0x4F

; external functions

[extern terminal_puts]
[extern terminal_clrscr]
[extern terminal_printf]
[extern x86_pic_get_irqreg]
[extern x86_pit_update_count]
[extern x86_pic_send_eoi]

; Interrupt Handlers

;------------------------------------------------------------;
;	x86_default_interrupt: Default interrupt handler     ;
;	Inputs: None                                         ;
;	Returns: None                                        ;
;------------------------------------------------------------;

[global x86_default_interrupt]
x86_default_interrupt:

	; save registers
	pushad

	; print the interrupt message string

	push msgDefaultHandler
	call terminal_puts
	
	; fix the stack

	add esp,0x04

	; return to caller
	popad
	iretd

msgDefaultHandler db "Interrupt called with no resolution; defaulting...",10,0

;------------------------------------------------------------------------;
;	x86_dividebyzero: Interrupt handler for zero division exceptions ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_dividebyzero]
x86_dividebyzero:

	; save all registers

	pushad

	; print a division by zero message

	push msgDivideByZero
	call terminal_puts

	; fix the stack

	add esp,0x04

	popad
	iretd

msgDivideByZero db "[Exception 0x01]: Division by zero is not allowed!",10,0

;------------------------------------------------------------------------;
;	x86_debugtrap: Interrupt handler for debug trap exception        ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_debugtrap]
x86_debugtrap:

	push msgDebugTrap
	call terminal_puts

	cli
	jmp $

	iretd

msgDebugTrap db "System error: [x86_debugtrap] called...",10,0

;------------------------------------------------------------------------;
;	x86_nonmaskable: Interrupt handler for nonmaskable interrupt     ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_nonmaskable]
x86_nonmaskable:

	push msgNonMaskable
	call terminal_puts

	cli
	jmp $

	iretd

msgNonMaskable db "System error: [x86_nonmaskable] called...",10,0

;------------------------------------------------------------------------;
;	x86_breakpoint: Interrupt handler for breakpoints                ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_breakpoint]
x86_breakpoint:

	pushad

	push msgBreakPoint
	call terminal_puts

	popad
	iretd

msgBreakPoint db "The system encountered a breakpoint...",10,0

;------------------------------------------------------------------------;
;	x86_overflow: Interrupt handler for overflow exceptions          ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_overflow]
x86_overflow:

	push msgOverflow
	call terminal_puts

	cli
	jmp $

	iretd

msgOverflow db "System error: [x86_overflow] called...",10,0

;------------------------------------------------------------------------;
;	x86_bounderror: Interrupt handler for boundary fault	         ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_bounderror]
x86_bounderror:

	push msgBoundFault
	call terminal_puts

	cli
	jmp $

	iretd

msgBoundFault db "System error: [x86_bounderror] called...",10,0

;------------------------------------------------------------------------;
;	x86_badopcode: Interrupt handler for invalid opcode exceptions   ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_badopcode]
x86_badopcode:

	push msgInvalidOpcode
	call terminal_puts

	cli
	jmp $

	iretd

msgInvalidOpcode db "System error: [x86_badopcode] called...",10,0

;------------------------------------------------------------------------;
;	x86_deviceerror: Interrupt handler for device error exceptions   ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_deviceerror]
x86_deviceerror:

	push msgDeviceError
	call terminal_puts

	cli
	jmp $

	iretd

msgDeviceError db "System error: [x86_deviceerror] called...",10,0

;------------------------------------------------------------------------;
;	x86_doublefault: Interrupt handler for double fault exceptions   ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_doublefault]
x86_doublefault:

	push msgDoubleFault
	call terminal_puts

	cli
	jmp $

	iretd

msgDoubleFault db "System error: [x86_doublefault] called...",10,0

;------------------------------------------------------------------------;
;	x86_invalidtss: Interrupt handler for invalid tss exceptions     ;
;	Inputs: None                                                     ;
;	Returns: None                                                    ;
;------------------------------------------------------------------------;

[global x86_invalidtss]
x86_invalidtss:

	push msgInvalidTss
	call terminal_puts

	cli
	jmp $

	iretd

msgInvalidTss db "System error: [x86_invalidtss] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_segnotpresent: Interrupt handler for segment not present exceptions   ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_segnotpresent]
x86_segnotpresent:

	push msgSegmentNotPresent
	call terminal_puts

	cli
	jmp $

	iretd

msgSegmentNotPresent db "System error: [x86_segnotpresent] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_stacksegfault: Interrupt handler for stack segment fault exceptions   ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_stacksegfault]
x86_stacksegfault:

	push msgStackSegmentFault
	call terminal_puts

	cli
	jmp $

	iretd

msgStackSegmentFault db "System error: [x86_stacksegfault] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_genprotectfault: Interrupt handler for general protection faults      ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_genprotectfault]
x86_genprotectfault:

	; pop the error code from the stack

	pop dword [_gpf_error]

	; save all registers

	pushad

	; print a general protection fault message

	push msgGeneralProtectionFault
	call terminal_puts

	; fix the stack

	add esp,0x04

	; see what caused the error

	mov eax,dword [_gpf_error]
	and eax,0x00000002
	jnz .idt_descriptor_error

.gdt_descriptor_error:

	; print a message that the error was in a GDT descriptor

	push msgGdtDescriptor
	call terminal_puts

	add esp,0x04
	jmp .print_selector

.idt_descriptor_error:

	; print a message that the error was in an IDT descriptor

	push msgIdtDescriptor
	call terminal_puts

	add esp,0x04

.print_selector:

	popad
	iretd

; exception data

_gpf_error dd 0x00000000

msgGeneralProtectionFault db "System error: [x86_general_protection_fault] has occurred.",10,0
msgGdtDescriptor	  db "The GPF was caused by a GDT descriptor.",10,0
msgIdtDescriptor	  db "The GPF was caused by an IDT descriptor.",10,0

;---------------------------------------------------------------------------------;
;	x86_pagefault: Interrupt handler for page faults                          ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

%define PF_ERROR_PAGE_NOT_PRESENT     0x00000001
%define PF_ERROR_PAGE_RW              0x00000002
%define PF_ERROR_SUPERVISOR_MODE      0x00000004

[extern _x86_swap_page]
[global x86_pagefault]
x86_pagefault:

	; save the stack frame and get the error code

	pop dword [_pf_err_code]

	; save all the registers

	pushad

	; examine the error code. If this was a non-present page,
	; we can see if the page is present in swap storage

	mov eax,dword [_pf_err_code]
	and eax,PF_ERROR_PAGE_NOT_PRESENT

	; if bit 1 is clear, page is not present
	; otherwise, a protection error occured
	
	jnz .pf_protection

.pf_notpresent:

	; load CR2 into EAX and shift the value right to remove the present flag

	mov eax,cr2
	shr eax,1

	; if the entry is zero, the page just doesn't exist

	or eax,eax
	jz .page_nonexistant

	; otherwise, at this point we would load the page from the disk

	; TODO: handle load page from disk here!

	push msgPageFault
	call terminal_puts
	add esp,0x04

	push msgPageFaultNotPresent
	call terminal_puts
	add esp,0x04

	mov eax,cr2
	push eax
	push msgPageFaultNPAddress
	call terminal_printf
	add esp,0x08

	cli
	jmp $

.pf_protection:

	push byte X86_EXCEPTION_COLOR_ATTRIB
	call terminal_clrscr
	add esp,0x01

	push msgPageFault
	call terminal_puts
	add esp,0x04

	push msgPageProtectError
	call terminal_puts
	add esp,0x04

	; see what the privilege level of the fault was

	mov eax,dword [_pf_err_code]
	and eax,PF_ERROR_SUPERVISOR_MODE
	jnz .from_user_mode

	; notify that this was a supervisor-mode error

	push msgPagePrivilegeKernel
	call terminal_puts
	add esp,0x04

	jmp .check_readwrite

.from_user_mode:

	push msgPagePrivilegeUser
	call terminal_puts
	add esp,0x04

.check_readwrite:

	; see whether a read or write caused the error

	mov eax,dword [_pf_err_code]
	and eax,PF_ERROR_PAGE_RW
	jnz .on_write

	push msgPageAccessRead
	call terminal_puts
	add esp,0x04

	jmp .stop

.on_write:

	push msgPageAccessWrite
	call terminal_puts
	add esp,0x04

.stop:

	cli
	jmp $

.page_nonexistant:

	push msgPageNonexistant
	call terminal_puts
	add esp,0x04

	cli
	jmp $

.fault_done:

	; restore the registers and continue

	popad
	iretd

msgPageFault           db 10,"x86 system error: This machine has encountered a page fault.",0
msgPageFaultNotPresent db "The requested page is not present in system memory.",0
msgPageFaultNPAddress  db "Linear address of page fault: 0x%x",0
msgPageProtectError    db "A page protection error was encountered.",0
msgPageNonexistant     db "The requested page does not exist.",0
msgPageAccessRead      db "An attempt was made to read the requested page.",0
msgPageAccessWrite     db "An attempt was made to write the requested page.",0
msgPagePrivilegeUser   db "The attempt was made from user code.",0
msgPagePrivilegeKernel db "The attempt was made from kernel code.",0

_pf_err_code dd 0x00000000

;---------------------------------------------------------------------------------;
;	x86_fpuexception: Interrupt handler for fpu exceptions                    ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_fpuexception]
x86_fpuexception:

	push msgFPUException
	call terminal_puts

	cli
	jmp $

	iretd

msgFPUException db "System error: [x86_fpuexception] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_aligncheck: Interrupt handler for alignment check exceptions          ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_aligncheck]
x86_aligncheck:

	push msgAlignmentCheck
	call terminal_puts

	cli
	jmp $

	iretd

msgAlignmentCheck db "System error: [x86_aligncheck] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_machinecheck: Interrupt handler for machine check exceptions          ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_machinecheck]
x86_machinecheck:

	push msgMachineCheck
	call terminal_puts

	cli
	jmp $

	iretd

msgMachineCheck db "System error: [x86_machinecheck] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_simdexception: Interrupt handler for SIMD FPU exceptions              ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_simdexception]
x86_simdexception:

	push msgSimdException
	call terminal_puts

	cli
	jmp $

	iretd

msgSimdException db "System error: [x86_simdexception] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_virtualizeexception: Interrupt handler for virtualization exceptions  ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_virtualizeexception]
x86_virtualizeexception:

	push msgVirtualizeException
	call terminal_puts

	cli
	jmp $

	iretd

msgVirtualizeException db "System error: [x86_virtualizeexception] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_securityexception: Interrupt handler for security exceptions          ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_securityexception]
x86_securityexception:

	push msgSecurityException
	call terminal_puts

	cli
	jmp $

	iretd

msgSecurityException db "System error: [x86_securityexception] called...",10,0

;---------------------------------------------------------------------------------;
;	x86_testcode: Interrupt handler for testing other interrupt handlers      ;
;	Inputs: None                                                              ;
;	Returns: None                                                             ;
;---------------------------------------------------------------------------------;

[global x86_testcode]
x86_testcode:

	push msgTestCode
	call terminal_puts

	cli
	jmp $

	iretd

msgTestCode db "System error: [x86_testcode] called...",10,0
