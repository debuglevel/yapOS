#ifndef SERIAL_H
#define SERIAL_H

void initSerial();
void write_com(unsigned char chr);
unsigned int serial_received();
unsigned int read_serial();

#endif
