;-------------------------------------------------------------------------------;
;	x86_handler_irq7.asm: Interrupt service routine for IRQ 7    		;
;	Ben Melikant 10/2014                                         		;
;	Revision Log:                                                		;
;		- 10/22/14: Stripped diagnostic print messages from IRQ Handler ;
;-------------------------------------------------------------------------------;

section .text
align 4
bits 32

%define X86_PIC1_PORT_CMD	0x20
%define X86_PIC1_PORT_DATA	0x21
%define X86_PIC2_PORT_CMD	0xa0
%define X86_PIC2_PORT_DATA 	0xa1
%define X86_PIC1_MASK_IRQ7	0x80
%define X86_PIC2_MASK_IRQ15	0x80
%define X86_PIC_COMMAND_EOI	0x20

[extern debug_puts]
[extern spurious]
[global i386_handler_irq7]

i386_handler_irq7:

	; save our registers

	pushad

	; check for a spurious interrupt

	mov dx,X86_PIC1_PORT_DATA
	in al,dx
	and al,X86_PIC1_MASK_IRQ7

	jz .spurious

	; not spurious, send the EOI command to the PIC

	mov al,X86_PIC_COMMAND_EOI
	out X86_PIC1_PORT_CMD,al
	jmp .interrupt_done

.spurious:

	; increment our spurious count

	inc dword [spurious]

.interrupt_done:

	; restore registers and exit handler

	popad
	iretd
