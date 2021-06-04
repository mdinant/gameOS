#include <system.h>

bool doChecksum(char  *table, size_t length)
{
    unsigned char sum = 0;
    int i;
    for (i = 0; i < length; i++)
    {
        sum += table[i];
    }

    return sum == 0;
}

bool find_string(char * string, int len, char * baseAddr, unsigned long length,
		char ** location) {


	unsigned long i;
	for (i = 0; i < length; i++) {

		char * lin_addr = &baseAddr[i];

		if (strncmp(string, lin_addr, len) == 0) {

			*location = lin_addr;

			return TRUE;

		}

	}

	return FALSE;
}
