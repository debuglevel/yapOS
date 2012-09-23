#include "vga.h"
#include "io.h"
#include "memory.h"

unsigned int currentLine = 0;
unsigned int currentColumn = 0;
unsigned int currentOffset = 0;
unsigned char *video = (unsigned char*)0xB8000;
unsigned short *videow = (unsigned short*)0xB8000;

//Initialisiert die VideoAusgabe
void initVideo () {
	clearScreen();
	removeCursor();
}

//resetet den Bildschirm
void clearScreen () {
  fillScreen(0x00, COLOR_NORMAL);
}

//füllt den Bildschirm mit CHR in der Farbe COLOR
void fillScreen (unsigned char chr, unsigned char color) {
	//Achtung: Wegen Big-Endian & Short muss das /Attribut/ um 8Bits nach 'links' verschoben werden.
	//Eigentlich ist die Reihenfolge 1 Byte 'Zeichen', 1 Byte Attribut.
	short filling = chr|color<<8;
	memsetw(videow, filling, SCREEN_WIDTH*SCREEN_HEIGHT);
}

//schiebt den Bildschirminhalt um eine Zeile nach oben
void scroll () {
	//Inhalt des Speichers ab der zweiten Zeile (video+SCREEN_WIDTH*2) an den Anfang kopieren.
	memcpy(video, video+2*SCREEN_WIDTH, 2*(SCREEN_HEIGHT-1)*SCREEN_WIDTH);
	//Die letzte Zeile füllen
	short blank = 0x00|COLOR_NORMAL<<8;
	memsetw(videow+(SCREEN_HEIGHT-1)*SCREEN_WIDTH, blank, SCREEN_WIDTH);
	currentLine=24;
	currentColumn=0;
}

void checkScroll() {
	if (currentLine >= SCREEN_HEIGHT) {
		scroll();
	}
}

//Zeichen mit Attribut an Position schreiben
void printCharPosAttrib (unsigned char chr, unsigned char color, unsigned char x, unsigned char y, unsigned char mode) {
	unsigned short *offset = videow;
	unsigned char attribute = 0x00;

	offset += (y*80+x);	//Offset berechnen

	//Attribut-Byte anhand von Mode und Color zusammensetzen.
	if (mode & MODE_DRAW_FG) {	//Vordergrundfarbe setzen
		attribute = attribute | (color & 0x0F);	//die hinteren 4 Bits von color werden zu attribute hinzugefügt
	}else{
		attribute = attribute | (COLOR_NORMAL & 0x0F);
	}

	if (mode & MODE_DRAW_BG) { //Hintergrundfarbe setzen
		attribute = attribute | (color & 0xF0);	//die vorderen 4 Bits von color werden zu attribute hinzugefügt
	}else{
		attribute = attribute | (COLOR_NORMAL & 0xF0);
	}

	//absolut positioniert
	/*if (absolute == 1)
	{
		*offset = chr | attribute<<8;
	}
	else
	{*/
		switch (chr) {
			case '\n':
				currentLine++;	//Zähler für aktuelle Zeile erhöhen
				currentColumn=0;	//Zähler für aktuelle Spalte zurücksetzen
				break;
			case '\r':
				currentColumn=0;
				break;
			case '\a':
				break;
			case 0x08:
				if (currentColumn>0) {
					currentColumn--;
				}
				break;
			default:
				*offset = chr | attribute<<8;
				
				currentColumn++;	//Zähler für aktuelle Spalte erhöhen
				
				break;
		}
		updateCurrentPosition();
		checkScroll();
	//}
	
}

//Zeichen an Position schreiben
void printCharPos (unsigned char chr, unsigned char x, unsigned char y) {
	printCharPosAttrib(chr, COLOR_NORMAL, x, y, MODE_DRAW_STD);
}

//Zeichen mit Attribut schreiben
void printCharAttrib (unsigned char chr, unsigned char color, unsigned char mode) {
	printCharPosAttrib(chr, color, currentColumn, currentLine, mode);
}

//Zeichen schreiben
void printChar (unsigned char chr) {
	printCharAttrib(chr, COLOR_NORMAL, MODE_DRAW_STD);
}

//String mit Attribut an Position schreiben
void printStringPosAttrib(unsigned char *string, unsigned char color, unsigned char x, unsigned char y, unsigned char mode) {
	unsigned int savedColumn = currentColumn;
	unsigned int savedLine = currentLine;
	
	currentColumn = x;
	currentLine = y;
	
	int no=0;
	for (no=0; no < strlen(string); ++no) {
		printCharPosAttrib (string[no], color, currentColumn, currentLine, MODE_DRAW_FG);
	}
	
	
	if (!(mode & MODE_UPDATE_POSITION)) {	//wenn die Zeigerposition aktualisiert werden soll
		currentColumn = savedColumn;
		currentLine = savedLine;
	}
}

//String mit Attribut schreiben
void printStringAttrib(unsigned char *string, unsigned char color, unsigned char mode) {
	printStringPosAttrib(string, color, currentColumn, currentLine, mode|MODE_UPDATE_POSITION);
  
	/*int no=0;
	for (no=0; no < strlen(string); ++no) {
		printCharAttrib(string[no], color, mode);
	}*/
}

//String schreiben
void printString(unsigned char *string) {
	printStringAttrib(string, COLOR_NORMAL, MODE_DRAW_STD);
}

//aktualisiert nach der letzten Spalte die Zeilen
void updateCurrentPosition() {
	if (currentColumn>SCREEN_WIDTH-1) {
		currentLine = currentLine + currentColumn/SCREEN_WIDTH;
		currentColumn = currentColumn % SCREEN_WIDTH;
	}
}

//verschiebt den Cursor
void moveCursor(unsigned char x, unsigned char y) {
	int position = y*80+x;	//ergibt etwas wie 1208 (0x04B8) für Zeile 15, Zeichen 8
	outb(0x3D4, 14);	//VGA sagen, dass High Cursor Byte gesetzt werden soll
  outb(0x3D5, position >> 8);	//High Cursor Byte senden (im Beispiel: 0x04) (Bits werden mit Rechtsshift an die richtige Position gerückt)
  outb(0x3D4, 15);	//VGA sagen, dass Low Cursor Byte gesetzt werden soll
  outb(0x3D5, position);	//Low Cursor Byte setzen (im Beispiel: 0xB8)
}

//blendet den Cursor aus
void removeCursor() {
	moveCursor(0, 25);	//Cursor auf eine Zeile setzen, die es nicht gibt.
}
