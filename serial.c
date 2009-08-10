#include "io.h"
#include "vga.h"

// Einige Definitionen
#define IER 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6

#define COM1 0x3F8
#define COM2 0x2F8

/*
 Register der seriellen Schnittstelle
Offset	Lesen/Schreiben			Name

0				rw									Transmitting-Buffer
1				rw 									InterruptEnable-Register
2				r 									InterruptIdentification-Register
2				w 									FIFOControl-Register
3				rw 									LineControl-Register
4				rw 									ModemControl-Register
5				r 									LineStatus-Register
6				r 									ModemStatus-Register
7				rw 									Scratch-Register 
*/

//gibt die zugehörige Portadresse zurück
unsigned short getSerialPort(unsigned char comport) {
	unsigned short base;
	if (comport==1) {
		base=COM1;
	}else if (comport==2){
		base=COM2;
	}
	
	return base;
}

//Funktion zum Initialisieren der seriellen Schnittstelle
void initSerial(unsigned char comport, unsigned short baud) {
	unsigned short base = getSerialPort(comport);
	
	//mit union kann man mehrere Variablen auf den selben Speicherbereich legen
	union {
	  unsigned char byte[2];
	  unsigned short word;
	} teiler;
	
	teiler.word = 115200/baud;

	//Interrupts ausschalten
	outb(base+1, 0x00);

	//DLAB-Bit setzen (eine Art Umschalter)
	outb(base+3, 0x80);

	//Die Baud-Rate wird gesetzt in dem man den Teiler bei BAUD=115200/TEILER setzt.
	//Teiler (low (Endian!)) setzen
	outb(base+0, teiler.byte[0]);

	//Teiler (high (Endian!)) setzen
	outb(base+1, teiler.byte[1]);

	// Anzahl Bits, Parität, usw setzen (DLAB zurücksetzen)
	outb(base+3, 0x03);    // 8 bits, no parity, one stop bit
	//outb(base+3, ((parity & 0x7) << 3) | ((bits-5) & 0x3));

	outb(base+2,0xC7); // Enable FIFO, clear them, with 14-byte threshold
	outb(base+4,0x0B); // IRQs enabled, RTS/DSR set
}

//Gibt 0x20 zurück, wenn man schreiben kann
unsigned int is_transmit_empty() {
	return inb(COM1+LSR)&0x20;
}

//Byte senden
void serial_write(unsigned char comport, unsigned char chr) {
	unsigned short base = getSerialPort(comport);
	
	while (is_transmit_empty(base)==0) { }
	outb(base, chr);
}

//Ob man lesen kann
unsigned int serial_received(unsigned char comport) {
	unsigned short base = getSerialPort(comport);
	
	return inb(base+LSR)&1;
}

//Byte empfangen
unsigned int serial_read(unsigned char comport) {
	unsigned short base = getSerialPort(comport);
	
	while (!serial_received(base)) { }
	return inb(base);
}

void serial_printString(unsigned char comport, unsigned char *string) {
	int no=0;
	for (no=0; no < strlen(string); ++no) {
		serial_write(comport, string[no]);
	}
}
