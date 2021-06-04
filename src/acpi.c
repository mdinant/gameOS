#include <system.h>
#include "apic.h"
#include "smp.h"


#define RSDP_SIGNATURE					"RSD PTR "
#define RSDT_SIGNATURE					"RSDT"
#define XSDT_SIGNATURE					"XSDT"
#define MADT_SIGNATURE					"APIC"

#define MET_PROCESSOR_LOCAL_APIC		0
#define MET_IO_APIC						1
#define MET_INTERRUPT_SOURCE_OVERRIDE	2
#define MET_NON_MASKABLE_INTERRUPT		4
#define MET_LOCAL_APIC_ADDRESS_OVERRIDE	5

typedef struct {
	char Signature[4];
	unsigned long Length;
	unsigned char Revision;
	unsigned char Checksum;
	char OEMID[6];
	char OEMTableID[8];
	unsigned long OEMRevision;
	unsigned long CreatorID;
	unsigned long CreatorRevision;
} __attribute__ ((packed)) ACPISDTHeader;

typedef struct  {
	ACPISDTHeader Header;
	unsigned long ListAddr;

} __attribute__ ((packed)) RSDT;

typedef struct  {
	ACPISDTHeader Header;

	unsigned long long ListAddr;

} __attribute__ ((packed)) XSDT;


typedef struct {
	char Signature[8];
	unsigned char Checksum;
	char OEMID[6];
	unsigned char Revision;
	unsigned long RsdtAddress;
} __attribute__ ((packed)) RSDPDescriptor ;

typedef struct {
	RSDPDescriptor FirstPart;

	unsigned long Length;
	unsigned long long XsdtAddress;
	unsigned char ExtendedChecksum;
	unsigned char reserved[3];
} __attribute__ ((packed)) RSDPDescriptor20;


typedef struct {
	ACPISDTHeader Header;

	unsigned long LocalControllerAddress;
	unsigned long Flags;

} __attribute__ ((packed)) MADT;

typedef struct {
	unsigned char EntryType;
	unsigned char RecordLength;

} __attribute__ ((packed)) MADTRecord;

typedef struct {
	unsigned char AcpiProcessorId;
	unsigned char ApicId;
	unsigned long Flags;
} __attribute__ ((packed)) MADTEntryTypePLA;

typedef struct {
	unsigned char IoApicId;
	unsigned char Reserved;
	unsigned long IoApicAddress;
	unsigned long GlobalInterruptSystemBase;
} __attribute__ ((packed)) MADTEntryTypeIOA;

typedef struct {
	unsigned char BusSource;
	unsigned char IrqSource;
	unsigned long GlobalSystemInterrupt;
	unsigned short Flags;
} __attribute__ ((packed)) MADTEntryTypeISO;

typedef struct {
	unsigned char Processor;
	unsigned short Flags;
	unsigned char Lint;
} __attribute__ ((packed)) MADTEntryTypeNMI;

typedef struct {
	unsigned short Reserved;
	unsigned long long LocalApicAddress;
} __attribute__ ((packed)) MADTEntryTypeLAAO;

//extern Processor processorList[MAX_CPU];
//extern int processorListSize;
//unsigned long localApicAddress;

extern smp_t smp;


bool parseMADTTable(void * tableAddr) {

	MADT * madtTable = (MADT*)tableAddr;

	char hex[8];
	memset(hex, 0, 8);
	hex_to_char(madtTable->LocalControllerAddress, hex);

	printf("LocalControllerAddress hex: 0x%s\n", hex);


	smp.localApicAddress = madtTable->LocalControllerAddress;

	if (doChecksum((char*)madtTable, madtTable->Header.Length) == FALSE) {

		printf("checksum failed for madt\n");

		return FALSE;
	}

	MADTRecord  * madtRecord = (MADTRecord*) ((char*)(madtTable) + sizeof(MADT));

	// parsen maar
	while ((char*)madtRecord < (char*)madtTable + madtTable->Header.Length) {
		//printf("entry type %u\n", madtRecord->EntryType);
		//printf("record length: %u\n", madtRecord->RecordLength);

		void * recordData = ((char*)(madtRecord) + sizeof(MADTRecord));

		switch (madtRecord->EntryType) {
		case MET_PROCESSOR_LOCAL_APIC: {

			MADTEntryTypePLA * madtEntryTypePla = (MADTEntryTypePLA *) recordData;

			// copy into our own list with bounds check
			if (smp.numberOfProcessors < MAX_CPU) {
				//memcpy(&madtEntryTypePLAList[madtEntryTypePLAListSize++], recordData, sizeof(MADTEntryTypePLA));

				smp.processorList[smp.numberOfProcessors].ApicId = madtEntryTypePla->ApicId;
				smp.numberOfProcessors++;
			} else {
				printf("cannot add processor, max reached\n");
			}


			printf("acpi processor id: %u\tapic id: %u\tflags: %u\n", madtEntryTypePla->AcpiProcessorId, madtEntryTypePla->ApicId, madtEntryTypePla->Flags);
			break;
		}
		case MET_IO_APIC: {
			MADTEntryTypeIOA * madtEntryTypeIOa = (MADTEntryTypeIOA *) recordData;
			char hex[8];
			hex_to_char(madtEntryTypeIOa->IoApicAddress, hex);
			printf("IOApic id: %u\tIOApicAddress: %s\tGISB: %u\n", madtEntryTypeIOa->IoApicId, hex, madtEntryTypeIOa->GlobalInterruptSystemBase);
			break;
		}
		case MET_INTERRUPT_SOURCE_OVERRIDE: {

			MADTEntryTypeISO * madtEntryTypeISO = (MADTEntryTypeISO *) recordData;
			printf("busSource: %u\tIrqSource: %u\tGISB: %u\tFlags: %u\n", madtEntryTypeISO->BusSource, madtEntryTypeISO->IrqSource, madtEntryTypeISO->GlobalSystemInterrupt, madtEntryTypeISO->Flags);

			break;
		}
		case MET_NON_MASKABLE_INTERRUPT: {

			MADTEntryTypeNMI * madtEntryTypeNMI = (MADTEntryTypeNMI *) recordData;
			printf("processor: %u\tFlags: %u\tLint: %u\n", madtEntryTypeNMI->Processor, madtEntryTypeNMI->Flags, madtEntryTypeNMI->Lint);

			break;
		}
		case MET_LOCAL_APIC_ADDRESS_OVERRIDE: {


			MADTEntryTypeLAAO * madtEntryTypeLAAO = (MADTEntryTypeLAAO *) recordData;
			printf("local apic address: %u\n", madtEntryTypeLAAO->LocalApicAddress);

			break;
		}
		default:
			break;
		}

		madtRecord = (MADTRecord*) ((char*)(madtRecord) + madtRecord->RecordLength);

	}
	return TRUE;
}


bool findTableRSDT(RSDT * rsdt, char * tableName, void ** location) {

	int entryCount = (rsdt->Header.Length - sizeof(rsdt->Header)) / 4;

	unsigned long * addr = &rsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++) {
		ACPISDTHeader *header = (ACPISDTHeader *) (*addr);

        if (strncmp(tableName, header->Signature, 4) == 0) {
        	//printf("found madt\n");
        	*location = (void *) header;

        	return TRUE;
        }

        addr++;
    }
	return FALSE;
}

bool findTableXSDT(XSDT * xsdt, char * tableName, void ** location) {

	int entryCount = (xsdt->Header.Length - sizeof(xsdt->Header)) / 8;

	unsigned long long * addr = &xsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++) {
		ACPISDTHeader *header = (ACPISDTHeader *) (unsigned long)(*addr);

        if (strncmp(tableName, header->Signature, 4) == 0) {
        	//printf("found madt\n");
        	*location = (void *) header;

        	return TRUE;
        }

        addr++;
    }
	return FALSE;
}

bool parse_rsdp_table(void * tableAddr) {

	RSDPDescriptor20 * rsdpDescriptor20 = (RSDPDescriptor20*) tableAddr;

	// check version
	printf("rsdp version: %u\n", rsdpDescriptor20->FirstPart.Revision);

	if (doChecksum((char*) &rsdpDescriptor20->FirstPart,
			sizeof(rsdpDescriptor20->FirstPart)) == FALSE) {
		printf("checksum failed for 1.0\n");

		return FALSE;
	}

	if ((rsdpDescriptor20->FirstPart.Revision > 1)
			&& (doChecksum((char*) &rsdpDescriptor20->Length, 16) == FALSE)) {
		printf("checksum failed for 2.0\n");

		return FALSE;
	}



	void * location;
	if (rsdpDescriptor20->FirstPart.Revision > 1) {

		ACPISDTHeader * acpiSdtHeader =
				(ACPISDTHeader*) (unsigned long)rsdpDescriptor20->XsdtAddress;
		if (strncmp(XSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0) {
			printf("%s signature check failed\n", XSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char*) acpiSdtHeader, acpiSdtHeader->Length) == FALSE) {
			printf("checksum failed for xsdt header\n");

			return FALSE;
		}



		if (findTableXSDT((XSDT*) acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE) {
			printf("could not find MADT tABLE\n");
			return FALSE;
		}
	} else {
		ACPISDTHeader * acpiSdtHeader = (ACPISDTHeader*) rsdpDescriptor20->FirstPart.RsdtAddress;
		if (strncmp(RSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0) {
			printf("%s signature check failed\n", RSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char*) acpiSdtHeader, acpiSdtHeader->Length) == FALSE) {
			printf("checksum failed for rsdt header\n");

			return FALSE;
		}



		if (findTableRSDT((RSDT*) acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE) {
			printf("could not find MADT tABLe\n");
			return FALSE;
		}
	}

	// location is madt


	return parseMADTTable(location);
}

bool check_rsdp_table() {

	char hex[8];

	char * location;
	bool found = (find_string(RSDP_SIGNATURE, 8, (char *) LAST_KB_640, KB,
			&location)
			|| find_string(RSDP_SIGNATURE, 8, (char *) BIOS_ROM_START,
					(BIOS_ROM_END - BIOS_ROM_START), &location));

	if (found == TRUE) {
		hex_to_char((long) location, hex);
		printf("rsdp table found at: %s\n", hex);

		printf("str: %s\n", location);
		return parse_rsdp_table(location);

	}

	return FALSE;
}





