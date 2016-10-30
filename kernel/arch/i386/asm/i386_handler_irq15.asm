;--------------------------------------------------------------------;
;	x86_handler_irq7.asm: Interrupt service routine for IRQ 7    ;
;	Ben Melikant 10/2014                                         ;
;	Revision Log:                                                ;
;	- 10/22/14: Removed diagnostic prints from IRQ handler       ;
;--------------------------------------------------------------------;

section .text
align 4
bits 32

%define X86_PIC1_PORT_CMD	0x20
%define X86_PIC2_PORT_DATA	0x21
%define X86_PIC2_PORT_CMD	0xa0
%define X86_PIC2_PORT_DATA 	0xa1
%define X86_PIC1_MASK_IRQ7	0x80
%define X86_PIC2_MASK_IRQ15	0x80
%define X86_PIC_COMMAND_EOI	0x20

[extern spurious]
[extern debug_puts]
[global i386_handler_irq15]

i386_handler_irq15:

	; save all registers

	pushad

	; read port 0xA0 for in-service register

	mov dx,X86_PIC2_PORT_DATA
	in al,dx
	and al,X86_PIC2_MASK_IRQ15

	jz .spurious

	; this interrupt was not spurious, send EOI to both PICs

	mov al,X86_PIC_COMMAND_EOI
	out X86_PIC1_PORT_CMD,al
	out X86_PIC2_PORT_CMD,al
	jmp .interrupt_done

.spurious:

	; increment our spurious count

	inc dword [spurious]

	; we must still notify the master PIC that the interrupt is over

	mov al,X86_PIC_COMMAND_EOI
	out X86_PIC1_PORT_CMD,al

.interrupt_done:

	; restore the registers and return

	popad
	iretd
