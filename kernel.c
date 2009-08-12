#include "vga.h"
#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "memory.h"
#include "irq.h"
#include "serial.h"
#include "timer.h"
#include "kb.h"
#include "string.h"

int main() {
	
	initGDT();
	initIDT();
	initISRs();
	initIRQ();


	
	initTimer();
	initVideo();
	initSerial(1, 38400);
	initSerial(2, 38400);
	initKeyboard();
	//	init_multitasking();
  __asm__ __volatile__ ("sti"); 
	
	unsigned char *hello = KERNEL_BUILD;
	
	printStringPosAttrib(hello, 0x0C, 80-(strlen(hello)+1),24, 2);	//tollen Versionsstring anzeigen
	//printString("test");
	printCharPosAttrib (0x42, 0x7E, 40, 0, MODE_DRAW_FG|MODE_DRAW_BG);

	//int a=1/0;
	/*
	printString("aaa\n");
	printString("bbb\n");
	unsigned char *bla = "abcdefghijk";
	printString("ccc\n");
	int2str(1234567890, bla);
	printString("ddd\n");
	printString(bla);
	printString("\n");
	printString("eee\n");

	
	printString("Testing Wait... ");
	timer_wait(200);
	printString("done\n");
	printString("Testing Wait... ");
	timer_wait(200);
	printString("done\n");*/
	
	
		
  //Kernel in eine Endlosschleife schicken
  while (1) {
		//printCharPosAttrib (0x45, 0x7E, 45, 0, MODE_DRAW_FG|MODE_DRAW_BG);
		//printString("_");
		//__asm__ __volatile__ ("hlt");	//Ruhezustand
	};
	
  return 0;
}
