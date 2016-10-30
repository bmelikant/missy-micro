;--------------------------------------------------------------------------------;
; i386_fpu_init.asm: MISSY Microsystem floating point unit setup routine
;	Detects and initializes the floating point unit on x86-based systems
;	Detection / initialization code comes from OSDev Wiki FPU tutorial
;
; Created 8/15/16 - Ben Melikant
; Last edited 8/15/16
;
; Changelog: N/A
;--------------------------------------------------------------------------------;

; FPU control register bitmasks

%define CR0_MP_BIT	0x00000002		; cr0 bit 1
%define CR0_EM_BIT	0x00000004		; cr0 bit 2
%define CR0_TS_BIT	0x00000008		; cr0 bit 3
%define CR0_ET_BIT	0x00000010		; cr0 bit 4
%define CR0_NE_BIT	0x00000020		; cr0 bit 5

%define CR4_OSFXSR		0x0000200		; cr4 bit 9
%define CR4_OSXMMEXCEPT		0x0000400	 	; cr4 bit 10
%define CR4_OSXSAVE		0x0040000		; cr4 bit 18

%define I386_FPU_INIT_SUCCESS	0
%define I386_NO_FPU_PRESENT 	1

[section .text]
align 4

[global i386_fpu_init]

i386_fpu_init:

	; preserve the stack for this function call
	push ebp
	mov esp,ebp
	
	; grab the arguments from the stack
	mov edx,[ebp+8]

	mov edx,cr0									; get the current cr0 value
	and edx,(-1) - (CR0_TS_BIT + CR0_EM_BIT)	; use the bitmasks to force an FPU read
	mov cr0,edx				; restore the control word

	fninit					; load fpu default values
	fnstsw [.testword]		; use the FPU to modify the test value

	cmp word [.testword],0		; the fpu should have set this location to 0
	jne .no_fpu					; if the value isn't 0, no FPU is present

	jmp .init_fpu			; otherwise, continue initialization

.no_fpu:

	; restore the stack and return error
	mov ebp,esp
	pop ebp
	mov eax,dword I386_NO_FPU_PRESENT		; return error!!
	ret

.init_fpu:

	; TODO: set up CR0 and CR4 for the FPU
	mov edx,cr0
	and edx,(-1) - (CR0_TS_BIT + CR0_EM_BIT)
	mov cr0,edx

	mov edx,cr4
	

	; set up the control word in the FPU itself
	fldcw [.value_37f]
	fldcw [.value_37e]
	fldcw [.value_37a]

.testword 	dw 0x55aa
.value_37f 	dw 0x37f
.value_37e	dw 0x37e
.value_37a	dw 0x37a
