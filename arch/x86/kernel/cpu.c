#include <system.h>
#include <cpuid.h>
#include <smp.h>
#include <apic.h>

extern int _ap_boot_start;
extern int _ap_boot_end;

extern int end_stack;
extern int _ap_stack;
extern int _running_flag;

extern smp_t smp;


void copy_ap_startup_code() {

	char * dest = (char *) 0x1000;


	memcpy(dest, (char *) &_ap_boot_start,
			(int) &_ap_boot_end - (int) &_ap_boot_start);

}



bool check_cpu() {

	if (check_cpuid_supported() == FALSE) {
		printf("CPUID not supported\n");
		return FALSE;
	}
	if (check_lapic() == FALSE) {
		printf("No lAPIC on chip\n");
		return FALSE;
	}
	if (check_msr() == FALSE) {
		printf("No MSR\n");
		return FALSE;
	}
	if (check_bsp() == TRUE) {
		smp.processorList[0].isBsp = TRUE;
	}
	// if (check_rsdp_table() == FALSE) {
	// 	printf("Could not find/parse RSDP\n");
	// 	return FALSE;
	// }
	//printf("number of processors: %u\n", smp.numberOfProcessors);
	if (smp.numberOfProcessors <= 1) {
		printf("only 1 processor detected\n");
	//	return FALSE;
	}

	printf("cpu stuff good\n");

	return TRUE;

}

void init_cpu() {

	//disable_pic();

	//anykey();

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

	apic_write(SPURIOUS_INTERRUPT_VECTOR_REGISTER, 0x1FF);

	hex_to_char(smp.localApicAddress, hex);
	printf("apic address: %s\n", hex);

	printf("ApicID\tStack\tStatus\n");

	/**
	 * Borrowed from linux
	 */
	int i;
	int stack_count = 1;
	for (i = 0; i < smp.numberOfProcessors; i++) {
	//for (i = 0; i < 2; i++) {
		if (smp.processorList[i].ApicId != 0) {	// don't start bsp
//		if (smp.processorList[i].ApicId == 1) {
			_running_flag = i;

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

			// do Startup IPI's
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

			// waiting for AP's signal via memory
			if (_running_flag != 0) {
				printf("waiting for flag (probably forever)\n");
				while (_running_flag != 0) {}
			}

			printf("\tOK\n");

				// on to next

		}
//	}
	}
	//disable_pic();


}

// static bool MADTTableParser(void *tableAddr)
// {

// 	MADT *madtTable = (MADT *)tableAddr;

// 	char hex[8];
// 	memset(hex, 0, 8);
// 	hex_to_char(madtTable->LocalControllerAddress, hex);

// 	printf("LocalControllerAddress hex: 0x%s\n", hex);

// 	smp.localApicAddress = madtTable->LocalControllerAddress;

// 	if (doChecksum((char *)madtTable, madtTable->Header.Length) == FALSE)
// 	{

// 		printf("checksum failed for madt\n");

// 		return FALSE;
// 	}
// 	MADTRecord *madtRecord = (MADTRecord *)((char *)(madtTable) + sizeof(MADT));

// 	// parsen maar
// 	while ((char *)madtRecord < (char *)madtTable + madtTable->Header.Length)
// 	{
// 		//printf("entry type %u\n", madtRecord->EntryType);
// 		// printf("record length: %u\n", madtRecord->RecordLength);

// 		void *recordData = ((char *)(madtRecord) + sizeof(MADTRecord));

// 		switch (madtRecord->EntryType)
// 		{
// 		case MET_PROCESSOR_LOCAL_APIC:
// 		{

// 			MADTEntryTypePLA *madtEntryTypePla = (MADTEntryTypePLA *)recordData;

// 			// copy into our own list with bounds check
// 			if (smp.numberOfProcessors < MAX_CPU)
// 			{
// 				//memcpy(&madtEntryTypePLAList[madtEntryTypePLAListSize++], recordData, sizeof(MADTEntryTypePLA));

// 				smp.processorList[smp.numberOfProcessors].ApicId = madtEntryTypePla->ApicId;
// 				smp.numberOfProcessors++;
// 			}
// 			else
// 			{
// 				printf("cannot add processor, max reached\n");
// 			}

// 			printf("acpi processor id: %u\tapic id: %u\tflags: %u\n", madtEntryTypePla->AcpiProcessorId, madtEntryTypePla->ApicId, madtEntryTypePla->Flags);
// 			break;
// 		}
// 		case MET_IO_APIC:
// 		{
// 			MADTEntryTypeIOA *madtEntryTypeIOa = (MADTEntryTypeIOA *)recordData;
// 			char hex[8];
// 			hex_to_char(madtEntryTypeIOa->IoApicAddress, hex);
// 			printf("IOApic id: %u\tIOApicAddress: %s\tGISB: %u\n", madtEntryTypeIOa->IoApicId, hex, madtEntryTypeIOa->GlobalInterruptSystemBase);
// 			break;
// 		}
// 		case MET_INTERRUPT_SOURCE_OVERRIDE:
// 		{

// 			MADTEntryTypeISO *madtEntryTypeISO = (MADTEntryTypeISO *)recordData;
// 			printf("busSource: %u\tIrqSource: %u\tGISB: %u\tFlags: %u\n", madtEntryTypeISO->BusSource, madtEntryTypeISO->IrqSource, madtEntryTypeISO->GlobalSystemInterrupt, madtEntryTypeISO->Flags);

// 			break;
// 		}
// 		case MET_NON_MASKABLE_INTERRUPT:
// 		{

// 			MADTEntryTypeNMI *madtEntryTypeNMI = (MADTEntryTypeNMI *)recordData;
// 			printf("processor: %u\tFlags: %u\tLint: %u\n", madtEntryTypeNMI->Processor, madtEntryTypeNMI->Flags, madtEntryTypeNMI->Lint);

// 			break;
// 		}
// 		case MET_LOCAL_APIC_ADDRESS_OVERRIDE:
// 		{

// 			MADTEntryTypeLAAO *madtEntryTypeLAAO = (MADTEntryTypeLAAO *)recordData;
// 			printf("local apic address: %u\n", madtEntryTypeLAAO->LocalApicAddress);

// 			break;
// 		}
// 		default:
// 			break;
// 		}

// 		madtRecord = (MADTRecord *)((char *)(madtRecord) + madtRecord->RecordLength);
// 	}
// 	return TRUE;
// }