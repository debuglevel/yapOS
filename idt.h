#ifndef IDT_H
#define IDT_H

void initIDT();
void setIDTgate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
char assembleIDTFlagbyte (unsigned char enable, unsigned char priv, unsigned char bits);

#endif
