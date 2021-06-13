#include <system.h>
#include <cpuid.h>


#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800




/** returns a 'true' value if the CPU supports APIC
 *  and if the local APIC hasn't been disabled in MSRs
 *  note that this requires CPUID to be supported.
 */
// bool check_apic() {
//    unsigned int eax, edx;
//    cpuid(1, &eax, &edx);
//    return edx & CPUID_FEAT_EDX_ACPI;
// }

/* Set the physical address for local APIC registers */
void cpu_set_apic_base(unsigned int apic) {
   unsigned int edx = 0;
   unsigned int eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
   edx = (apic >> 32) & 0x0f;
#endif

   cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

/**
 * Get the physical address of the APIC registers page
 * make sure you map it to virtual memory ;)
 */
unsigned int cpu_get_apic_base() {
   unsigned int eax, edx;
   cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);

#ifdef __PHYSICAL_MEMORY_EXTENSION__ // wtf is dit
   return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
#else
   return (eax & 0xfffff000);
#endif
}

void enable_apic() {
    /* Hardware enable the Local APIC if it wasn't enabled */
    cpu_set_apic_base(cpu_get_apic_base());

    /* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
    write_reg(0xF0, ReadRegister(0xF0) | 0x100);
}
