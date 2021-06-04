#include <system.h>
#include <acpi.h>
#include <apic.h>
#include <smp.h>



extern smp_t smp;
static char *supportedTables[7] = {RSDP_NAME,
								  RSDT_NAME,
								  XSDT_NAME,
								  FADT_NAME,
								  MADT_NAME,
								  HPET_NAME,
								  ""};


static bool RSDPTableParser(void *tableAddr);
static bool XSDTTableParser(void *tableAddr);
static bool RSDTTableParser(void *tableAddr);
static bool FADTTableParser(void *tableAddr);
static bool MADTTableParser(void *tableAddr);
static bool HPETTableParser(void *tableAddr);


static bool (*tableParser)(void *);


static ACPITableParser tableParserList[6] = {
	{RSDP_NAME, &RSDPTableParser},
	{XSDT_SIGNATURE, &XSDTTableParser},
	{RSDT_SIGNATURE, &RSDTTableParser},
	{FADT_SIGNATURE, &FADTTableParser},
	{MADT_SIGNATURE, &MADTTableParser},
	{HPET_SIGNATURE, &HPETTableParser}
};

static bool findTableParser(char *name, bool (**parserFnc)(void *))
{

	ACPITableParser *ptr = tableParserList;
	ACPITableParser *endPtr = tableParserList + sizeof(tableParserList) / sizeof(tableParserList[0]);
	while (ptr < endPtr)
	{
		if (strncmp(name, ptr->name, 4) == 0)
//		if (strcmp(name, ptr->name) == 0)	// name (should be) 0 terminated
		{
			*parserFnc = ptr->parserFnc;
			return TRUE;
		}
		ptr++;
	}

	return FALSE;
}

static void printTableName(char *tableName)
{
	char tempTableNameStore[5] = {0};

	memcpy(tempTableNameStore, tableName, 4);
	printf("%s", tempTableNameStore);
}

bool doChecksum(char *table, size_t length)
{
	unsigned char sum = 0;
	int i;
	for (i = 0; i < length; i++)
	{
		sum += table[i];
	}

	return sum == 0;
}

static bool MADTTableParser(void *tableAddr)
{

	MADT *madtTable = (MADT *)tableAddr;

	char hex[8];
	memset(hex, 0, 8);
	hex_to_char(madtTable->LocalControllerAddress, hex);

	printf("LocalControllerAddress hex: 0x%s\n", hex);

	smp.localApicAddress = madtTable->LocalControllerAddress;

	if (doChecksum((char *)madtTable, madtTable->Header.Length) == FALSE)
	{

		printf("checksum failed for madt\n");

		return FALSE;
	}
	MADTRecord *madtRecord = (MADTRecord *)((char *)(madtTable) + sizeof(MADT));

	// parsen maar
	while ((char *)madtRecord < (char *)madtTable + madtTable->Header.Length)
	{
		//printf("entry type %u\n", madtRecord->EntryType);
		// printf("record length: %u\n", madtRecord->RecordLength);

		void *recordData = ((char *)(madtRecord) + sizeof(MADTRecord));

		switch (madtRecord->EntryType)
		{
		case MET_PROCESSOR_LOCAL_APIC:
		{

			MADTEntryTypePLA *madtEntryTypePla = (MADTEntryTypePLA *)recordData;

			// copy into our own list with bounds check
			if (smp.numberOfProcessors < MAX_CPU)
			{
				//memcpy(&madtEntryTypePLAList[madtEntryTypePLAListSize++], recordData, sizeof(MADTEntryTypePLA));

				smp.processorList[smp.numberOfProcessors].ApicId = madtEntryTypePla->ApicId;
				smp.numberOfProcessors++;
			}
			else
			{
				printf("cannot add processor, max reached\n");
			}

			printf("acpi processor id: %u\tapic id: %u\tflags: %u\n", madtEntryTypePla->AcpiProcessorId, madtEntryTypePla->ApicId, madtEntryTypePla->Flags);
			break;
		}
		case MET_IO_APIC:
		{
			MADTEntryTypeIOA *madtEntryTypeIOa = (MADTEntryTypeIOA *)recordData;
			char hex[8];
			hex_to_char(madtEntryTypeIOa->IoApicAddress, hex);
			printf("IOApic id: %u\tIOApicAddress: %s\tGISB: %u\n", madtEntryTypeIOa->IoApicId, hex, madtEntryTypeIOa->GlobalInterruptSystemBase);
			break;
		}
		case MET_INTERRUPT_SOURCE_OVERRIDE:
		{

			MADTEntryTypeISO *madtEntryTypeISO = (MADTEntryTypeISO *)recordData;
			printf("busSource: %u\tIrqSource: %u\tGISB: %u\tFlags: %u\n", madtEntryTypeISO->BusSource, madtEntryTypeISO->IrqSource, madtEntryTypeISO->GlobalSystemInterrupt, madtEntryTypeISO->Flags);

			break;
		}
		case MET_NON_MASKABLE_INTERRUPT:
		{

			MADTEntryTypeNMI *madtEntryTypeNMI = (MADTEntryTypeNMI *)recordData;
			printf("processor: %u\tFlags: %u\tLint: %u\n", madtEntryTypeNMI->Processor, madtEntryTypeNMI->Flags, madtEntryTypeNMI->Lint);

			break;
		}
		case MET_LOCAL_APIC_ADDRESS_OVERRIDE:
		{

			MADTEntryTypeLAAO *madtEntryTypeLAAO = (MADTEntryTypeLAAO *)recordData;
			printf("local apic address: %u\n", madtEntryTypeLAAO->LocalApicAddress);

			break;
		}
		default:
			break;
		}

		madtRecord = (MADTRecord *)((char *)(madtRecord) + madtRecord->RecordLength);
	}
	return TRUE;
}

// old table
static bool findTableRSDT(ACPI_SDT_32 *rsdt, char *tableName, void **location)
{

	int entryCount = (rsdt->Header.Length - sizeof(rsdt->Header)) / 4;

	unsigned long *addr = &rsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++)
	{
		ACPISDTHeader *header = (ACPISDTHeader *)(*addr);

		if (strncmp(tableName, header->Signature, 4) == 0)
		{
			//printf("found madt\n");
			*location = (void *)header;

			return TRUE;
		}

		addr++;
	}
	return FALSE;
}

// new table
static bool findTableXSDT(ACPI_SDT_64 *xsdt, char *tableName, void **location)
{

	int entryCount = (xsdt->Header.Length - sizeof(xsdt->Header)) / 8;

	unsigned long long *addr = &xsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++)
	{
		ACPISDTHeader *header = (ACPISDTHeader *)(unsigned long)(*addr);

		if (strncmp(tableName, header->Signature, 4) == 0)
		{
			//printf("found madt\n");
			*location = (void *)header;

			return TRUE;
		}

		addr++;
	}
	return FALSE;
}

static bool parse_rsdp_table(void *tableAddr)
{

	RSDPDescriptor20 *rsdpDescriptor20 = (RSDPDescriptor20 *)tableAddr;

	// check version
	printf("rsdp version: %u\n", rsdpDescriptor20->FirstPart.Revision);

	if (doChecksum((char *)&rsdpDescriptor20->FirstPart,
				   sizeof(rsdpDescriptor20->FirstPart)) == FALSE)
	{
		printf("checksum failed for 1.0\n");

		return FALSE;
	}

	if ((rsdpDescriptor20->FirstPart.Revision > 1) && (doChecksum((char *)&rsdpDescriptor20->Length, 16) == FALSE))
	{
		printf("checksum failed for 2.0\n");

		return FALSE;
	}

	void *location;
	if (rsdpDescriptor20->FirstPart.Revision > 1)
	{

		ACPISDTHeader *acpiSdtHeader =
			(ACPISDTHeader *)(unsigned long)rsdpDescriptor20->XsdtAddress;
		if (strncmp(XSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0)
		{
			printf("%s signature check failed\n", XSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char *)acpiSdtHeader, acpiSdtHeader->Length) == FALSE)
		{
			printf("checksum failed for xsdt header\n");

			return FALSE;
		}

		if (findTableXSDT((ACPI_SDT_64 *)acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE)
		{
			printf("could not find MADT tABLE\n");
			return FALSE;
		}
	}
	else
	{
		ACPISDTHeader *acpiSdtHeader = (ACPISDTHeader *)rsdpDescriptor20->FirstPart.RsdtAddress;
		if (strncmp(RSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0)
		{
			printf("%s signature check failed\n", RSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char *)acpiSdtHeader, acpiSdtHeader->Length) == FALSE)
		{
			printf("checksum failed for rsdt header\n");

			return FALSE;
		}

		if (findTableRSDT((ACPI_SDT_32 *)acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE)
		{
			printf("could not find MADT tABLe\n");
			return FALSE;
		}
	}

	// location is madt

	return MADTTableParser(location);
}

bool check_rsdp_table()
{
	char hex[8];
	char *location;

	bool found = (find_string(RSDP_SIGNATURE, 8, (char *)LAST_KB_640, KB,
							  &location) ||
				  find_string(RSDP_SIGNATURE, 8, (char *)BIOS_ROM_START,
							  (BIOS_ROM_END - BIOS_ROM_START), &location));

	if (found == TRUE)
	{
		hex_to_char((long)location, hex);
		printf("rsdp table found at: %s\n", hex);
		printf("str: %s\n", location);

		return parse_rsdp_table(location);
	}

	return FALSE;
}

static bool HPETTableParser(void *tableAddr)
{

	printf("to be implemented\n");
	return FALSE;

}

static bool FADTTableParser(void *tableAddr)
{

	printf("to be implemented\n");
	return FALSE;

}

static bool RSDTTableParser(void *tableAddr)
{
	ACPI_SDT_32 *rsdt = (ACPI_SDT_32*)(tableAddr);

	int entryCount = (rsdt->Header.Length - sizeof(rsdt->Header)) / 4;

	unsigned long *addr = &rsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++)
	{
		ACPISDTHeader *header = (ACPISDTHeader *)(*addr);
		printTableName(header->Signature);
		printf(" found\n");
		if (findTableParser(header->Signature, &tableParser) == FALSE)
		{
			perror("parser not found\n");
			return FALSE;
		}
		tableParser((ACPI_SDT_32 *)header);
		addr++;
	}
	return TRUE;
}

//todo: check 64 bits long used where needed (not 32)
static bool XSDTTableParser(void *tableAddr)
{

	ACPI_SDT_64 * xsdt = (ACPI_SDT_64*)(tableAddr);

	int entryCount = (xsdt->Header.Length - sizeof(xsdt->Header)) / 8;

	unsigned long long *addr = &xsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++)
	{
		ACPISDTHeader *header = (ACPISDTHeader *)(unsigned long)(*addr);
		printTableName(header->Signature);
		printf(" found\n");
		// if (strncmp(tableName, header->Signature, 4) == 0)
		// {
		// 	//printf("found madt\n");
		// 	*location = (void *)header;

		// }

		addr++;
	}
	return TRUE;
}

static bool RSDPTableParser(void *tableAddr)
{
	RSDPDescriptor20 *rsdpDescriptor20 = (RSDPDescriptor20 *)tableAddr;

	// check version
	printf("\tRSDP version: %u\n", rsdpDescriptor20->FirstPart.Revision);

	if (doChecksum((char *)&rsdpDescriptor20->FirstPart,
				   sizeof(rsdpDescriptor20->FirstPart)) == FALSE)
	{
		printf("checksum failed for 1.0\n");

		return FALSE;
	}

	if ((rsdpDescriptor20->FirstPart.Revision > 1) && (doChecksum((char *)&rsdpDescriptor20->Length, 16) == FALSE))
	{
		printf("checksum failed for 2.0\n");

		return FALSE;
	}

	//void *location;
	if (rsdpDescriptor20->FirstPart.Revision > 1)
	{

		ACPISDTHeader *acpiSdtHeader =
			(ACPISDTHeader *)(unsigned long)rsdpDescriptor20->XsdtAddress;
		if (strncmp(XSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0)
		{
			printf("%s signature check failed\n", XSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char *)acpiSdtHeader, acpiSdtHeader->Length) == FALSE)
		{
			printf("checksum failed for xsdt header\n");

			return FALSE;
		}

		printf("XSTD found\n");
		if (findTableParser(XSDT_NAME, &tableParser) == FALSE)
		{
			perror("XSTD parser not found\n");
			return FALSE;
		}
		return tableParser((ACPI_SDT_64 *)acpiSdtHeader);

		// if (findTableXSDT((XSDT *)acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE)
		// {
		// 	printf("could not find MADT tABLE\n");
		// 	return FALSE;
		// }
	}
	else
	{
		ACPISDTHeader *acpiSdtHeader = (ACPISDTHeader *)rsdpDescriptor20->FirstPart.RsdtAddress;
		if (strncmp(RSDT_SIGNATURE, acpiSdtHeader->Signature, 4) != 0)
		{
			printf("%s signature check failed\n", RSDT_SIGNATURE);
			return FALSE;
		}

		if (doChecksum((char *)acpiSdtHeader, acpiSdtHeader->Length) == FALSE)
		{
			printf("checksum failed for rsdt header\n");

			return FALSE;
		}
		printf("RSDT found\n");


		if (findTableParser(RSDT_NAME, &tableParser) == FALSE)
		{
			perror("RSDT parser not found\n");
			return FALSE;
		}
		return tableParser((ACPI_SDT_32 *)acpiSdtHeader);

		// if (findTableRSDT((RSDT *)acpiSdtHeader, MADT_SIGNATURE, &location) == FALSE)
		// {
		// 	printf("could not find MADT tABLe\n");
		// 	return FALSE;
		// }
	}

	// location is madt

	//return parseMADTTable(location);
}

void listAllTables()
{
	printf("Supported Tables:\n");

	char **n = supportedTables;
	while (strcmp(*n, "") != 0)
	{
		printf("%s\n", *n++);
	}

	char hex[8];
	char *location;

	bool rsdpFound = (find_string(RSDP_SIGNATURE, 8, (char *)LAST_KB_640, KB,
								  &location) ||
					  find_string(RSDP_SIGNATURE, 8, (char *)BIOS_ROM_START,
								  (BIOS_ROM_END - BIOS_ROM_START), &location));

	if (rsdpFound == FALSE)
	{
		perror("RSDP not found\n");
		return;
	}

	hex_to_char((long)location, hex);
	printf("RSDP found at: %s\n", hex);

	if (findTableParser(RSDP_NAME, &tableParser) == FALSE)
	{
		perror("RSDP parser not found\n");
		return;
	}
	tableParser(location);

}
