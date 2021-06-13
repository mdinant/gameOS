

global copy_ap_startup_code
extern _ap_boot_start
extern _ap_boot_end

%define BASE_ADDR                             	0x1000	; 0100:0000 in real
section .text
	copy_ap_startup_code:                        		; by matthijs
		cli                                    			; disable interrupts
		pushad
		mov  esi, _ap_boot_start                        			; set source to code below
		mov  edi, BASE_ADDR	                   			; set destination to new base address
		mov  ecx, (_ap_boot_end - _ap_boot_start)    ; set copy size to our codes size
		cld                                    			; clear direction flag (so we copy forward)
		rep  movsb                             			; do the actual copy (relocate code to low 16bit space)
		popad
		sti
		ret
;		jmp BASE_ADDR                         			; jump to new code location

