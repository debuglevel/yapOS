#include "string.h"

/* Gibt die Länge eines Strings zurück */
int strlen(const unsigned char *string)
{
	int count=0;
	for (count=0; *string!='\0'; count++)
	{
		string++;
	}
	
	return count;
}

unsigned char *int2str(signed int zahl, unsigned char *text)
{
	unsigned char sign;
	//unsigned char text[11] = "\0\0\0\0\0\0\0\0\0\0\0";
	signed char ziffern[10];
	
	signed char no=0;
	for (no=0; no<10; no++) {
		ziffern[no]=-1;
	}
	for (no=0; no<11; no++) {
		text[no]='\0';
	}

	if (zahl < 0) {
		sign = '-';
		zahl = zahl * -1;
	}	else {
		sign = '+';
	}
	
	for (no=0; zahl!=0; no++) {
		ziffern[no]=zahl%10;
		zahl=zahl/10;
	}
	
	unsigned char stelle=1;
	text[0]=sign;
	for (no=9; no>=0; no--) {
		if (ziffern[no]>=0) {
			text[stelle]=ziffern[no]+'0';
			stelle++;
		}
	}
	
	return;
}
