/* bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: Main.c: C code entry.
 *
 *  Notes: No warranty expressed or implied. Use at own risk. */
#include <system.h>
#include <multiboot.h>




void _main(multiboot_info_t *mbt, unsigned long magic)
{
	
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	init_video();
	timer_install();
//init_mouse();
	keyboard_install();
	

	__asm__ __volatile__ ("sti");

	listAllTables();
//		testHPET();
	anykey();


	showMemory();


//	init_cpu();
	printf("\n\nOK echt");

//	init_vbe();


	
	for (;;); // or halt
}
