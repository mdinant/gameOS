#include <system.h>
#include <acpi.h>
#include <apic.h>
#include <smp.h>

extern smp_t smp;

static bool RSDPTableParser(void *tableAddr);
static bool XSDTTableParser(void *tableAddr);
static bool RSDTTableParser(void *tableAddr);
static bool FADTTableParser(void *tableAddr);
static bool MADTTableParser(void *tableAddr);
static bool HPETTableParser(void *tableAddr);
static bool SSDTTableParser(void *tableAddr);

#define NUMBER_SUPPORTED_TABLES 7
static ACPITable supportedTables[NUMBER_SUPPORTED_TABLES] = {
	{RSDP_SIGNATURE, 0L, FALSE, &RSDPTableParser},
	{RSDT_SIGNATURE, 0L, FALSE, &RSDTTableParser},
	{XSDT_SIGNATURE, 0L, FALSE, &XSDTTableParser},
	{FADT_SIGNATURE, 0L, FALSE, &FADTTableParser},
	{MADT_SIGNATURE, 0L, FALSE, &MADTTableParser},
	{HPET_SIGNATURE, 0L, FALSE, &HPETTableParser},
	{SSDT_SIGNATURE, 0L, FALSE, &SSDTTableParser}};

static bool (*tableParser)(void *);

static void verifyTable(char *name, uint32_t address)
{
	for (int i = 0; i < NUMBER_SUPPORTED_TABLES; i++)
	{
		if (strncmp(name, supportedTables[i].name, MAX_SIGNATURE_SIZE) == 0)
		{
			printf("\nVERIFIED %s\n", supportedTables[i].name);
			supportedTables[i].address = address;
			supportedTables[i].verified = TRUE;
		}
	}
}

bool getACPITableAddr(char *name, uint32_t * addrOut)
{
	for (int i = 0; i < NUMBER_SUPPORTED_TABLES; i++)
	{
		if (strncmp(name, supportedTables[i].name, 4) == 0)
		{
			if (supportedTables[i].verified)
			{
				*addrOut = supportedTables[i].address;
				return TRUE;
			}
		}
	}
	return FALSE;
}
static bool findTableParser(char *name, bool (**parserFnc)(void *))
{

	for (int i = 0; i < NUMBER_SUPPORTED_TABLES; i++)
	{
		if (strncmp(name, supportedTables[i].name, 4) == 0)
		{
			*parserFnc = supportedTables[i].parserFnc;
			return TRUE;
		}
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
	if (doChecksum((char *)madtTable, madtTable->Header.Length) == FALSE)
	{

		printf("checksum failed for madt\n");

		return FALSE;
	}

	verifyTable(MADT_SIGNATURE, (uint32_t)(tableAddr));

	char hex[8];
	memset(hex, 0, 8);
	hex_to_char(madtTable->LocalControllerAddress, hex);

	printf("LocalControllerAddress hex: 0x%s\n", hex);

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


static bool SSDTTableParser(void *tableAddr)
{

	printf("to be implemented\n");
	return FALSE;
}

static bool HPETTableParser(void *tableAddr)
{

	HPET *hpetTable = (HPET *)tableAddr;

	if (doChecksum(tableAddr, hpetTable->Header.Length) == FALSE)
	{
		perror("checksum failed for hpet\n");
		return FALSE;
	}
	verifyTable(HPET_SIGNATURE, (uint32_t)(tableAddr));
	return TRUE;
}

static bool FADTTableParser(void *tableAddr)
{

	printf("to be implemented\n");
	return FALSE;
}

static bool RSDTTableParser(void *tableAddr)
{
	ACPI_SDT_32 *rsdt = (ACPI_SDT_32 *)(tableAddr);

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
	verifyTable(RSDT_SIGNATURE, (uint32_t)(tableAddr));
	return TRUE;
}

//todo: check 64 bits long used where needed (not 32)
static bool XSDTTableParser(void *tableAddr)
{

	ACPI_SDT_64 *xsdt = (ACPI_SDT_64 *)(tableAddr);

	int entryCount = (xsdt->Header.Length - sizeof(xsdt->Header)) / 8;

	unsigned long long *addr = &xsdt->ListAddr;

	int i;
	for (i = 0; i < entryCount; i++)
	{
		// assumes address is < 4GB
		ACPISDTHeader *header = (ACPISDTHeader *)(unsigned long)(*addr); // get rightmost 32 bits from 64, we are assuming < 4GB
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
	verifyTable(XSDT_SIGNATURE, (uint32_t)(tableAddr));
	return TRUE;
}

static bool RSDPTableParser(void *tableAddr)
{
	RSDPDescriptor20 *rsdpDescriptor20 = (RSDPDescriptor20 *)tableAddr;

	if ((doChecksum((char *)&rsdpDescriptor20->FirstPart,
					sizeof(rsdpDescriptor20->FirstPart)) == FALSE) ||
		((rsdpDescriptor20->FirstPart.Revision > 1) && (doChecksum((char *)&rsdpDescriptor20->Length, 16) == FALSE)))
	{
		perror("bad RSDP\n");
		return FALSE;
	}
	printf("\tRSDP version: %u\n", rsdpDescriptor20->FirstPart.Revision);

	//	void *location;
	ACPISDTHeader *acpiSdtHeader;
	if (rsdpDescriptor20->FirstPart.Revision > 1)
	{

		acpiSdtHeader =
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
		if (findTableParser(XSDT_SIGNATURE, &tableParser) == FALSE)
		{
			perror("XSTD parser not found\n");
			return FALSE;
		}
		//return tableParser((ACPI_SDT_64 *)acpiSdtHeader);
	}
	else
	{
		acpiSdtHeader = (ACPISDTHeader *)rsdpDescriptor20->FirstPart.RsdtAddress;
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

		if (findTableParser(RSDT_SIGNATURE, &tableParser) == FALSE)
		{
			perror("RSDT parser not found\n");
			return FALSE;
		}
		//return tableParser((ACPI_SDT_32 *)acpiSdtHeader);
	}

	verifyTable(RSDP_SIGNATURE, (uint32_t)(tableAddr));
	return tableParser((ACPI_SDT_32 *)acpiSdtHeader);
}

static bool startParser()
{

	void *location;
	bool rsdpFound = (find_string(RSDP_SIGNATURE, 8, (char *)LAST_KB_640, KB,
								  &location) ||
					  find_string(RSDP_SIGNATURE, 8, (char *)BIOS_ROM_START,
								  (BIOS_ROM_END - BIOS_ROM_START), &location));

	if (rsdpFound == FALSE)
	{
		perror("RSDP not found\n");
		return FALSE;
	}

	char hex[8];

	hex_to_char((uint32_t)location, hex);
	printf("RSDP found at: %s\n", hex);
	if (findTableParser(RSDP_SIGNATURE, &tableParser) == FALSE)
	{
		perror("RSDP parser not found\n");
		return FALSE;
	}
	return tableParser(location);
}

void listAllTables()
{
	//bool ret = TRUE;

	if (startParser() == FALSE)
	{
		perror("Bad parsing\n");
	}

	//printf("Supported Tables:\n");

	// char **n = supportedTables;
	// while (strcmp(*n, "") != 0)
	// {
	// 	printf("%s\n", *n++);
	// }
}
