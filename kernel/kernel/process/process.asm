;----------------------------------------------------------------------------
;
;   process.asm: i386 assembler helper functions for process scheduling
;   Ben Melikant, 26-Sept-2017
;
;----------------------------------------------------------------------------

bits 32
align 4

%define PROCESS_ID_INVALID 0x0

%define _offset_pid         0
%define _offset_entrypt     4
%define _offset_pdirectory  8
%define _offset_stack_addr  12
%define _offset_running     16

section .text

; export these functions into the global namespace
[global kickoff_process]
[global switch_process]
[global install_stack]

;------------------------------------------------------------------------
;   kickoff_process: Start the given process
;   inputs: proc_data - ptr to the process data structure to execute
;   returns: the process ID on success, PROCESS_ID_INVALID on failure
;------------------------------------------------------------------------
kickoff_process:

    ; set up a new stack frame
    push ebp
    mov ebp,esp

    ; save the processor state onto the system stack
    pusha
    
    ; grab the stack ptr and the entry point from the stack
    mov ebx,dword [ebp+4]
    mov esp,dword [ebp+8]

    ; the stack context has changed - call the usermode program!
    call ebx
    
    ; reset the stack to pre-call condition
    mov esp,ebp
    pop ebp

    ret
