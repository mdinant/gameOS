#include <system.h>


typedef struct __attribute__((aligned(16))) {
	char stack_bottom[MAX_CPU * STACK_SIZE];
	//int * top; // points to itself
} stack_t;

stack_t stack;
int * end_stack;
