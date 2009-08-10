CC = gcc
ASM = nasm
LD = ld
DEBUGGER = ddd

qemu: copy
	qemu -s -fda floppy.img

copy: kernel.bin
	sudo mount -o loop floppy.img floppy/
	cp kernel.bin floppy/
	sudo umount floppy/

kernel.bin: kernel_asm.o kernel_c.o vga_c.o io_c.o memory_c.o gdt_c.o
	$(LD) -T linkerfile -o kernel.bin kernel_asm.o kernel_c.o vga_c.o io_c.o memory_c.o gdt_c.o


debug:
	$(DEBUGGER) kernel.bin --eval-command "target remote localhost:1234"

kernel_asm.o: kernel.asm
	$(ASM) -f elf -o kernel_asm.o kernel.asm


kernel_c.o: kernel.c kernel.h
	$(CC) -g -ffreestanding -o kernel_c.o -c kernel.c -Wall -nostdlib -nostartfiles -nodefaultlibs

memory_c.o: memory.c memory.h
	$(CC) -g -ffreestanding -o memory_c.o -c memory.c -Wall -nostdlib -nostartfiles -nodefaultlibs

vga_c.o: vga.c vga.h
	$(CC) -g -ffreestanding -o vga_c.o -c vga.c -Wall -nostdlib -nostartfiles -nodefaultlibs

io_c.o: io.c io.h
	$(CC) -g -ffreestanding -o io_c.o -c io.c -Wall -nostdlib -nostartfiles -nodefaultlibs

gdt_c.o: gdt.c gdt.h
	$(CC) -g -ffreestanding -o gdt_c.o -c gdt.c -Wall -nostdlib -nostartfiles -nodefaultlibs


clean:
	rm *.o kernel.bin
