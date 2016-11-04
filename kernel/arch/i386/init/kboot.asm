;-----------------------------------------------------------------------------------------;
;	kboot.asm: Version two of the kernel bootstrap (forms jump point for GRUB)        ;
;	Written by Ben Melikant 11/2014                                                   ;
;	This bootstrap sets up a basic paging directory table for the kernel so that our  ;
;	jump to the kernel does not result in triple-fault.                               ;
;-----------------------------------------------------------------------------------------;

bits 32

; paging identification info

%define PAGE_DIRECTORY_ADDR 	0x7F000
%define PAGE_TABLE_0_ADDR   	0x80000
%define PAGE_TABLE_768_ADDR 	0x81000
%define PAGE_TABLE_STACK_ADDR	0x82000

%define PAGE_TABLE_ENTRIES 	1024
%define	PAGE_SIZE 			4096
%define PAGE_PRIVILEGE     	3

; we want to place the stack in memory right below the page directory mapping
%define KERNEL_STACK_PAGE			0xffbff000
%define KERNEL_STACK_ADDR			0xffbfffff
%define KERNEL_STACK_PAGE_INDEX		(KERNEL_STACK_PAGE>>22)

; multiboot information definitions

MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM     equ -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_HEADER_FLAGS)

; place the multiboot header in it's own section

section .multiboot
align 4
multiboot_hdr:

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

; switch to the .startup section

[section .startup]
[global start]

; memory location to preserve the multiboot pointer

multiboot_ptr dd 0x00000000

; global descriptor table (to override GRUB's table)

gdt_start:

	dd 0,0
	db 0xff,0xff,0x00,0x00,0x00,10011010b,11001111b,0x00
	db 0xff,0xff,0x00,0x00,0x00,10010010b,11001111b,0x00

gdt_table:

	dw gdt_table-gdt_start-1
	dd gdt_start

;------------------------------------------------------------------------------------------;
;	start: Load into the operating system proper by setting up a page directory table  ;
;	and enabling paging. The OS is loaded at 0x10000 physical, 0xC0000000 virtual.     ;
;	If the OS ever returns, we have an issue.                                          ;
;------------------------------------------------------------------------------------------;

start:

	; load a new global descriptor table (override GRUB's table)

	lgdt [gdt_table]
	jmp 0x08:next

next:

	; fix our segments

	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	
	mov ss,ax
	mov esp,0x90000

	; jump to the actual start code, below the paging install routine

	mov dword [multiboot_ptr],ebx
	jmp start_system

;---------------------------------------------------------;
;	setup_paging: Enable paging before executing OS   ;
;	Inputs: None, Returns: None                       ;
;---------------------------------------------------------;

[extern stack_physical]
setup_paging:

	; save the registers

	pusha

	; identity-map addresses 0x00000000 - 0x00400000

	mov eax,PAGE_TABLE_0_ADDR
	mov ebx,0x0 | PAGE_PRIVILEGE
	mov ecx,PAGE_TABLE_ENTRIES

.identity_loop:

	mov dword [eax],ebx		; put the address entry into the table
	add eax,4				; queue next page entry
	add ebx,4096			; each entry is 4Kb

	loop .identity_loop		; set up next entry

	; next set up the address for the kernel page table

	mov eax,PAGE_TABLE_768_ADDR
	mov ebx,0x100000 | PAGE_PRIVILEGE
	mov ecx,PAGE_TABLE_ENTRIES

.kernel_loop:

	mov dword [eax],ebx		; same as above
	add eax,4				; same as above
	add ebx,4096			; same as above

	loop .kernel_loop		; same as above

	; create all the stack pages

	mov eax,PAGE_TABLE_STACK_ADDR
	mov ebx,stack_physical
	or ebx,PAGE_PRIVILEGE
	mov ecx,PAGE_TABLE_ENTRIES

.stack_loop:

	mov dword [eax],ebx
	add eax,4
	add ebx,4096

	loop .stack_loop

.already_mapped:

	; clear the page directory

	mov eax,PAGE_DIRECTORY_ADDR
	mov ebx,0x00
	mov ecx,PAGE_TABLE_ENTRIES

.clear_dir:

	mov dword [eax],ebx
	add eax,4

	loop .clear_dir
	
	; install the tables into the directory

	mov eax,PAGE_TABLE_0_ADDR | PAGE_PRIVILEGE
	mov dword [PAGE_DIRECTORY_ADDR],eax

	mov eax,PAGE_TABLE_768_ADDR | PAGE_PRIVILEGE
	mov dword [PAGE_DIRECTORY_ADDR+(768*4)],eax

	; install the stack page tables into the
	mov eax,PAGE_TABLE_STACK_ADDR | PAGE_PRIVILEGE
	mov dword [PAGE_DIRECTORY_ADDR+(KERNEL_STACK_PAGE_INDEX*4)],eax

	; map the last entry of the page directory to itself

	mov eax,PAGE_DIRECTORY_ADDR | PAGE_PRIVILEGE
	mov dword [PAGE_DIRECTORY_ADDR+(1023*4)],eax

	; install the page directory into cr3 (pdbr)

	mov eax,PAGE_DIRECTORY_ADDR
	mov cr3,eax

	popa
	ret

start_system:

	; first set up the page directory table and page tables

	call setup_paging
	push dword [multiboot_ptr]

	; now enable the paging mechanism

	mov eax,cr0
	or eax,0x80000000
	mov cr0,eax

	jmp paging_code

[section .text]
[extern kernel_early_init]
[extern kernel_main]
[extern _init]

paging_code:

; paging is enabled. All addresses must now be translated as virtual
; switch to the .text section (regular system code, 0xC0000000 virtual)

	; restore the multiboot ptr

	pop ebx

	; set up a new stack frame
	mov ebp,KERNEL_STACK_ADDR
	mov esp,ebp

	; kernel early initialization functiom
	push ebx
	call kernel_early_init

	cmp eax,0
	jl .early_init_error

	; TODO: if success, initialize the C runtime
	call _init

	; now all we need to do is call the kernel loadpoint

	mov esp,KERNEL_STACK_ADDR
	push ebx
	call kernel_main

	; a safeguard. The code should never reach this point

	jmp $

.early_init_error:

	cli
	jmp $

[section .system_stack]

resb 4096
[global system_stack]
system_stack:

	; our kernel stack begins here

[section .bss]
[global errno]
errno: resd 0x01
