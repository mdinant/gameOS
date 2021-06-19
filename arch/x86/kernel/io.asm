[BITS 32]

global _inportb_nasm
global _inportw_nasm
global _inportdw_nasm
global _outportb_nasm
global _outportw_nasm
global _outportdw_nasm

_inportb_nasm:
    mov	edx, [esp+4]
    in al, dx
    ret

_inportw_nasm:
    mov	edx, [esp+4]
    in ax, dx
    ret

_inportdw_nasm:
    mov	edx, [esp+4]
    in eax, dx
    ret

_outportb_nasm:
    mov	edx, [esp+4]
    mov eax, [esp+8]
    out dx, al
    ret

_outportw_nasm:
    mov	edx, [esp+4]
    mov eax, [esp+8]
    out dx, ax
    ret

_outportdw_nasm:
    mov	edx, [esp+4]
    mov eax, [esp+8]
    out dx, eax
    ret
