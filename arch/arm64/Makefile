#PREFIX = $(shell export PREFIX="$HOME/opt/cross")
#TARGET = $(shell export TARGET=i686-elf)
#export PATH="$PREFIX/bin:$PATH"
#PATH = $(shell export PATH="$HOME/opt/cross/bin:$PATH")

#CFLAGS = -Wall -fstrength-reduce -fomit-frame-pointer -fno-asynchronous-unwind-tables -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./include
CFLAGS = -Wall -fpic -ffreestanding
LINKFLAGS = -ffreestanding -O2 -nostdlib
CC = $$HOME/opt/cross-arm64/bin/aarch64-elf-gcc
ASM = $$HOME/opt/cross-arm64/bin/aarch64-elf-as
OBJCPY = $$HOME/opt/cross-arm64/bin/aarch64-elf-objcopy

BUILD_DIR := ./build
OBJ_DIR := ./obj
ASM_DIR := ./kernel
C_DIR := ./kernel
L_DIR := ./kernel
#ISO_DIR := ./iso
#WIN_DIR := /mnt/win

.PHONY: all iso link asm c

all: iso

#iso: link
#	$(shell rm $(ISO_DIR)/myos.iso)
#	$(shell sudo cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/kernel.bin)
#	$(shell grub-mkrescue -o $(ISO_DIR)/myos.iso iso)
#	$(shell sudo cp $(ISO_DIR)/myos.iso $(WIN_DIR)/myos.iso)

img: link
	$(OBJCPY) $(BUILD_DIR)/myos.elf -O binary $(BUILD_DIR)/kernel8.img

link: asm c
	$(CC) -T $(L_DIR)/link.ld -o $(BUILD_DIR)/myos.elf $(LINKFLAGS) -lgcc

asm: 
	$(ASM) -c $(ASM_DIR)/boot.S -o $(OBJ_DIR)/boot.o

c:
	$(CC) $(CFLAGS) -c -o $(OBJ_DIR)/kernel.o $(C_DIR)/main.c
