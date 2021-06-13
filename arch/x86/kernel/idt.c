/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Interrupt Descriptor Table management
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#include <system.h>
//struct idt_entry idt[256];
//struct idt_ptr idtp;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load(struct idt_ptr * idtp);

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(struct idt_entry * idt, unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    /* The interrupt routine's base address */
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

/* Installs the IDT */
void idt_install(struct idt_entry * idt, struct idt_ptr * idtp)
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp->limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp->base = (unsigned int)idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */



    /* Points the processor's internal register to the new IDT */
    idt_load(idtp);
}