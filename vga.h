#ifndef VGA_H
#define VGA_H

void clearScreen();
void fillScreen(unsigned char chr, unsigned char color);
void printCharPos(unsigned char chr, unsigned char color, unsigned char x, unsigned char y, unsigned char mode);
void printStringPos(unsigned char *string, unsigned char color, unsigned char x, unsigned char y, unsigned char mode);
void printString(unsigned char *string, unsigned char color, unsigned char mode);
void updateCurrentPosition();
void moveCursor(unsigned char x, unsigned char y);
void removeCursor();

#define COLOR_NORMAL 0x8F
#define MODE_DRAW_STD 0 //Benutzt die Standardfarbe für Ausgabe
#define MODE_DRAW_FG 2 //Benutzt die übergebene Vordergrundfarbe
#define MODE_DRAW_BG 4 //Benutzt die übergebene Hintergrundfarbe
#define MODE_UPDATE_POSITION 8 //Setzt nach dem Schreiben die aktuelle Position im Speicher

#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80

#endif