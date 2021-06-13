#ifndef __APIC_H
#define __APIC_H


#define DEFAULT_IO_APIC_ADDRESS				0xFEC00000

#define LOCAL_APIC_ID_REGISTER				0x20
#define LOCAL_APIC_ID_VERSION_REGISTER		0x30
#define EOI_REGISTER						0xB0
#define SPURIOUS_INTERRUPT_VECTOR_REGISTER	0xF0
#define ERROR_STATUS_REGISTER				0x280
#define INTERRUPT_COMMAND_REGISTER_1		0x300
#define INTERRUPT_COMMAND_REGISTER_2		0x310



unsigned int apic_read(unsigned int reg);
void apic_write(unsigned int reg, unsigned int value);
unsigned int apic_wait_icr_idle(void);

void apic_irq_install(struct idt_entry * idt);

#endif