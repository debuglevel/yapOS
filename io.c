#include "io.h"

//inline: Inhalt der Funktion wird quasi an die Stelle des Aufrufs kopiert, anstatt aufgerufen zu werden.
inline void outb(unsigned short port, unsigned char value)
{
	//Assembler-Code, der nicht "wegoptimiert" werden darf (volatile)
	//%0 etc. sind Platzhalter, die durch die nachfolgenden Operanden ersetzt werden
  asm volatile("out %0,%1"::"a"(value), "Nd" (port));
}

inline unsigned char inb(unsigned short port)
{
  unsigned char ret;
  asm volatile ("in %1,%0":"=a"(ret):"Nd"(port));
  return ret;
}
