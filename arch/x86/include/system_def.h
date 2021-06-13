#define FALSE 0
#define TRUE 1

// pos 0 is right most bit
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))


#define LAST_KB_640					0x0009FC00
#define BIOS_ROM_START				0xE0000
#define BIOS_ROM_END				0xFFFFF

#define KB							0x400
#define KB_64						0x10000

#define MB							0x100000

#define MAX_CPU						128
#define STACK_SIZE					0x1000
