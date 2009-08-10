#include "vga.h"
#include "io.h"

int currentLine = 0;
int currentColumn = 0;
int currentOffset = 0;
//char *video = (char*)0xB8000;
short *videow = (short*)0xB8000;

void clearScreen () {
  fillScreen(0x00, COLOR_NORMAL);
}

void fillScreen (unsigned char chr, unsigned char color) {
	/*while (video < (char*)0xB8000+2*SCREEN_HEIGHT*SCREEN_WIDTH) {
    *video = chr; //mit Zeichen füllen
    video++;
    *video = color; //Attribut
    video++;
  }*/
	memset(video, 0x59, 10);
}

void scroll () {
	memcpy(video, video+80*2, 2*25*80-2*1*80);
	memsetw(video+(25-1)*80, 0x00<<8|COLOR_NORMAL, 80);
}

void printCharPos (unsigned char chr, unsigned char color, unsigned char x, unsigned char y, unsigned char mode) {
  char *video = (char*)0xB8000;
	unsigned char attribute = 0x00;

	video += 2*(y*80+x);	//Offset berechnen

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

	*video = chr;	//Zeichen schreiben

	video++;	//vorwärts zum Attribut-Byte
	*video = attribute;	//Attribut setzen
}

void printChar (unsigned char chr, unsigned char color, unsigned char mode) {
	printCharPos(chr, color, currentColumn, currentLine, mode);
  currentColumn++;
  updateCurrentPosition();
}

void printStringPos(unsigned char *string, unsigned char color, unsigned char x, unsigned char y, unsigned char mode) {
  int writtenLines = 0;
	int writtenColumns = 0;

  while (*string) { //bis der String zu Ende ist, jedes Zeichen ausgeben.
    switch (*string) {
      case 0x0A:
				y++;	//eine Zeile nach unten
				x=0;	//an Anfang der Zeile
				writtenLines++;	//Zähler für geschriebene Linien erhöhen
				writtenColumns=0;	//Zähler für geschriebene Zeichen zurücksetzen
	      break;
      default:
        printCharPos(*string, color, x, y, mode);
				x++;	//ein Zeichen im Videospeicher weiter
				writtenColumns++;	//Zähler für geschriebene Zeichen erhöhen
        break;
    }
    string++; //ein Zeichen weiter im String
  }

	if (mode & MODE_UPDATE_POSITION) {
		currentLine+=writtenLines;
		if (writtenLines > 0) {
			currentColumn=writtenColumns;
		}else{
			currentColumn+=writtenColumns;
		}
	}
}

void printString(unsigned char *string, unsigned char color, unsigned char mode) {
	printStringPos(string, color, currentColumn, currentLine, mode|MODE_UPDATE_POSITION);
  updateCurrentPosition();
}

void updateCurrentPosition() {
	if (currentColumn>79) {
		currentLine=currentLine+currentColumn/80;
		currentColumn=currentColumn%80;
	}
}

void moveCursor(unsigned char x, unsigned char y) {
	int position = y*80+x;	//ergibt etwas wie 1208 (0x04B8) für Zeile 15, Zeichen 8
	outb(0x3D4, 14);	//VGA sagen, dass High Cursor Byte gesetzt werden soll
  outb(0x3D5, position >> 8);	//High Cursor Byte senden (im Beispiel: 0x04) (Bits werden mit Rechtsshift an die richtige Position gerückt)
  outb(0x3D4, 15);	//VGA sagen, dass Low Cursor Byte gesetzt werden soll
  outb(0x3D5, position);	//Low Cursor Byte setzen (im Beispiel: 0xB8)
}

void removeCursor() {
	moveCursor(0, 25);	//Cursor auf eine Zeile setzen, die es nicht gibt.
}
