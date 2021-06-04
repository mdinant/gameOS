#include <system.h>

#define MEM_INFO_ADDR							0x2000
#define DETECT_MEMORY_COMMAND					0xE820
#define MAGIC									0x534D4150

#define SSE_XMM_SIZE							0x10

extern bool sse2Supported;

char *regionTypes[] = {"UNKNOWN", "Usable RAM", "Reserved", "ACPI reclaimable", "ACPI NVS", "Bad Memory"};

typedef struct {
	unsigned long BaseAddressLow;
	unsigned long BaseAddressHigh;
	unsigned long LengthOfRegionLow;
	unsigned long LengthOfRegionHigh;
	unsigned long RegionType;
	unsigned long ExtendedAttributes;

}__attribute__ ((packed)) memInfoBlock_t;

long segToFlatAddr(long segAddr) {
	return ((segAddr & 0xFFFF0000) >> 12) + (segAddr & 0xFFFF);
}
inline void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}

inline void *memset(void *dest, char val, size_t count)
{
	char *temp = (char *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

inline size_t strlen(const char *str)
{
	size_t retval;
	for(retval = 0; *str != '\0'; str++) retval++;
	return retval;
}

int strcmp(char string1[], char string2[] )
{
	int i;

    for (i = 0; ; i++)
    {
        if (string1[i] != string2[i])
        {
            return string1[i] < string2[i] ? -1 : 1;
        }

        if (string1[i] == '\0')
        {
            return 0;
        }
    }

    return 0;

}

int strncmp(char string1[], char string2[], int n)
{
	int i;

    for (i = 0; i < n; i++)
    {
        if (string1[i] != string2[i])
        {
            return string1[i] < string2[i] ? -1 : 1;
        }

        if (string1[i] == '\0')
        {
            return 0;
        }
    }

    return 0;

}

inline byte inportb (word _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

inline void outportb (word _port, byte _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

inline void outportw(word _port, word _data)
{
	__asm__ __volatile__("outw %0, %1" : : "a" (_data), "Nd" (_port));

	return;
}

inline word inportw(word _port)
{
	register word _data;

	asm volatile("inw %1, %0" : "=a" (_data) : "Nd" (_port));

	return _data;
}


// can be unaligned
static const void * memclr_sse2(const void * const m_start, const size_t m_count)
{
    // "i" is our counter of how many bytes we've cleared
    size_t i;

    // find out if "m_start" is aligned on a SSE_XMM_SIZE boundary
    if((size_t)m_start & (SSE_XMM_SIZE - 1))
    {
        i = 0;

        // we need to clear byte-by-byte until "m_start" is aligned on an SSE_XMM_SIZE boundary
        // ... and lets make sure we don't copy 'too' many bytes (i < m_count)
        while((((size_t)m_start + i) & (SSE_XMM_SIZE - 1)) && i < m_count)
        {
            asm("stosb;" :: "D"((size_t)m_start + i), "a"(0));
            i++;
        }
    }
    else
    {
        // if "m_start" was aligned, set our count to 0
        i = 0;
    }

    // clear 64-byte chunks of memory (4 16-byte operations)
    for(; i + 64 <= m_count; i += 64)
    {
        asm volatile(" pxor %%xmm0, %%xmm0;	"    // set XMM0 to 0
                     " movdqa %%xmm0, 0(%0);	"    // move 16 bytes from XMM0 to %0 + 0
                     " movdqa %%xmm0, 16(%0);	"
                     " movdqa %%xmm0, 32(%0);	"
                     " movdqa %%xmm0, 48(%0);	"
                     :: "r"((size_t)m_start + i));

    }

    // copy the remaining bytes (if any)
    asm(" rep stosb; " :: "a"((size_t)(0)), "D"(((size_t)m_start) + i), "c"(m_count - i));

    // "i" will contain the total amount of bytes that were actually transfered
    i += m_count - i;

    // we return "m_start" + the amount of bytes that were transfered
    return (void *)(((size_t)m_start) + i);
}

static const void * memclr_std(const void * const m_start, const size_t m_count)
{
    asm("rep stosl;"::"a"(0),"D"((size_t)m_start),"c"(m_count / 4));
    asm("rep stosb;"::"a"(0),"D"(((size_t)m_start) + ((m_count / 4) * 4)),"c"(m_count - ((m_count / 4) * 4)));

    return (void *)((size_t)m_start + m_count);
}

const void * memclr(const void * const mem, const size_t count) {
    // return if there is no count
    if(!count){ return mem; }
    // if the CPU is SSE2 capable, we favor the SSE2 version of this function first
    else if(sse2Supported)
    {
        return memclr_sse2(mem, count);
    }
    // if the CPU didn't support SSE2, then we use the generic standard function
    return memclr_std(mem, count);
}


void showMemory() {

	memInfoBlock_t * mib = (memInfoBlock_t *) MEM_INFO_ADDR;
	//int entries = 0;

	// first call

	regs32_t regs;
	memset(&regs, 0, sizeof(regs));
	regs.eax = DETECT_MEMORY_COMMAND;
	regs.edi = MEM_INFO_ADDR;
	regs.es = 0;
	regs.edx = MAGIC;
	regs.ecx = 24;

	//regs.ebx = 0x0;

	int32_beta(0x15, &regs);

	if (regs.eax != MAGIC) {
		printf("mem call not good\n");
		return;
	}

	// save ebx
	printf("number of bytes stored: %u\n", regs.ecx);

	//printf("BaseAddress\tLengthOfRegion\tType\n");
	printf("%8s\t%8s\t%8s\t\n", "BaseAddrLow", "LengthOfReg", "RegionType");

	//printf("BaseAddrLow\t\tLengthOfReg\tType\n");
	//printf("%u\t%u\t%u\n", mib->BaseAddressLow, mib->LengthOfRegionLow, mib->RegionType);





	bool printedMB = FALSE;

	char hex[8];
	do  {

		if ((mib->BaseAddressLow >= 1048576) && (!printedMB)){
			printf("-------- 1 MB --------\n");
			printedMB = TRUE;
		}


		hex_to_char(mib->BaseAddressLow, hex);
		printf("%8s\t", hex);
		hex_to_char(mib->LengthOfRegionLow, hex);
		printf("%8s\t", hex);
		hex_to_char(mib->RegionType, hex);
		printf("%8s\t", hex);
		if ((mib->RegionType >= 0) && (mib->RegionType < 6)) {
			printf("%10s", regionTypes[mib->RegionType]);
		}
		printf("\n");

		//printf("%u\t\t%u\t\tt%u\n", mib->BaseAddressLow, mib->LengthOfRegionLow, mib->RegionType);
		//mib++;								// increase by entry size

		mib += sizeof(memInfoBlock_t);


		regs.eax = DETECT_MEMORY_COMMAND;	// reset eax
		regs.ecx = 24;						// reset ecx
		regs.edi = (unsigned int)mib;

		int32_beta(0x15, &regs);



	} while ((regs.ebx != 0) && (CHECK_BIT(regs.eflags, 0) != 1));


}
