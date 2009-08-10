#ifndef IRQ_H
#define IRQ_H

void keyboard_handler(struct regs *r);
void initKeyboard();
void kbd_set_led_status(char mask);

#endif
