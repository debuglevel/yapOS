#ifndef IRQ_H
#define IRQ_H

void initIRQ();
void irq_install_handler(int irq, void (*handler)(struct regs *r));

#endif
