#include <smp.h>
#include <apic.h>

smp_t smp;

void initSMP() {
	smp.IOApicAddress = DEFAULT_IO_APIC_ADDRESS;
	smp.numberOfProcessors = 0;
	memset(smp.processorList, 0, sizeof(processor_t) * MAX_CPU);
}

void acquireLock(int * lock) {
	while (__sync_bool_compare_and_swap(lock, 0, 1) == FALSE) {}
}

void releaseLock(int * lock) {
	__sync_bool_compare_and_swap(lock, 1, 0);
}