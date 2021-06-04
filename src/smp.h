#ifndef __SMP_H
#define __SMP_H

#include <system.h>

typedef struct {
	unsigned char ApicId;
	bool isBsp;
} processor_t;


typedef struct {
	unsigned int localApicAddress;
	unsigned long IOApicAddress;
	processor_t processorList[MAX_CPU];
	int numberOfProcessors;
} smp_t;


void initSMP();

#endif
