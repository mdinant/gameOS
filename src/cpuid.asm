[bits 32]
%define IA32_APIC_BASE_MSR 					0x1B
%define IA32_APIC_BASE_MSR_BSP 				0x100 // Processor is a BSP
%define IA32_APIC_BASE_MSR_ENABLE 			0x800

global check_cpuid_supported
global check_lapic
global check_msr
global check_sse
global check_bsp

check_cpuid_supported:
	pushfd                               ;Save EFLAGS
    pushfd                               ;Store EFLAGS
    xor dword [esp],0x00200000           ;Invert the ID bit in stored EFLAGS
    popfd                                ;Load stored EFLAGS (with ID bit inverted)
    pushfd                               ;Store EFLAGS again (ID bit may or may not be inverted)
    pop eax                              ;eax = modified EFLAGS (ID bit may or may not be inverted)
    xor eax,[esp]                        ;eax = whichever bits were changed
    popfd                                ;Restore original EFLAGS
    and eax,0x00200000                   ;eax = zero if ID bit can't be changed, else non-zero
    ret

check_lapic:
	mov eax, 1
	cpuid
	test edx, 1<<9
	jz noLapic
	mov eax, 1
	ret
noLapic:
	mov eax, 0
	ret

check_msr:
	mov eax, 1
	cpuid
	test edx, 1<<5
	jz noMsr
	mov eax, 1
	ret
noMsr:
	mov eax, 0
	ret

check_sse:
	mov eax, 0x1
	cpuid
	test edx, 1<<25
	jz noSSE
	test edx, 1<<26
	jz noSSE
	;SSE is available
	;turn it on
	;now enable SSE and the like
	mov eax, cr0
	and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
	or ax, 0x2			;set coprocessor monitoring  CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, eax
	mov eax, 0x1
	ret

 noSSE:
 	mov eax, 0x0
 	ret

check_bsp:
	mov ecx, IA32_APIC_BASE_MSR
	rdmsr
	test eax, 1<<8
	jz noBsp
	mov eax, 1
	ret
noBsp:
	mov eax, 0
	ret

