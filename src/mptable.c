#include <system.h>

#define MP_SIGNATURE				"_MP_"
#define PCMP_SIGNATURE				"PCMP"





#define IMCRP_BIT					7

#define ENTRY_TYPE_PROCESSOR		0
#define ENTRY_TYPE_BUS				1
#define ENTRY_TYPE_IOAPIC			2
#define ENTRY_TYPE_IO_INTERRUPT		3
#define ENTRY_TYPE_LOCAL_INTERRUPT	4

typedef struct __attribute__ ((packed)) {
	char Signature[4];
	unsigned long PsysicalAddressPointer;
	unsigned char Length;
	unsigned char SpecRev;
	unsigned char Checksum;
	unsigned char InformationByte1;
	unsigned char InformationByte2;
	unsigned char Reserved[3];
} mp_t;

typedef struct __attribute__ ((packed)) {
	char Signature[4];
	unsigned char BaseTableLength[2];
	unsigned char SpecRev;
	unsigned char Checksum;
	unsigned char OemId[8];
	unsigned char ProductId[12];
	unsigned char OemTablePointer[4];
	unsigned char OemTableSize[2];
	unsigned char EntryCount[2];
	unsigned char LocalApicAddr[4];
	unsigned char ExtendedTableLength[2];
	unsigned char ExtendedTableChecksum;

	// hack
	unsigned char firstEntryType;

} mp_conf_header_t;

typedef struct __attribute__ ((packed)) {
	unsigned char EntryType;
	unsigned char LocalApicId;
	unsigned char LocalApicVersion;
	unsigned char CpuFlags;
	unsigned char CpuSignature[4];
	unsigned char FeatureFlags[4];
	unsigned char Reserved[8];
} mp_conf_entry_processor_t;

typedef struct __attribute__ ((packed)) {
	unsigned char EntryType;
	unsigned char BusId;
	char BusTypeString[6];
} mp_conf_entry_bus_t;

typedef struct __attribute__ ((packed)) {
	unsigned char EntryType;
	unsigned char IoApicId;
	unsigned char IoApicVersion;
	unsigned char IoApicFlags;
	unsigned long IoApicAddr;
} mp_conf_entry_ioapic_t;


typedef struct __attribute__ ((packed)) {
	unsigned char EntryType;
	unsigned char InterruptType;
	unsigned char Flags[2];
	unsigned char SourceBusId;
	unsigned char SourceBusIrq;
	unsigned char DestIoApicId;
	unsigned char DestIoApicIntIn;

} mp_conf_entry_ioInterruptAssignment_t;

typedef struct __attribute__ ((packed)) {
	unsigned char EntryType;
	unsigned char InterruptType;
	unsigned char Flags[2];
	unsigned char SourceBusId;
	unsigned char SourceBusIrq;
	unsigned char DestLocalApicId;
	unsigned char DestLocalApicLintIn;
} mp_conf_entry_localInterruptAssignment_t;

bool parse_mp_table(void * tableAddr) {

	char hex[8];
	memset(hex, 0, 8);

	mp_t * mpTable = (mp_t*) tableAddr;
	if (doChecksum((char*)mpTable, mpTable->Length) == FALSE) {
		printf("checksum mp table failed\n");
		return FALSE;
	}
	if (CHECK_BIT(mpTable->InformationByte2, IMCRP_BIT)) {
		printf("PIC mode implemented\n");
	} else {
		printf("Virtual Wire mode implemented\n");
	}


	if (mpTable->PsysicalAddressPointer == 0x0) {
		// default config

		// sanity
		if (mpTable->InformationByte1 == 0x0) {
			printf("mp conf table specified but address = 0x0\n");
			return FALSE;
		}

		printf("default mp conf implemented: %u\n", mpTable->Checksum);

		return TRUE;

	} else {

		// parse conf table

		mp_conf_header_t * mpConfHeader = (mp_conf_header_t*) mpTable->PsysicalAddressPointer;

		char signature[5];
		memset(signature, 0, 5);
		memcpy(signature, mpConfHeader->Signature, 4);
		// sanity
		if (strcmp(PCMP_SIGNATURE, signature) != 0) {
			printf("PCMP signature not found\n");
			return FALSE;
		}

		char oemId[9];
		memset(oemId, 0, 9);
		memcpy(oemId, mpConfHeader->OemId, 8);
		printf("OemId: %s\n", oemId);

		char productId[13];
		memset(productId, 0, 13);
		memcpy(productId, mpConfHeader->ProductId, 12);
		printf("ProductId: %s\n", productId);

		unsigned char * entryTypePtr = &mpConfHeader->firstEntryType;


		unsigned short i;
		unsigned short count = *((unsigned short*)(mpConfHeader->EntryCount));
		printf("number of entries: %u\n", count);
		unsigned long offset = 0;

		for(i = 0; i < count; i++) {


			unsigned char entryType = entryTypePtr[offset];

			memset(hex, 0, 8);

			unsigned long offaddr = (unsigned long)&(entryTypePtr[offset]);

			hex_to_char(offaddr, hex);


			switch (entryType) {
			case ENTRY_TYPE_PROCESSOR: {
				// processor
				printf("offset: 0x%s\t", hex);
				printf("Processor entry\t");

				printf("apic id: %u\n", entryTypePtr[offset+1]);

				offset += 20;
				break;
			}
			case ENTRY_TYPE_BUS: {
				printf("offset: 0x%s\t", hex);
				printf("Bus entry\n");


				offset += 8;
				break;
			}
			case ENTRY_TYPE_IOAPIC: {
				printf("offset: 0x%s\t", hex);
				printf("IOAPIC entry\n");

				offset += 8;
				break;
			}
			case ENTRY_TYPE_IO_INTERRUPT: {
				printf("offset: 0x%s\t", hex);
				printf("IO Interrupt entry\n");

				offset += 8;
				break;
			}
			case ENTRY_TYPE_LOCAL_INTERRUPT: {
				printf("offset: 0x%s\t", hex);
				printf("Local Interrupt entry\n");

				offset += 8;
				break;
			}
			default:	// shut up compiler
				//printf("Unknown entry type: %u\n", *entryTypePtr);
				//entryTypePtr += 8;
				break;
			}


		}



	}




	return TRUE;


}

bool check_mp_table() {

	char hex[8];

	char * location;
	bool found = (find_string(MP_SIGNATURE, 4, (char *) LAST_KB_640, KB, &location)
			|| find_string(MP_SIGNATURE, 4, (char *) BIOS_ROM_START, (BIOS_ROM_END - BIOS_ROM_START), &location));

	if (found == TRUE) {
		hex_to_char((long) location, hex);
		printf("mp table found at: %s\n", hex);

		return parse_mp_table(location);

	}

	return FALSE;
}



