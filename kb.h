#ifndef KB_H
#define KB_H

#include "isrs.h"

void keyboard_handler(struct regs *r);
void initKeyboard();
void kbd_set_led_status(unsigned char mask);

#endif
