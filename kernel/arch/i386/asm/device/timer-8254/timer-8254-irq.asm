;-----------------------------------------------------------------
;	timer-8254-irq.asm: Provide a callback for timer interrupts
;
;	Author: Ben Melikant, 10/30/16
;-----------------------------------------------------------------

[bits 32]
[section .text]
align 4

%define IRQ_TIMER_8254 0x01

; functions needed by the callback
[extern i386_pic_send_eoi]
[extern terminal_printf]

[global timer_8254_irq]
timer_8254_irq:

	pushad

	; increment the timer counter
	inc dword [timer_8254_count]

	; send EOI
	mov al,0x20
	out 0x20,al

	popad
	iretd

[section .data]
[global timer_8254_count]

timer_8254_count dd 0
