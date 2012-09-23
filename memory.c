#include "memory.h"

/* COUNT Bytes von SRC nach DEST kopieren */
void memcpy(unsigned char *dest, const unsigned char *src, unsigned int count)
{
	int pos=0;
	for(pos=0; pos < count; pos++)
	{
		*dest = *src;
		dest++;
		src++;
	}
}

/* Setzte COUNT Bytes ab DEST auf VAL */
void memset(unsigned char *dest, unsigned char val, int count)
{
	int no=0;
	for(no=0; no < count; no++)
	{
		*dest = val;
		dest++;
	}
}

/* Setzte COUNT Words ab DEST auf VAL */
void memsetw(unsigned short *dest, unsigned short val, int count)
{
	int no=0;
	for(no=0; no < count; no++)
	{
		*dest = val;
		dest++;
	}
}


