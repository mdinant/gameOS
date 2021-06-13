PREFIX = $(shell export PREFIX="$HOME/opt/cross")
TARGET = $(shell export TARGET=i686-elf)
#export PATH="$PREFIX/bin:$PATH"
#PATH = $(shell export PATH="$HOME/opt/cross/bin:$PATH")

CFLAGS = -Wall -fstrength-reduce -fomit-frame-pointer -fno-asynchronous-unwind-tables -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./include
LINKFLAGS = -ffreestanding -O2 -nostdlib
CC = $$HOME/opt/cross/bin/i686-elf-gcc


BUILD_DIR := ./build
OBJ_DIR := ./obj
ASM_DIR := ./kernel
C_DIR := ./kernel
L_DIR := ./kernel
ISO_DIR := ./iso
WIN_DIR := /mnt/win

.PHONY: all iso link asm c

all: iso

iso: link
	$(shell rm $(ISO_DIR)/myos.iso)
	$(shell sudo cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/kernel.bin)
	$(shell grub-mkrescue -o $(ISO_DIR)/myos.iso iso)
	$(shell sudo cp $(ISO_DIR)/myos.iso $(WIN_DIR)/myos.iso)

link: asm c
	$(CC) -T $(L_DIR)/link2.ld -o $(BUILD_DIR)/kernel.bin $(LINKFLAGS) -lgcc

asm: 
	nasm -f elf32 $(ASM_DIR)/multiboot_header.asm -o $(OBJ_DIR)/multiboot_header.o
	nasm -f elf32 $(ASM_DIR)/start.asm -o $(OBJ_DIR)/start.o
	nasm -f elf32 $(ASM_DIR)/int32_beta.asm -o $(OBJ_DIR)/int32_beta.o
	nasm -f elf32 $(ASM_DIR)/cpuid.asm -o $(OBJ_DIR)/cpuid.o
	nasm -f elf32 $(ASM_DIR)/memasm.asm -o $(OBJ_DIR)/memasm.o
	nasm -f elf32 $(ASM_DIR)/ap_boot.asm -o $(OBJ_DIR)/ap_boot.o
	nasm -f elf32 $(ASM_DIR)/stack.asm -o $(OBJ_DIR)/stack.o
	nasm -f elf32 $(ASM_DIR)/pit.asm -o $(OBJ_DIR)/pit.o
	nasm -f elf32 $(ASM_DIR)/pic.asm -o $(OBJ_DIR)/pic.o
	nasm -f elf32 $(ASM_DIR)/gdt.asm -o $(OBJ_DIR)/gdt_asm.o

c:
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/main.o $(C_DIR)/main.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/main_ap.o $(C_DIR)/main_ap.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/common.o $(C_DIR)/common.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/scrn.o $(C_DIR)/scrn.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/gdt.o $(C_DIR)/gdt.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/idt.o $(C_DIR)/idt.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/isrs.o $(C_DIR)/isrs.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/irq.o $(C_DIR)/irq.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/timer.o $(C_DIR)/timer.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/kb.o $(C_DIR)/kb.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/vga.o $(C_DIR)/vga.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/math.o $(C_DIR)/math.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/mouse.o $(C_DIR)/mouse.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/vbe.o $(C_DIR)/vbe.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/mem.o $(C_DIR)/mem.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/cpudet.o $(C_DIR)/cpudet.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/cpu.o $(C_DIR)/cpu.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/smp.o $(C_DIR)/smp.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/apic.o $(C_DIR)/apic.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/acpi.o $(C_DIR)/acpi.c
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/hpet.o $(C_DIR)/hpet.c

