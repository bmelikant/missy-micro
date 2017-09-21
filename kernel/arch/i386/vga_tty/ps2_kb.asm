;----------------------------------------------------------------------------------------
;   ps2_kb.asm: Device control assembly for PS/2 keyboard controller (interrupt handler)
;   Ben Melikant, 20-Aug-2017
;----------------------------------------------------------------------------------------

%include "arch/i386/exception/exception.inc"

[bits 32]
[section .text]

[extern ps2_keyboard_enqueue]
[global keyb_interrupt]
keyb_interrupt:

    cli
    pushad

    ; grab the keypress and put it in the queue
    xor eax,eax
    in al,0x60

    push eax
    call ps2_keyboard_enqueue
    add esp,4

    mov al,0x20
    out 0x20,al

    popad
    sti
    iret
