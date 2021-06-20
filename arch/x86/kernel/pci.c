#include <system.h>
#include <pci.h>

#define VENDOR_ID_OFFSET 0
#define DEVICE_ID_OFFSET 2
#define COMMAND_OFFSET 4
#define STATUS_OFFSET 6
#define REVISION_ID_OFFSET 8
#define PROG_IF_OFFSET 9
#define SUBCLASS_OFFSET 10
#define CLASS_CODE_OFFSET 11

static char *classnames[] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station",
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non-Essential Instrumentation",
    "Reserved",
    "Co-Processor",
    "Reserved",
    "Unassigned Class"};

bool check_pci()
{
    regs32_t regs;
    regs.eax = 0xb101;
    regs.edi = 0x0;
    regs.edx = 0x0;
    int32_beta(0x1A, &regs);

    char hex[8];
    memset(hex, 0, 8);
    unsigned int number = regs.edx;
    hex_to_char(number, hex);
    printf("EDI: 0x%s\n", hex);

    return regs.edx == 0x20494350;
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outpdw(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((inpdw(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint8_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outpdw(0xCF8, address);
    /* read in the data */
    tmp = (uint8_t)((inpdw(0xCFC) >> ((offset % 4) * 4)));

    return (tmp);
}

uint32_t pci_config_read_reg(uint32_t bus, uint32_t slot, uint32_t func, uint8_t offset)
{
    uint32_t address;
    // uint32_t lbus = (uint32_t)bus;
    // uint32_t lslot = (uint32_t)slot;
    // uint32_t lfunc = (uint32_t)func;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((bus << 16) | (slot << 11) |
                         (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outpdw(0xCF8, address);
    /* read in the data */

    return inpdw(0xCFC);
}

// void pci_find_devices()
// {

//     bool done = FALSE;

//     while (!done)
//     {
//     }
// }

// uint8_t get_header_type(uint8_t bus, uint8_t slot, uint8_t function)
// {
// }

// void check_device(uint8_t bus, uint8_t device)
// {
//     uint8_t function = 0;

//     uint16_t vendorID = pci_get_vendor_id(bus, device, function);
//     if (vendorID == 0xFFFF)
//         return; // Device doesn't exist
//     //checkFunction(bus, device, function);
//     uint8_t headerType = getHeaderType(bus, device, function);
//     if ((headerType & 0x80) != 0)
//     {
//         /* It is a multi-function device, so check remaining functions */
//         // for (function = 1; function < 8; function++)
//         // {
//         //     if (getVendorID(bus, device, function) != 0xFFFF)
//         //     {
//         //         checkFunction(bus, device, function);
//         //     }
//         // }
//     }
// }

void parseDevice(uint32_t bus, uint32_t slot, uint32_t func, bool *multifunction)
{
    *multifunction = FALSE;
    uint32_t reg = pci_config_read_reg(bus, slot, func, 0);

    // char hex[8];
    // memset(hex, 0, 8);
    // hex_to_char(reg, hex);
    uint16_t vendorId = reg;
    if (vendorId == 0xFFFF)
        return; // BAD device
    uint16_t deviceId = reg >> 16;

    reg = pci_config_read_reg(bus, slot, func, 8);
    uint8_t class = reg >> 24;
    uint8_t subclass = reg >> 16;
    uint8_t progIf = reg >> 8;

    if ((class == 0x0C) && (subclass == 0x03))
    {
        printf ("USB type: %u\n", progIf);
    }

    reg = pci_config_read_reg(bus, slot, func, 12);
    uint8_t headerType = reg >> 16;
    //printf("Vendor ID: %u\n", vendorId);
    //printf("Device ID: %u\n", deviceId);
    printf("device: %u class: %s subclass: %u function: %u\n", slot, classnames[class], subclass, func);
    if ((headerType & 0x01) != 0)
    {
        printf("\tPCI to PCI bridge\n");
        reg = pci_config_read_reg(bus, slot, func, 24);
        uint8_t secbus = reg >> 8;
        printf("\tsecundary bus: %u\n", secbus);
    }
    else if ((headerType & 0x02) != 0)
        printf("\tCardBus bridge");
    if ((headerType & 0x80) != 0)
    {
        *multifunction = TRUE;
    }
    printf("------------\n");
}

void pci_check_all_buses(void)
{

    uint32_t bus;
    uint32_t device;

        cls();
    for (bus = 0; bus < 5; bus++)
    {
        printf("\n... PCI ... busnum: %u\n", bus);
        for (device = 0; device < 50; device++)
        {
            //            check_device(bus, device);

            // uint16_t vendorId = pci_config_read_word(bus, device, 0, VENDOR_ID_OFFSET);
            // uint16_t deviceId = pci_config_read_word(bus, device, 0, DEVICE_ID_OFFSET);

            // uint8_t classCode = pci_config_read_byte(bus, device, 0, CLASS_CODE_OFFSET);
            // uint8_t subclass = pci_config_read_byte(bus, device, 0, SUBCLASS_OFFSET);
            bool multifunction;
            parseDevice(bus, device, 0, &multifunction);
            if (multifunction == TRUE)
            {
                // already done 0, start with 1
                for (uint32_t i = 1; i < 8; i++)
                {
                    // TODO: multifunction is now dummy
                    parseDevice(bus, device, i, &multifunction);
                }
            }
            // pretty print
            if ((device % 5 == 0) && (device > 0))
               anykey();
        }
       // anykey();
    }
}