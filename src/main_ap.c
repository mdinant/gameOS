#include <system.h>




void test_stack(int val) {
	memcpy((char*)0xb8000, (char *)&val, 4);
}

void _main_ap()
{
	__asm__ __volatile__ ("sti");

	//int val = 0x2f4b2f4f;
	test_stack(0x2f4b2f4f);

	for (;;); // or halt
}
