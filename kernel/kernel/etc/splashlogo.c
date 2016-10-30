/*
 * splashlogo.c: Print the MISSY splash logo (MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 */

#include <kernel/tty.h>

void splash_logo (void) {

        terminal_puts("     __  __________ ______ ________  __                                       ");
        terminal_puts("    /  \\/   /_   _// ____// ____/\\ \\/ / ************************************");
        terminal_puts("   / /\\__/ /  / /  \\___ \\ \\__  \\  \\  / i386 Microcomputer Operating System *");
        terminal_puts("  / /   / /__/ /_ ____/ /____/ /  / /  version 3.0 - Alpha                 *");
        terminal_puts(" /_/   /_/______//_____//_____/  /_/ ***************************************");
}
