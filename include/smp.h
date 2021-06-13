#ifndef __SMP_H
#define __SMP_H

#include <system.h>

enum ProcessorState {
	WAITING = 0, BUSY = 1
};



typedef int lock_t __attribute__ ((aligned (32)));

typedef struct {
	unsigned char ApicId;
	bool isBsp;
	lock_t lState;
	int state;

	/* Declare an IDT of 256 entries. Although we will only use the
	*  first 32 entries in this tutorial, the rest exists as a bit
	*  of a trap. If any undefined IDT entry is hit, it normally
	*  will cause an "Unhandled Interrupt" exception. Any descriptor
	*  for which the 'presence' bit is cleared (0) will generate an
	*  "Unhandled Interrupt" exception */
	struct idt_entry idt[256];
	struct idt_ptr idtp;

	//void *irq48_routine;

	/* Our GDT, with 3 entries, and finally our special GDT pointer */
	//struct gdt_entry gdt[3];
	//struct gdt_ptr gp;


} processor_t;


typedef struct {
	uint32_t IOApicAddress;
	uint32_t localApicAddress;
	processor_t processorList[MAX_CPU];
	uint32_t numberOfProcessors;
} smp_t;

bool setupLapic(processor_t * processor);
#endif