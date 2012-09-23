#include "memory.h"

//Base: Adresse des ISR [ingesamt 4 Bytes/32 Bits]
//Selektor: must point to a valid selector in your GDT [2 Bytes]
//Always0: immer Null [1 Byte]
//Flags: Ähnlich wie Accessbyte [1 Byte]
struct IDTentry
{
    unsigned short base_lo;
    unsigned short sel;        /* Our kernel segment goes here! */
    unsigned char always0;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct IDTptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct IDTentry IDT[256];
struct IDTptr IDTpointer;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load();

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void setIDTgate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    /* We'll leave you to try and code this function: take the
    *  argument 'base' and split it up into a high and low 16-bits,
    *  storing them in idt[num].base_hi and base_lo. The rest of the
    *  fields that you must set in idt[num] are fairly self-
    *  explanatory when it comes to setup */
	
	//Adresse speichern
	IDT[num].base_lo = (base & 0xFFFF);
	IDT[num].base_hi = ((base >> 16) & 0xFFFF);
	
	IDT[num].always0 = 0;
	
	//Selektor speichern
	IDT[num].sel = sel;
	
	//Flags speichern
	IDT[num].flags = flags;
}

char assembleIDTFlagbyte (unsigned char enable, unsigned char priv, unsigned char bits)
{
	char flag = 0;
	
	if (enable == 1)
	{
		//Present: Gibt an ob dieser Eintrag benutzt wird.
		//00000000
		//^
		flag |= 0x80;	//0x80 = 1000 0000b
	}
	
	//DPL: Gibt das Descriptor Privilege Level an, das man brauch um diesen Interrupt aufrufen zu dürfen.
	//00000000
	// ^^
	if (priv == 1)	{
		flag |= 0x20;	//00100000
	} else if (priv == 2)	{
		flag |= 0x40;	//01000000
	} else if (priv == 3)	{
		flag |= 0x60;	//01100000
	}

	//Nullbit
	//00000000
	//   ^ 
	
	//D: Gibt an ob es sich um ein 32bit (1b) oder um ein 16bit (0b) Segment handelt.
	//00000000
	//    ^ 
	if (bits == 0) {
		flag |= 0x00;	
	}else if (bits == 1) {
		flag |= 0x08; //0000 1000
	}
	
	//Type: Sollte bei einem Interruptgate immer 110b sein. 
	//00000000
	//   	 ^^^
	flag |= 0x06;
	
	return flag;
}

/* Installs the IDT */
void initIDT()
{
	//Größe der IDT speichern	(es werden 256 angelegt)
	IDTpointer.limit = (sizeof (struct IDTptr) * 256) - 1;
	//Adresse der IDT speichern
	IDTpointer.base = &IDT;

	//Die gesamte IDT mit Nullen überschreiben
	memset(&IDT, 0, sizeof(struct IDTptr) * 256);

	/* Add any new ISRs to the IDT here using idt_set_gate */

	/* Points the processor's internal register to the new IDT */
	idt_load();
}
