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
	init_mouse();
	keyboard_install();
	
	__asm__ __volatile__ ("sti");

//	unsigned int p = (int)&_ap_start;
//
//	char hex[8];
//	hex_to_char(p, hex);
//	printf("ap_start: 0x%s\n", hex);
//	anykey();

	showMemory();


	init_cpu();
	printf("\n\nOK echt");

	init_vbe();


//	_main_ap();
	
	for (;;); // or halt
}
