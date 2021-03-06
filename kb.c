#include "kb.h"
#include "io.h"
#include "irq.h"
#include "timer.h"
#include "vga.h"

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
unsigned char kbd_us[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

/* Handles the keyboard interrupt */
struct regs* keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
			//ganz sinnfrei LEDs blinken lassen, wenn man was eingibt ;)
			//durch timer_wait verhakt sich irgendwas im interrupt handling - auskommentiert
			//kbd_set_led_status(scancode);
			
			
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        printChar(kbd_us[scancode]);
    }

	return r;
}

void initKeyboard()
{
	/* Installs 'timer_handler' to IRQ0 */
	irq_install_handler(1, keyboard_handler);
}

void kbd_set_led_status(unsigned char mask)
{
		//timer_wait(100);
	//Wir müssen warten, bis der Controller frei ist
	while((inb(0x64) & 0x02) != 0) {
		//nichts
	}
	
	//Senden, dass wir LED ändern wollen	
	outb(0x60, 0xED);
	
	
	//Auf die Antwort vom Keyboard warten - oder auf Timeout warten
	int i=100;
  while((inb(0x60)!=0xFA) && (i!=0)) {
		timer_wait(1);
		i--;
	}
	
	//Senden, welche LED aktiviert werden sollen
	outb(0x60, mask);
	
	//Auf die Antwort vom Keyboard warten - oder auf Timeout warten
	i=100;
  while((inb(0x60)!=0xFA) && (i!=0)) {
		timer_wait(1);
		i--;
	}

	
}
