#ifndef MEMORY_H
#define MEMORY_H

void memcpy(unsigned char *dest, const unsigned char *src, unsigned int count);

void memset(unsigned char *dest, unsigned char val, int count);
void memsetw(unsigned short *dest, unsigned short val, int count);

int strlen(const unsigned char *string);

#endif
