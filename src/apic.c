#include <system.h>
#include <apic.h>
#include <smp.h>

extern smp_t smp;
extern void irq48();	//asm stuff first
extern void irq49();
extern void irq255();
extern void apic_irq48_handler(struct regs *r);
extern void apic_irq49_handler(struct regs *r);

unsigned int apic_read(unsigned int reg) {

	return *((unsigned int*)(smp.localApicAddress + reg));
}

void apic_write(unsigned int reg, unsigned int value) {
	*((unsigned int*)(smp.localApicAddress + reg)) = value;
}

unsigned int apic_wait_icr_idle(void) {
	unsigned int send_status;
	int timeout;

	timeout = 0;
	do {
		send_status = apic_read(INTERRUPT_COMMAND_REGISTER_1) & 0x01000;

		if (!send_status)
			break;
		sleep(10);
	} while (timeout++ < 100);

	return send_status;
}

void apic_irq48_handler(struct regs *r) {

	register int id asm("ebp");

	smp.processorList[id].state = BUSY;

}

void apic_irq49_handler(struct regs *r) {
	//numberOfProcessesorsDoneDrawing++;
}


void apic_irq_install(struct idt_entry * idt) {
    /**
     * APIC
     */
    idt_set_gate(idt, 48, (unsigned)irq48, 0x08, 0x8E);
    idt_set_gate(idt, 49, (unsigned)irq49, 0x08, 0x8E);
    idt_set_gate(idt, 255, (unsigned)irq255, 0x08, 0x8E);

	irq_install_handler(16, apic_irq48_handler);
	irq_install_handler(17, apic_irq49_handler);
}

