#define MAX_SIGNATURE_SIZE  10
#define RSDP_SIGNATURE      "RSD PTR "
#define RSDT_SIGNATURE      "RSDT"
#define XSDT_SIGNATURE      "XSDT"
#define FADT_SIGNATURE      "FACP"
#define MADT_SIGNATURE      "APIC"
#define HPET_SIGNATURE      "HPET"
#define SSDT_SIGNATURE      "SSDT"

#define MET_PROCESSOR_LOCAL_APIC 0
#define MET_IO_APIC 1
#define MET_INTERRUPT_SOURCE_OVERRIDE 2
#define MET_NON_MASKABLE_INTERRUPT 4
#define MET_LOCAL_APIC_ADDRESS_OVERRIDE 5

typedef struct 
{
    uint8_t AddressSpaceId;
    uint8_t RegisterBitWith;
    uint8_t RegisterBitOffset;
    uint8_t Reserved;
    uint64_t Address;
}__attribute__((packed)) ACPIAddressFormat;


typedef struct
{
    char Signature[4];
    unsigned long Length;
    unsigned char Revision;
    unsigned char Checksum;
    char OEMID[6];
    char OEMTableID[8];
    unsigned long OEMRevision;
    unsigned long CreatorID;
    unsigned long CreatorRevision;
} __attribute__((packed)) ACPISDTHeader;


typedef struct
{
    ACPISDTHeader Header;
    unsigned long ListAddr;

} __attribute__((packed)) ACPI_SDT_32;

typedef struct
{
    ACPISDTHeader Header;

    unsigned long long ListAddr;

} __attribute__((packed)) ACPI_SDT_64;

typedef struct
{
    char Signature[8];
    unsigned char Checksum;
    char OEMID[6];
    unsigned char Revision;
    unsigned long RsdtAddress;
} __attribute__((packed)) RSDPDescriptor;

typedef struct
{
    RSDPDescriptor FirstPart;

    unsigned long Length;
    unsigned long long XsdtAddress;
    unsigned char ExtendedChecksum;
    unsigned char Reserved[3];
} __attribute__((packed)) RSDPDescriptor20;

typedef struct
{
    ACPISDTHeader Header;
    uint8_t HardwareRevId;
    uint8_t NumComparators:5;
    uint8_t CountSizeCap;
    uint8_t Reserved;
    uint8_t IRQRoutingTable;
    uint16_t PCIVenderId;
    ACPIAddressFormat BaseAddress;
    unsigned char HpetNumber;
    unsigned short MCounterMinimumCT;   // in periodic mode
    unsigned char PageProtection;
} __attribute__((packed)) HPET;

typedef struct
{
    ACPISDTHeader Header;

    unsigned long LocalControllerAddress;
    unsigned long Flags;

} __attribute__((packed)) MADT;

typedef struct
{
    unsigned char EntryType;
    unsigned char RecordLength;

} __attribute__((packed)) MADTRecord;

typedef struct
{
    unsigned char AcpiProcessorId;
    unsigned char ApicId;
    unsigned long Flags;
} __attribute__((packed)) MADTEntryTypePLA;

typedef struct
{
    unsigned char IoApicId;
    unsigned char Reserved;
    unsigned long IoApicAddress;
    unsigned long GlobalInterruptSystemBase;
} __attribute__((packed)) MADTEntryTypeIOA;

typedef struct
{
    unsigned char BusSource;
    unsigned char IrqSource;
    unsigned long GlobalSystemInterrupt;
    unsigned short Flags;
} __attribute__((packed)) MADTEntryTypeISO;

typedef struct
{
    unsigned char Processor;
    unsigned short Flags;
    unsigned char Lint;
} __attribute__((packed)) MADTEntryTypeNMI;

typedef struct
{
    unsigned short Reserved;
    unsigned long long LocalApicAddress;
} __attribute__((packed)) MADTEntryTypeLAAO;

typedef struct
{
    char name[MAX_SIGNATURE_SIZE];
    uint32_t address;
    bool verified;
    bool (*parserFnc)(void *);
} __attribute__((packed)) ACPITable;



bool doChecksum(char *table, size_t length);

bool getACPITableAddr(char *name, uint32_t * addrOut);