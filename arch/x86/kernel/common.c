#include <system.h>

bool find_string(char *string, int len, char *baseAddr, unsigned long length,
				 void **location)
{

	unsigned long i;
	for (i = 0; i < length; i++)
	{

		char *lin_addr = &baseAddr[i];

		if (strncmp(string, lin_addr, len) == 0)
		{

			*location = lin_addr;

			return TRUE;
		}
	}

	return FALSE;
}
/* This function doesn't swap and doesn't modify the content
 * of inp, the output is computed on out.
 *
 * Returns: a pointer to the output value (out) */
void *swapbytesout(void *inp, void *out, size_t len)
{
	unsigned int i;
	unsigned char *o = (unsigned char *)out;
	unsigned char *in = (unsigned char *)inp;

	for (i = 0; i < len; i++)
	{
		*(o + len - i - 1) = *(in + i);
	}

	return out;
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

inline word inportw(word _port)
{
	register word _data;

	__asm__ __volatile__ ("inw %1, %0" : "=a" (_data) : "Nd" (_port));

	return _data;
}

inline void outportw(word _port, word _data)
{
	__asm__ __volatile__("outw %0, %1" : : "a" (_data), "Nd" (_port));

	return;
}

inline byte inportdw (word _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}