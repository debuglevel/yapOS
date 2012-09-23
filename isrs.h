#ifndef ISRS_H
#define ISRS_H

void initISRs();

//Wenn hier irgendwas geändert wird, MUSS ein make clean gemacht werden!

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned long gs, fs, es, ds;      /* pushed the segs last */
    unsigned long edi, esi, ebp, /*esp,*/ ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned long int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned long eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

#endif
