#include <system.h>
#include <cpuid.h>
#include "smp.h"
#include "apic.h"

extern int _boot_addr;
extern int _boot_end;

extern int _ap_start;
extern int _ap_end;

extern int _ap_boot_start;
extern int _ap_boot_end;

extern int end_stack;
extern int _ap_stack;
extern int _running_flag;

extern smp_t smp;
//Processor processorList[MAX_CPU];

//int processorListSize 						= 0;
//unsigned long localApicAddress				= 0x0;

void copy_ap_startup_code() {

	char * dest = (char *) 0x1000;

//	char hex[8];
//	memset(hex, 0, 8);
//
//	memset(hex, 0, 8);
//	hex_to_char((int) &_boot_addr, hex);
//	printf("boot_addr: \t0x%s\n", hex);
//	memset(hex, 0, 8);
//	hex_to_char((int) &_boot_end, hex);
//	printf("boot_end: \t0x%s\n", hex);
//
//	memset(hex, 0, 8);
//	hex_to_char((int) &_ap_start, hex);
//	printf("ap_start: \t0x%s\n", hex);
//
//	memset(hex, 0, 8);
//	hex_to_char((int) &_ap_end, hex);
//	printf("ap_end: \t0x%s\n", hex);
//
//	hex_to_char((int) dest, hex);
//	printf("dest: \t0x%s\n", hex);
//	memset(hex, 0, 8);
//	hex_to_char((int) &_ap_boot_start, hex);
//	printf("ap_boot_start: \t0x%s\n", hex);
//	memset(hex, 0, 8);
//	hex_to_char((int) &_ap_boot_end, hex);
//	printf("ap_boot_end: \t0x%s\n", hex);
//
//	hex_to_char((int) &end_stack, hex);
//	printf("end_stack: \t0x%s\n", hex);

	memcpy(dest, (char *) &_ap_boot_start,
			(int) &_ap_boot_end - (int) &_ap_boot_start);

}


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


void init_cpu() {

	//disable_pic();
	detect_cpu();
	anykey();
	initSMP();	// just zero out a couple of stuff

	if (check_cpuid_supported() == FALSE) {
		printf("CPUID not supported\n");
		return;
	}
	if (check_lapic() == FALSE) {
		printf("No lAPIC on chip\n");
		return;
	}
	if (check_msr() == FALSE) {
		printf("No MSR\n");
		return;
	}
	check_bsp() ? printf("we are BSP\n") : printf("we are AP\n");

	/* TODO: implement */
	
	// if (check_rsdp_table() == FALSE) {
	// 	printf("Could not find/parse RSDP\n");
	// 	return;
	// }
	printf("number of processors: %u\n", smp.numberOfProcessors);
	if (smp.numberOfProcessors <= 1) {
		printf("only 1 processor detected\n");
		return;
	}



	unsigned long *localApicIdVersionReg = (unsigned long *) (smp.localApicAddress
			+ LOCAL_APIC_ID_VERSION_REGISTER);

	char hex[8];
	hex_to_char(*localApicIdVersionReg, hex);


	char apic_version = (char)(*localApicIdVersionReg);

	hex_to_char((unsigned long) apic_version, hex);
	printf("apic version: 0x%s\n", hex);
	if ((apic_version < 0x10) || (apic_version >> 0x15)) {
		printf("version not supported\n");
		return;
	}


	copy_ap_startup_code();
	//apic_write(SPURIOUS_INTERRUPT_VECTOR_REGISTER, 0x100);

	printf("ApicID\tStack\tStatus\n");

	/**
	 * Borrowed from linux
	 */
	int i;
	int stack_count = 1;
	for (i = 0; i < smp.numberOfProcessors; i++) {
		if (smp.processorList[i].ApicId != 0) {	// don't start bsp
			unsigned long send_status = 0, accept_status = 0;
			_ap_stack = (int) &end_stack - (stack_count++ * STACK_SIZE);
			char hex[8];
			hex_to_char(_ap_stack, hex);
			printf("%u\t0x%s\t", smp.processorList[i].ApicId, hex);

			// apparently bug fix on some pentia
			apic_write(ERROR_STATUS_REGISTER, 0x0);
			apic_read(ERROR_STATUS_REGISTER);

			// TURN init on target chip
			apic_write(INTERRUPT_COMMAND_REGISTER_2, smp.processorList[i].ApicId << 24);
			apic_write(INTERRUPT_COMMAND_REGISTER_1, 0xC500);
			send_status = apic_wait_icr_idle();

			sleep(10);
			apic_write(INTERRUPT_COMMAND_REGISTER_2, smp.processorList[i].ApicId << 24);
			apic_write(INTERRUPT_COMMAND_REGISTER_1, 0x8500);
			send_status = apic_wait_icr_idle();

			// NOTE: no idea what this does and why it's needed
			asm volatile("mfence" : : : "memory");

			// do startup IPI's
			int j = 1;
			for(j = 1; j <= 2; j++) {

				// pentia bug workaround
				apic_write(ERROR_STATUS_REGISTER, 0x0);
				apic_read(ERROR_STATUS_REGISTER);

				// send SIPI
				apic_write(INTERRUPT_COMMAND_REGISTER_2, smp.processorList[i].ApicId << 24);
				apic_write(INTERRUPT_COMMAND_REGISTER_1, 0x4601);

				sleep(10);

				send_status = apic_wait_icr_idle();

				sleep(10);

				apic_write(ERROR_STATUS_REGISTER, 0x0);
				accept_status = (apic_read(ERROR_STATUS_REGISTER) & 0xEF);
				if (send_status || accept_status)
					break;
			}

			// waiting for AP's signal via mem
			if (_running_flag == 0) {
				printf("waiting for flag (probably forever)\n");
				while (_running_flag == 0) {}
			}

			printf("\tOK\n");

			_running_flag = 0;	// on to next

		}
	}



}

