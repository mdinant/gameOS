/**
 * 
 * Doesnt work yet, can't get all registers I am interested in to give a normal value
 * 
 * 
*/


#include <system.h>
#include <acpi.h>



#define LEGACY_REPLACEMENT_BIT 15
#define COUNT_SIZE_CAP 13
#define NUM_TIM_CAP 8
#define T0_32_MODE_CNF 8
#define T0_SIZE_CAP 5
#define T0_PER_INT_CAP 4
#define T0_TYPE_CNF 3
#define T0_INT_ENB_CNF 2
#define T0_INT_TYPE_CNF 1
#define LEG_RT_CNF 1
#define ENABLE_CNF 0
#define T0_INT_STS 0



// typedef struct
// {
//     uint8_t RevId:8;
//     uint8_t NumTimCap : 5;
//     uint8_t CountSizeCap : 1;
//     uint8_t Reserved : 1;
//     uint8_t LegRtCap : 1;
//     uint16_t VendorId:16;
//     uint32_t CounterClockPeriod:32;
// } __attribute__((packed)) GEN_CAP_ID_REG;

typedef struct
{
    uint64_t GenCapIdReg;
    uint64_t : 64;
    uint64_t GenConfReg;
    uint64_t : 64;
    uint64_t GenIntStatReg;
    uint64_t : 64;
    uint32_t MainCountValRegLow;
    uint32_t MainCountValRegHigh;
    uint64_t : 64;
    uint64_t Tim0ConfCapReg;
    uint64_t Tim0CompValReg;
    uint64_t Tim0FsbIntRouteReg;
    uint64_t : 64;
    uint64_t Tim1ConfCapReg;
    uint64_t Tim1CompValReg;
    uint64_t Tim1FsbIntRouteReg;
    uint64_t : 64;
    uint64_t Tim2ConfCapReg;
    uint64_t Tim2CompValReg;
    uint64_t Tim2FsbIntRouteReg;
    /* Rest is for additional timers */
} __attribute__((packed)) TIMER_REGISTER_SPACE;

void testHPET()
{
    uint32_t addr;
    if (getACPITableAddr(HPET_SIGNATURE, &addr) == FALSE)
    {
        perror("Could not find HPET table addr\n");
    }
    else
    {
        HPET *hpet = (HPET *)(addr);
        printf("hpet sequence number: %u\n", hpet->HpetNumber);

        /* cast to 32 pointer, again see acpi for example, we do this all the time, we ASSUME < 4gb */
        TIMER_REGISTER_SPACE *trs = (TIMER_REGISTER_SPACE *)((uint32_t)(hpet->BaseAddress.Address));
        //GEN_CAP_ID_REG gcig = (GEN_CAP_ID_REG)(trs->GenCapIdReg);

        // if (trs->GenCapIdReg.bits == 0) {
        //     printf("BITS IS 0\n");
        // }
        //uint8_t temp = 0xFF;
//        uint8_t temp = ((trs->GenCapIdReg) >> NUM_TIM_CAP) & 0x1f;
        printf("MainCounter low: %u\n", trs->MainCountValRegLow);
        printf("MainCounter high: %u\n", trs->MainCountValRegHigh);

        printf("timer capable 64 mode: %u\n", ((trs->GenCapIdReg) >> COUNT_SIZE_CAP) & 1UL);
//        printf("NUM_TIM_CAP: %u\n", temp);
        printf("Legacy Replacement Route: %u\n", ((trs->GenConfReg) >> LEG_RT_CNF) & 1UL);
        printf("GenConfReg: %u\n", trs->GenConfReg);
//        printf("Tim0 supports periodic mode %u\n", ((trs->Tim0ConfCapReg) >> T0_PER_INT_CAP) & 1);
        printf("GenIntStatReg %u\n", (trs->GenIntStatReg >> T0_INT_STS) & 1UL);

        // printf("number of comparators: %u\n", hpet->NumComparators);
        // printf("base addr: %u\n", (uint32_t)hpet->BaseAddress.Address);
        // printf("Hpet period: %u\n", trs->GenCapIdReg >> 32);
        // printf("legacy mode: %u\n", ((trs->GenCapIdReg) & 0x8000) >> 15);
        // printf("legacy mode: %u\n", ((trs->GenCapIdReg) >> LEGACY_REPLACEMENT_BIT) & 1);
        // printf("main counter val: %u\n", ((trs->MainCountValReg)));
        // printf("main counter val: %u\n", ((trs->MainCountValReg)));
        // printf("main counter val: %u\n", ((trs->MainCountValReg)));
        // printf("timer in 32 mode? %u\n", ((trs->Tim0ConfCapReg) >> T0_32_MODE_CNF) & 1);
        // //trs->Tim0ConfCapReg |= 0x100;
        // printf("timer in 32 mode? %u\n", ((trs->Tim0ConfCapReg) >> T0_32_MODE_CNF) & 1);
        // printf("timer size cap %u\n", ((trs->Tim0ConfCapReg) >> T0_SIZE_CAP) & 1);
        // printf("timer firing ints %u\n", ((trs->Tim0ConfCapReg) >> T0_INT_ENB_CNF) & 1);

        trs->GenConfReg &= ~(1UL << 0); // ENABLE_CNF //  •0 – Halt main count and disable all timer interrupts
        //                                     //  •1 – allow main counter to run, and allow timer interrupts if enabled

        trs->GenConfReg |= 0x2; // (try to) set Legacy Replacement Route

        trs->Tim0ConfCapReg |= (1UL << T0_INT_TYPE_CNF); // set level triggered, write to clear in interupt reg

        trs->Tim0ConfCapReg |= (1UL << T0_32_MODE_CNF);           // force 64 timer to behave 32
        // check 32 mode set
        printf("timer 32 mode %u\n", (trs->Tim0ConfCapReg >> T0_32_MODE_CNF) & 1);
        trs->Tim0ConfCapReg &= ~(1UL << T0_TYPE_CNF);   // clr bit to set non-periodic mode 
        trs->Tim0ConfCapReg |= 0x4; // Tn_INT_ENB_CNF
                                    // Timer n Interrupt Enable:  (where n is the timer number:  00 to 31).
                                    // This read/write bit must be set to enable timer n to cause an interrupt when the timer event fires.
                                    // Note: If this bit is 0, the timer will still operate and generate appropriate status bits, but will not cause an interrupt.
        trs->Tim0CompValReg = 500000000; // set counter ticks


        trs->GenConfReg |= 0x1; // ENABLE_CNF
        
        printf("GenConfReg: %u\n", trs->GenConfReg);
        printf("Legacy Replacement Route: %u\n", ((trs->GenConfReg) >> LEG_RT_CNF) & 1);

        // printf("main counter val: %u\n", ((uint32_t)(trs->MainCountValReg)));
        printf("GenIntStatReg %u\n", (trs->GenIntStatReg >> T0_INT_STS) & 1UL);

    }
}
