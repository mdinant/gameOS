/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Global function declarations and type definitions
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "system_def.h"

typedef int size_t;

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned short UInt16;
typedef unsigned long  dword;
typedef int bool;





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
typedef struct __attribute__ ((packed)) {


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

// tell compiler our int32 function is /* extern */al
///* extern */ void int32(unsigned char intnum, regs16_t *regs);


/* int32_beta */
/* extern */ void int32_beta(unsigned char intnum, regs32_t *regs);
void copy_ap_startup_code(void);

/* cpuid.asm */
/* extern */ bool check_cpuid_supported(void);
bool check_lapic(void);
bool check_msr(void);
/* extern */ bool check_sse(void);
bool check_bsp(void);

/* cpudet.c */
/* extern */ int detect_cpu(void);
/* extern */ void init_cpu(void);
/* MAIN.C */

/* pic.asm */
void disable_pic(void);



void _main();
extern void _main_ap();

/* common.c */
/* extern */ bool find_string(char * string, int len, char * baseAddr, unsigned long length, char ** location);
/* extern */ bool doChecksum(char  *table, size_t length);
/* CONSOLE.C */
/* extern */ void init_video(void);
/* extern */ void puts(char *text);
/* extern */ void putch(unsigned char c);
/* extern */ void cls();
/* extern */ int printf(const char *format, ...);
/* extern */ void anykey();


/* VGA.c */
/* extern */ void put_pixel(int x, int y, byte color);
/* extern */ void line(int x1, int y1, int x2, int y2, byte color);
/* extern */ void set_palette(const byte *palette);
/* extern */ void swap_vga_buffer(void);
/* extern */ void set_text_mode();
/* extern */ void set_unchained_mode(int width, int height);
/* extern */ void set_unchained_mode2(int width, int height);
/* extern */ void flip_screen();
/* extern */ void test_vga();

/* VBE.C */
bool init_vbe();

/* GDT.C */
/* extern */ void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
/* extern */ void gdt_install();

/* IDT.C */
/* extern */ void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
/* extern */ void idt_install();

/* ISRS.C */
/* extern */ void isrs_install();

/* IRQ.C */
/* extern */ void irq_install_handler(int irq, void (*handler)(struct regs *r));
/* extern */ void irq_uninstall_handler(int irq);
/* extern */ void irq_install();

/* TIMER.C */
void init_PIT(void);
void IRQ0_handler(void);
void sleep(unsigned long delay);
/* extern */ void timer_wait(int ticks);
/* extern */ void timer_install();


/* KEYBOARD.C */
/* extern */ void keyboard_install();

/* MATH.C */
/* extern */ int abs(int number);
/* extern */ float fabs(float number);
void hex_to_char(unsigned long number, char * hex_value);

/* MOUSE.C */
/* extern */ void init_mouse();

/* mem.c */
/* extern */ long segToFlatAddr(long segAddr);
/* extern */ void *memcpy(void *dest, const void *src, size_t count);
/* extern */ void *memset(void *dest, char val, size_t count);
/* extern */ unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
/* extern */ size_t strlen(const char *str);
/* extern */ int strcmp(char string1[], char string2[]);
/* extern */ int strncmp(char string1[], char string2[], int n);
/* extern */ const void * memclr(const void * const mem, const size_t count);
/* extern */ void memcpy_SSE2(void *dest, const void *src, size_t count);

/* extern */ void outportb (word _port, byte _data);
/* extern */ void outportw(word _port, word _data);
/* extern */ byte inportb (word _port);
/* extern */ word inportw(word _port);
/* extern */ void showMemory();

/* cpu.c */
/* extern */ void enable_apic(void);
/* extern */ void cpu_get_msr(unsigned int msr, unsigned int *lo, unsigned int *hi);
/* extern */ void cpu_set_msr(unsigned int msr, unsigned int lo, unsigned int hi);


/* mptable.c */
/* extern */ bool check_mp_table();


/* acpi.c */
/* extern */ bool check_rsdp_table();

void _main_ap();

#endif
