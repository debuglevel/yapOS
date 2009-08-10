#include "vga.h"
#include "kernel.h"

int main() {
  char *hello = KERNEL_BUILD;
	//clearScreen();

	//bunte Zeichen ausgeben
	/*unsigned int chr=0;
	for (chr=0; chr<=255; chr++) {
		printChar(chr, chr, 6);
	}*/

	/*char *newline="\n";
	printString(newline, 0, 0);
	
	for (chr=0; chr<=255; chr++) {
		printChar(chr, 0, 0);
	}*/

	//printStringPos(hello, 0x0C, 71,24, 2);	//tollen Versionsstring anzeigen
	//scroll();
	/*removeCursor();
		
	//memcpy(0xB8000, 0xB8100, 255);
	int len = strlen(hello);	
	printCharPos(len, 0x0C, 71,24, 2);	//tollen Versionsstring anzeigen*/
		
	char *video = (char*)0xB8000;
	memsetw(video, 0x59<<8|0x59, 20);
	
	//memcpy(video+80*2, video, 10);

  //Kernel in eine Endlosschleife schicken
  while (1);
  return 0;
}
