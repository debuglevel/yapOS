#include "irq.h"
#include "isrs.h"
#include "idt.h"
#include "io.h"
#include "vga.h"

/* These are own ISRs that point to our special IRQ handler
*  instead of the regular 'fault_handler' function */
extern struct regs* irq0();
extern struct regs* irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
struct regs* *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
/*
 PIC-Adressen
 
 Befehlsport:
 	Master: 0x20
 	Slave: 0xA0
 
 Datenport:
 	Master: 0x21
 	Slave: 0xA1
 */
void irq_remap(void)
{
	//ICW1: Befehl senden (0x10: Initialisierung einleiten; 0x01: Gesetzt wenn ICW4 gesendet wird)
	/*
	  #define ICW1_ICW4	0x01		   // ICW4 (not) needed
		#define ICW1_SINGLE	0x02		 // Single (cascade) mode
		#define ICW1_INTERVAL4	0x04 // Call address interval 4 (8)
		#define ICW1_LEVEL	0x08		 // Level triggered (edge) mode
		#define ICW1_INIT	0x10		   // Initialization - required!
	*/
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	
	//ICW2: Interrupt für IRQ 0 (und IRQ 0/8 des Slaves) senden
	//Vector Offset
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	
	//?
	//ICW3: 	Master: IRQs, auf denen Slaves gemappt sind (Bitmaske, normal 0x04, [0000100 -> IRQ 2?])
	//				Slave: IRQ, über den der Slave am Master angeschlossen ist (normal 2) 
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	
	/*
	#define ICW4_8086	0x01		// 8086/88 (MCS-80/85) mode
	#define ICW4_AUTO	0x02		// Auto (normal) EOI
	#define ICW4_BUF_SLAVE	0x08		// Buffered mode/slave
	#define ICW4_BUF_MASTER	0x0C		// Buffered mode/master
	#define ICW4_SFNM	0x10		// Special fully nested (not)
 */
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void initIRQ()
{
	irq_remap();

	//Master
	setIDTgate(32, (unsigned)irq0, 0x08, 0x8E);
	setIDTgate(33, (unsigned)irq1, 0x08, 0x8E);
	setIDTgate(34, (unsigned)irq2, 0x08, 0x8E);
	setIDTgate(35, (unsigned)irq3, 0x08, 0x8E);
	setIDTgate(36, (unsigned)irq4, 0x08, 0x8E);
	setIDTgate(37, (unsigned)irq5, 0x08, 0x8E);
	setIDTgate(38, (unsigned)irq6, 0x08, 0x8E);
	setIDTgate(39, (unsigned)irq7, 0x08, 0x8E);

	//Slave
	setIDTgate(40, (unsigned)irq8, 0x08, 0x8E);
	setIDTgate(41, (unsigned)irq9, 0x08, 0x8E);
	setIDTgate(42, (unsigned)irq10, 0x08, 0x8E);
	setIDTgate(43, (unsigned)irq11, 0x08, 0x8E);
	setIDTgate(44, (unsigned)irq12, 0x08, 0x8E);
	setIDTgate(45, (unsigned)irq13, 0x08, 0x8E);
	setIDTgate(46, (unsigned)irq14, 0x08, 0x8E);
	setIDTgate(47, (unsigned)irq15, 0x08, 0x8E);
}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
struct regs* irq_handler(struct regs *r)
{

    /* This is a blank function pointer */
    struct regs* (*handler)(struct regs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        r = handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outb(0x20, 0x20);

	return r;
}









/* Multitasking Zeug */

static char stack_a[4096];
static char stack_b[4096];
static char stack_c[4096];
 
/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct regs* init_task(char* stack, void* entry)
{
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    struct regs new_state = {
        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0,
        .ebp = 0,
        //.esp = unbenutzt (kein Ring-Wechsel)
        .eip = (unsigned long) entry,
 
        /* Ring-0-Segmentregister */
        .cs  = 0x08,
        //.ss  = unbenutzt (kein Ring-Wechsel)
 
        /* IRQs einschalten (IF = 1) */
        .eflags = 0x202,
    };
 
    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    struct regs* state = (void*) (stack + 4096 - sizeof(new_state));
    *state = new_state;
 
    return state;
}


static int current_task = -1;
static int num_tasks = 3;
static struct regs* task_states[3];
 
void init_multitasking(void)
{
    task_states[0] = init_task(stack_a, task_a);
    task_states[1] = init_task(stack_b, task_b);
		task_states[2] = init_task(stack_c, task_c);
}

/*
 * Gibt den Prozessorzustand des naechsten Tasks zurueck. Der aktuelle
 * Prozessorzustand wird als Parameter uebergeben und gespeichert, damit er
 * beim naechsten Aufruf des Tasks wiederhergestellt werden kann
 */
struct regs* schedule(struct regs *r)
{
    /*
     * Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
     * gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
     * wir spaeter nicht wieder.
     */
    if (current_task >= 0) {
        task_states[current_task] = r;
    }
 
    /*
     * Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
     */
	
	if (task_states[0]!=0)
	{
		do
		{
    	current_task++;
    	current_task %= num_tasks;
		}while(task_states[current_task] == 0);
 
    /* Prozessorzustand des neuen Tasks aktivieren */
    r = task_states[current_task];
	}
	
		
    return r;
}



void task_a(void)
{
	printCharPosAttrib (0x41, 0x7E, 45, 0, MODE_DRAW_FG|MODE_DRAW_BG);
	printCharPosAttrib (0x44, 0x7E, 45, 0, MODE_DRAW_FG|MODE_DRAW_BG);
    while (1) {
			//printCharPosAttrib (0x45, 0x7E, 45, 0, MODE_DRAW_FG|MODE_DRAW_BG);
//			printCharPosAttrib (0x42, 0x7E, 1, 2, MODE_DRAW_FG|MODE_DRAW_BG);
				//printStringPosAttrib("hallo", 0x0C, 80-(strlen("hallo")+1),24, 2);	//tollen Versionsstring anzeigen

    }
}
 
void task_b(void)
{
    while (1) {
        //printString("B");
    }
}

void task_c(void)
{
    while (1) {
        //printString("C");
    }
}
