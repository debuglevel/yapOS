CC = gcc
ASM = nasm
LD = ld
DEBUGGER = ddd
QEMU = qemu-system-i386

SRCS = $(shell find -name '*.asm' -o -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))


CFLAGS = -Wall -g -nostdlib -nostartfiles -nodefaultlibs -ffreestanding
LDFLAGS = -T linkerfile
ASMFLAGS = -f elf


all: kernel.bin qemu

qemu: floppy.img
	$(QEMU) -s -fda floppy.img

floppy.img: kernel.bin
	sudo mount -o loop floppy.img floppy/
	cp kernel.bin floppy/
	sudo umount floppy/

debug:
	$(DEBUGGER) kernel.bin --eval-command "target remote localhost:1234"



kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $^

clean:
	rm $(OBJS) kernel.bin

.PHONY: clean



#qemu: copy
#	qemu -s -fda floppy.img

#copy: kernel.bin
#	sudo mount -o loop floppy.img floppy/
#	cp kernel.bin floppy/
#	sudo umount floppy/

#kernel.bin: kernel_asm.o kernel_c.o vga_c.o io_c.o memory_c.o gdt_c.o idt_c.o isrs_c.o irq_c.o timer_c.o kb_c.o serial_c.o string_c.o 
#	$(LD) -T linkerfile -o kernel.bin kernel_asm.o kernel_c.o vga_c.o io_c.o memory_c.o gdt_c.o idt_c.o isrs_c.o irq_c.o timer_c.o kb_c.o serial_c.o string_c.o 


#debug:
#	$(DEBUGGER) kernel.bin --eval-command "target remote localhost:1234"

#kernel_asm.o: kernel.asm
#	$(ASM) -f elf -o kernel_asm.o kernel.asm


#kernel_c.o: kernel.c kernel.h
#	$(CC) -g -ffreestanding -o kernel_c.o -c kernel.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#memory_c.o: memory.c memory.h
#	$(CC) -g -ffreestanding -o memory_c.o -c memory.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#vga_c.o: vga.c vga.h
#	$(CC) -g -ffreestanding -o vga_c.o -c vga.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#io_c.o: io.c io.h
#	$(CC) -g -ffreestanding -o io_c.o -c io.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#gdt_c.o: gdt.c gdt.h
#	$(CC) -g -ffreestanding -o gdt_c.o -c gdt.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#idt_c.o: idt.c idt.h
#	$(CC) -g -ffreestanding -o idt_c.o -c idt.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#isrs_c.o: isrs.c isrs.h
#	$(CC) -g -ffreestanding -o isrs_c.o -c isrs.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#irq_c.o: irq.c irq.h
#	$(CC) -g -ffreestanding -o irq_c.o -c irq.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#timer_c.o: timer.c timer.h
#	$(CC) -g -ffreestanding -o timer_c.o -c timer.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#kb_c.o: kb.c kb.h
#	$(CC) -g -ffreestanding -o kb_c.o -c kb.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#serial_c.o: serial.c serial.h
#	$(CC) -g -ffreestanding -o serial_c.o -c serial.c -Wall -nostdlib -nostartfiles -nodefaultlibs

#string_c.o: string.c string.h
#	$(CC) -g -ffreestanding -o string_c.o -c string.c -Wall -nostdlib -nostartfiles -nodefaultlibs


#clean:
#	rm *.o kernel.bin
