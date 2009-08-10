#include "timer.h"
#include "io.h"
#include "irq.h"
#include "vga.h"

/* This will keep track of how many ticks that the system
*  has been running for */
unsigned int timer_ticks = 0;

//Timer-Frequenz einstellen
void set_timer_frequency(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
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

    if (timer_ticks % 50 == 0)
    {
        //printString("0.5 second passed\n");
    }
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void initTimer()
{
	set_timer_frequency(100);	//auf 100Hz setzen
		
	/* Installs 'timer_handler' to IRQ0 */
	irq_install_handler(0, timer_handler);
}


/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned int eticks;

    eticks = timer_ticks + ticks;
		unsigned char i=0;
	
		//keine Ahnung, ob das so gut und richtig ist. Wenn ich keine IRQs zulasse, funktioniert der Timer nicht mehr...
	  //__asm__ __volatile__ ("cli"); 
    while(timer_ticks < eticks){
			//printChar(i++);
			__asm__ __volatile__ ("hlt");	//Ruhezustand
		}
		//__asm__ __volatile__ ("sti"); 
	  
}

