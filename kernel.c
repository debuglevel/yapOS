#include "vga.h"
#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "memory.h"
#include "irq.h"

int main() {
	
	initGDT();
	initIDT();
	initISRs ();
	initIRQ();
	initTimer();
	initVideo();
	initKeyboard();
  char *hello = KERNEL_BUILD;
	
	
	
	printStringPosAttrib(hello, 0x0C, 80-(strlen(hello)+1),24, 2);	//tollen Versionsstring anzeigen

	
	int a=0;
	//a=1/0;
	__asm__ __volatile__ ("sti"); 
	
	
	
	printString("Booting up");
	int i=0;
	for (i=0; i<20; i++) {
		timer_wait(50);
		printString(".");
		kbd_set_led_status(i);	//LEDs blinken lassen
	}
	printString(" just kidding ;-)\n");
	
	
  //Kernel in eine Endlosschleife schicken
  while (1);
  return 0;
}
