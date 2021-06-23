#include <system.h>

struct idt_entry idt; 
struct idt_ptr idtp;

void _main()
{
	gdt_install();
	idt_install(&idt, &idtp);
	isrs_install(&idt);
	irq_remap();
	irq_install(&idt);
	timer_install();
	init_video();

	__asm__ __volatile__("sti");


	showMemory();

halt:
	for (;;)
		; // or halt
			// next station is a jmp $ in asm
}
