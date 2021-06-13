#include <smp.h>
#include <apic.h>

smp_t smp = {
	.IOApicAddress = DEFAULT_IO_APIC_ADDRESS,
	.localApicAddress = 0,
	.processorList = {{0}},
	.numberOfProcessors = 1};

void acquireLock(int *lock)
{
	while (__sync_bool_compare_and_swap(lock, 0, 1) == FALSE)
	{
	}
}

void releaseLock(int *lock)
{
	__sync_bool_compare_and_swap(lock, 1, 0);
}