
OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS {

	. = 0x7E000;

	.startup  BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.startup)
	}

	. = 0xC0000000;
	kernel_start = 0xC0000000;
	
	.text ALIGN(4096) : AT(ADDR(.text) - 0xbff00000)
	{
		*(.text)
	}
	
	.data ALIGN(4096) : AT(ADDR(.data) - 0xbff00000)
	{
		*(.data)
		*(.rodata*)
	}

	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xbff00000)
	{
		*(COMMON*)
		*(.bss*)
	}
	
	.sysstack ALIGN(4096) : AT(ADDR(.sysstack) - 0xbff00000)
	{
		*(.system_stack)
	}
	
	stack_physical = ADDR(.sysstack) - 0xbff00000;
	stack_blocks = (SIZEOF(.sysstack) / 4096) + 1;
	
	kernel_end = .;
}