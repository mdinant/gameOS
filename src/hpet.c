#include <system.h>
#include <acpi.h>

void testHPET()
{
    uint32_t addr;
    if (getACPITableAddr(HPET_SIGNATURE, &addr) == FALSE)
    {
        perror("Could not find HPET table addr\n");
    }
    else
    {
        printf("Found HPET addr\n");
    }
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
