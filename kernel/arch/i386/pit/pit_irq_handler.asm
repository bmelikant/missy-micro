;----------------------------------------------------------------
;
; pit_irq_handler.asm: Handle IRQs on IRQ0 (int 0x20) for
; 8253-compatible programmable interval timer
;
;----------------------------------------------------------------

[bits 32]
[section .text]
align 4;

[extern pit_tick_count]
[global pit_irq_handler]
pit_irq_handler:

	pushad

	inc dword [pit_tick_count]

	; send EOI command
	mov al,0x20
	out 0x20,al

	popad
	iretd
