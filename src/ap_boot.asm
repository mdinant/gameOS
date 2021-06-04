
; code is copied so labels are invalid, use this to fix them
%define REBASE(x) (((x) - startup) + _ap_boot_addr)



extern _ap_boot_addr						; where this code starts at

extern _main_ap								; c func to call when all is ok


global _ap_gdt_pointer						; filled in by bsp, we just copy the bsp's one
global _ap_stack							; filled in by bsp


global _running_flag						; to inform bsp

global _ap_boot_start						; to be copied to RM friendly address by bsp
global _ap_boot_end

SECTION .text

ALIGN 4096									; intel says so
_ap_boot_start:
startup: use16
	cli										; to be sure

	lgdt [REBASE(_ap_gdt_pointer)]
	mov eax, cr0
	or al, 1       							; set PE (Protection Enable) bit in CR0 (Control Register 0)
	;inc eax
	mov cr0, eax

	jmp dword 0x08:REBASE(protected_start)


[BITS 32]
protected_start: use32
	mov  ax, 0x10				; get our 32bit data selector
	mov  ds, ax					; reset ds selector
	mov  es, ax					; reset es selector
	mov  fs, ax					; reset fs selector
	mov  gs, ax					; reset gs selector
	mov  ss, ax					; reset ss selector


	mov esp, _ap_stack			; set stack
	;mov eax, [_running_flag]
	;inc eax
	mov eax, 1
	mov [_running_flag], eax	; inform bsp we are running

	jmp dword 0x08:_main_ap	; NOTE: why do we still need to do a far jump?
	hlt

_ap_gdt_pointer: 				; placeholder, filled in by bsp in early stage. Called in RM so needs to be inside copied code
	dw 0x0000					; table limit (size)
	dd 0x00000000				; table base address

_ap_boot_end:



_ap_stack:
	dd 0x0						; top of stack address, to be filled in by bsp

SECTION .data
	_running_flag dd 0x0		; init to 0 for bsp



