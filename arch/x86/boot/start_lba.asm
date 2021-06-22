BITS 16

bootloader_main:
    mov ax, 0x07C0      ; Set data segment to bootloader's default segment
    mov ds, ax

;    mov ah, 0x02        ; BIOS int13h "read sector" function
;    mov al, 1           ; Number of sectors to read
;    mov cl, 2           ; Sector to read
;    mov ch, 0           ; Cylinder/track
;    mov dh, 0           ; Head
;    mov bx, program_seg ; load program at program_seg:0
;    mov es, bx
;    xor bx, bx
;    int 0x13


 
	mov si, DAPACK		; address of "disk address packet"
	mov ah, 0x42		; AL is unused
	;mov dl, 0x80		; drive number 0 (OR the drive # with 0x80)
	int 0x13

    mov ax, program_seg
    mov ds, ax
    mov ss, ax          ; set stack to end of program_seg
    mov sp, 0
    jmp program_seg:0
    jmp $

program_seg equ (bootloader_end - bootloader_main + 0x7c00 + 15) / 16

align 16
DAPACK:
	db	0x10
	db	0
blkcnt:	dw	16		; int 13 resets this to # of blocks actually read/written
db_add:	dw	0		; memory buffer destination address (0:7c00)
	dw	program_seg		; in memory page zero
d_lba:	dd	1		; put the lba to read in this spot
	dd	0		; more storage bytes only for big lba's ( > 4 bytes )

bootloader_end:


times 510-($-$$) db 0       ; Pad rest of sector and add bootloader   
dw 0xAA55                   ;   signature