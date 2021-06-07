/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Timer driver
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#include <system.h>

/* This will keep track of how many ticks that the system
*  has been running for */
uint64_t timer_ticks = 0;

extern unsigned long system_timer_fractions;
extern unsigned long system_timer_ms;


void sleep(unsigned long delayms) {
	unsigned long oldTime = system_timer_ms;
	while ((system_timer_ms - oldTime) < delayms) {

	}
	//printf("%u ms passed\n", delayms);
}

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;
    //printf("ticks: %u\n", timer_ticks);
    //IRQ0_handler();
}

uint64_t getTicks()
{
    return timer_ticks;
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}
static void timerInit(uint32_t frequency)
{

   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
   outportb(0x43, 0x36);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

   // Send the frequency divisor.
   outportb(0x40, l);
   outportb(0x40, h);
} 
/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
	//init_PIT();
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
    timerInit(50);
}

