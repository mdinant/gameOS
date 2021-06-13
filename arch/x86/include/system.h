/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Global function declarations and type definitions
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "system_def.h"

typedef int size_t;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short UInt16;
typedef unsigned long dword;
typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;


/* This defines what the stack looks like after an ISR was running */

// 32 bit
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

// 32 bit
typedef struct __attribute__((packed))
{

    unsigned int edi, esi, ebp, esp;
    unsigned int ebx, edx, ecx, eax;

    unsigned short gs, fs, es, ds;
    unsigned int eflags;
} regs32_t;

// 16 bit
//typedef struct __attribute__ ((packed)) {
//	unsigned short di, si, bp, sp;
//	union {
//		struct {
//			unsigned short bx, dx, cx, ax;
//		};
//		struct {
//			byte bl, bh, dl, dh, cl, ch, al, ah;
//		};
//	};
//
//	unsigned short gs, fs, es, ds, eflags;
//
//} regs16_t;

// tell compiler our int32 function is al
// void int32(unsigned char intnum, regs16_t *regs);

/* hpet */
//void testHPET();

/* int32_beta */
void int32_beta(unsigned char intnum, regs32_t *regs);
void copy_ap_startup_code(void);

/* cpuid.asm */
bool check_cpuid_supported(void);
bool check_lapic(void);
bool check_msr(void);
bool check_sse(void);
bool check_bsp(void);

/* cpudet.c */
int detect_cpu(void);
void init_cpu(void);
/* MAIN.C */

/* pic.asm */
void disable_pic(void);

void _main();
extern void _main_ap();

/* common.c */
//bool find_string(char *string, int len, char *baseAddr, unsigned long length, char **location);
bool find_string(char *string, int len, char *baseAddr, unsigned long length, void **location);

/* CONSOLE.C */
void init_video(void);
void puts(char *text);
void putch(unsigned char c);
void cls();
int printf(const char *format, ...);
int perror(const char *format, ...);

/* scrn.c */
void anykey();

/* VGA.c */
void put_pixel(int x, int y, byte color);
void line(int x1, int y1, int x2, int y2, byte color);
void set_palette(const byte *palette);
void swap_vga_buffer(void);
void set_text_mode();
void set_unchained_mode(int width, int height);
void set_unchained_mode2(int width, int height);
void flip_screen();
void test_vga();

/* VBE.C */
bool init_vbe();

/* GDT.C */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();

/* IDT.C */

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

void idt_set_gate(struct idt_entry * idt, unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void idt_install(struct idt_entry * idt, struct idt_ptr * idtp);

/* ISRS.C */
void isrs_install();

/* IRQ.C */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_install(struct idt_entry * idt);
void irq_remap(void);

/* TIMER.C */
void init_PIT(void);
void IRQ0_handler(void);
void sleep(unsigned long delay);
void timer_wait(int ticks);
void timer_install();
uint64_t getTicks();

/* KEYBOARD.C */
void keyboard_install();

/* MATH.C */
int abs(int number);
float fabs(float number);
void hex_to_char(unsigned long number, char *hex_value);

/* MOUSE.C */
void init_mouse();

/* mem.c */
long segToFlatAddr(long segAddr);
void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
size_t strlen(const char *str);
int strcmp(char string1[], char string2[]);
int strncmp(char string1[], char string2[], int n);
const void *memclr(const void *const mem, const size_t count);
void memcpy_SSE2(void *dest, const void *src, size_t count);

void outportb(word _port, byte _data);
void outportw(word _port, word _data);
byte inportb(word _port);
word inportw(word _port);
void showMemory();

/* cpu.c */
void enable_apic(void);
void cpu_get_msr(unsigned int msr, unsigned int *lo, unsigned int *hi);
void cpu_set_msr(unsigned int msr, unsigned int lo, unsigned int hi);
bool check_cpu();
/* mptable.c */
bool check_mp_table();

/* acpi.c */
bool check_rsdp_table();
void listAllTables();

void _main_ap();

#endif
