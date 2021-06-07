/* bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: Main.c: C code entry.
 *
 *  Notes: No warranty expressed or implied. Use at own risk. */
#include <system.h>
#include <multiboot.h>
#include <smp.h>
#include <apic.h>


extern smp_t smp;

void _main(multiboot_info_t *mbt, unsigned long magic)
{
	initSMP();

	gdt_install();
	idt_install(smp.processorList[0].idt, &smp.processorList[0].idtp);
	isrs_install(smp.processorList[0].idt);
	irq_remap();
	irq_install(smp.processorList[0].idt);
	timer_install();
	init_mouse();
	keyboard_install();
	init_video();


	detect_cpu();
	bool systemReady = check_cpu();
	if (systemReady == FALSE) {

		printf("Can not operate on this system\n");
		return;
	}
	apic_irq_install(smp.processorList[0].idt);
	__asm__ __volatile__ ("sti");
	
	listAllTables();
	//showMemory();



	//init_cpu();

	//if (init_vbe() == FALSE) {
	//	return;
	//}



//	demoVBE();

	
	for (;;); // or halt
}
