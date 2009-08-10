#include "vga.h"
#include "kernel.h"

int main() {
	
	gdt_install();
	
  char *hello = KERNEL_BUILD;
	initVideo();

	

	
	//printStringPosAttrib(hello, 0x0C, 80-strlen(hello),24, 2);	//tollen Versionsstring anzeigen
	printStringPosAttrib(hello, 0x0C, 0,1, MODE_UPDATE_POSITION);	//tollen Versionsstring anzeigen
	printString ("1");
	printString ("2");
	printString ("3\n");
	printString ("4\n");
	printString ("5\n");
	printString ("6\n");
	printString ("7\n");
	printString ("8\n");
	printString ("9\n");
	printString ("0\n");
	printString ("1\n");
	printString ("2\n");
	printString ("3\n");
	printString ("4\n");
	printString ("5\n");
	printString ("6\n");
	printString ("7\n");
	printString ("8\n");
	printString ("9\n");
	printString ("0\n");
	printString ("1\n");
	printString ("2\n");
	printString ("3\n");
	printString ("4\n");
	printString ("5\n");
	printString ("6\n");
	printString ("123\t456\n");
	
	
	


  //Kernel in eine Endlosschleife schicken
  while (1);
  return 0;
}
