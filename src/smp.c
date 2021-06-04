#include "smp.h"
#include "apic.h"

smp_t smp;

void initSMP() {
	smp.IOApicAddress = DEFAULT_IO_APIC_ADDRESS;
	smp.numberOfProcessors = 0;
	memset(smp.processorList, 0, sizeof(processor_t) * MAX_CPU);
}
