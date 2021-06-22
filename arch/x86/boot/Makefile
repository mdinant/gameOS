
BINARY_DIR := $$HOME/projects/os/gameOS/arch/x86/boot
DISK_IMAGE_DIR := $$HOME/projects/os/gameOS/image


bootloader:
	nasm -f bin start_lba.asm -o start.bin
	nasm -f bin stage2.asm -o stage2.bin

disk: bootloader
	dd if=$(BINARY_DIR)/start.bin of=$(DISK_IMAGE_DIR)/disk.img conv=notrunc bs=446 count=1
	dd if=$(BINARY_DIR)/start.bin of=$(DISK_IMAGE_DIR)/disk.img conv=notrunc bs=1 count=2 skip=510 seek=510
	dd if=$(BINARY_DIR)/stage2.bin of=$(DISK_IMAGE_DIR)/disk.img conv=notrunc bs=512 count=1 seek=1
