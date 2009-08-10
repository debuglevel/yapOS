global loader ; loader für Linker sichtbar machen
extern main   ; main-Funktion des C-Kernels
 
FLAGS    equ 0
MAGIC    equ 0x1BADB002       ; Magicnumber - Erkennungsmerkmal für Grub
CHECKSUM equ -(MAGIC + FLAGS) ; Checksum
 
section .text
align 4
MultiBootHeader:
  dd MAGIC       ; Magic number
  dd FLAGS       ; Flags
  dd CHECKSUM    ; Checksum
 
loader:
  mov esp,0x200000 ; Stack an die 2MB-Grenze platzieren
  push eax         ; Multiboot Magicnumber auf den Stack legen
  push ebx         ; Adresse der Multiboot-Structure auf den Stack legen
  call main        ; main-Funktion des C-Kernels aufrufen
 
  cli ; falls der Kernel bis hier her kommt, CPU anhalten
  hlt


;;;;;;;;;;;;;;;;;;;;;; GDT ;;;;;;;;;;;;;;;;;;

; This will set up our new segment registers. We need to do
; something special in order to set CS. We do what is called a
; far jump. A jump that includes a segment as well as an offset.
; This is declared in C as 'extern void gdt_flush();'
global gdt_flush     ; Allows the C code to link to this
extern GDTpointer            ; Says that '_gp' is in another file
gdt_flush:
    lgdt [GDTpointer]        ; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               ; Returns back to the C code!


;;;;;;;;;;;;;;;;;;;;;; IDT ;;;;;;;;;;;;;;;;;;

; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
global idt_load
extern IDTpointer
idt_load:
    lidt [IDTpointer]
    ret


;;;;;;;;;;;;;;;;;;;;;; ISR ;;;;;;;;;;;;;;;;;;


; A normal ISR stub that pops a dummy error code to keep a
; uniform stack frame
%macro isr_without_errorcode 1
global isr%1
isr%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common_stub
%endmacro

; Note that we DON'T push a value on the stack in this one!
; It pushes one already! Use this type of stub for exceptions
; that pop error codes!
%macro isr_with_errorcode 1
global isr%1
isr%1:
	cli
	push byte %1
	jmp isr_common_stub
%endmacro


;  0: Divide By Zero Exception
isr_without_errorcode 0
;  1: Debug Exception
isr_without_errorcode 1    
isr_without_errorcode 2
isr_without_errorcode 3
isr_without_errorcode 4
isr_without_errorcode 5
isr_without_errorcode 6
isr_without_errorcode 7
;  8: Double Fault Exception (With Error Code!)
isr_with_errorcode 8
;  9: Coprocessor Segment Overrun Exception
isr_without_errorcode 9
; 10: Bad TSS Exception (With Error Code!)
isr_with_errorcode 10
; 11: Segment Not Present Exception (With Error Code!)
isr_with_errorcode 11
; 12: Stack Fault Exception (With Error Code!)
isr_with_errorcode 12
; 13: General Protection Fault Exception (With Error Code!)
isr_with_errorcode 13
; 14: Page Fault Exception (With Error Code!)
isr_with_errorcode 14
; 15: Reserved Exception
isr_without_errorcode 15
; 16: Floating Point Exception
isr_without_errorcode 16
; 17: Alignment Check Exception
isr_without_errorcode 17
; 18: Machine Check Exception
isr_without_errorcode 18
; 19..31: Reserved
isr_without_errorcode 19
isr_without_errorcode 20
isr_without_errorcode 21
isr_without_errorcode 22
isr_without_errorcode 23
isr_without_errorcode 24
isr_without_errorcode 25
isr_without_errorcode 26
isr_without_errorcode 27
isr_without_errorcode 28
isr_without_errorcode 29
isr_without_errorcode 30
isr_without_errorcode 31


; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern interrupt_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.


isr_common_stub:

	push EAX
	push ECX
	push EDX
	push EBX
	push ESP	;das ist hier eher nicht so sinnvoll. wenn dann eher direkt an den Anfang, da sich der StackPointer ja dauernd verändert. (gerade: von pusha übernommene Reihenfolge)
	push EBP
	push ESI
	push EDI

	push ds
	push es
	push fs
	push gs

	;mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax

	;Die Adresse des Stacks auf den Stack legen
	;Das ist dann wohl der Parameter der C-Funktion
	;mov eax, esp   ; Push us the stack
	;push eax
	push esp

	mov eax, interrupt_handler
	call eax       ; A special call, preserves the 'eip' register


	;'pop eax'   enstpricht   'add esp, 4'
	;hier wird 4 Byte/32bit hochgezählt. Ist wohl der Rückgabewert vom call
	add esp, 4

	pop gs
	pop fs
	pop es
	pop ds

	pop EDI
	pop ESI
	pop EBP
	pop ESP
	pop EBX
	pop EDX
	pop ECX
	pop EAX



	add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!




;;;;;;;;;;;;;;;;;;;;;; IRQ ;;;;;;;;;;;;;;;;;;;;;;;

; Handling ist das selbe wie bei ISRs. Aber es soll für die C-Funktionen
; ASM-Funktionen wie irq0 .. irq15 geben.

%macro irq 2
global irq%1
irq%1:
	isr_without_errorcode %2
%endmacro

irq 0,32 ;IRQ 0
irq 1,33
irq 2,34
irq 3,35
irq 4,36
irq 5,37
irq 6,38
irq 7,39
irq 8,40
irq 9,41
irq 10,42
irq 11,43
irq 12,44
irq 13,45
irq 14,46
irq 15,47 ;IRQ15
