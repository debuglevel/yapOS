#include "vga.h"
#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "memory.h"

int main() {
	
	initGDT();
	initIDT();
	initISRs ();
  char *hello = KERNEL_BUILD;
	initVideo();

	

	
	printStringPosAttrib(hello, 0x0C, 80-(strlen(hello)+1),24, 2);	//tollen Versionsstring anzeigen

	
	int a=0;
	a=1/0;
	


  //Kernel in eine Endlosschleife schicken
  while (1);
  return 0;
}
