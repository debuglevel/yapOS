//Limit: Die Größe des Segments (in Bytes oder Pages (4KiB)) [ingesamt 20 Bits]
//Base: Adresse, an dem das Segment beginnt [insgesamt 4 Byte/32 Bits]
//Access: Zugriffsinformationen [1 Byte/8 Bit]
//Flags: Körnung/Granularity (Bytes oder Pages) und ...? [4 Bits]
struct GDTentry
{
	//2 Bytes: Limit
    unsigned short limit_low;
	//3 Bytes: Base
    unsigned short base_low;
    unsigned char base_middle;
	//1 Byte: Access
    unsigned char access;
	//1 Byte: Flags (4 Bits) & Limit (4 Bits)
    unsigned char granularity;
	//1 Byte: Base
    unsigned char base_high;
} __attribute__((packed));	//damit der Compiler nichts wegoptimiert


struct GDTptr
{
	//2 Byte: Die Größe der GDT (in Bytes) minus 1
    unsigned short limit;
	//4 Byte: Die Adresse der GDT
    unsigned int base;
} __attribute__((packed));	//damit der Compiler nichts wegoptimiert

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct GDTentry GDT[3];

//Der Zeiger auf den GDT, der dem Prozessor übergeben wird.
//struct gdt_ptr gp;
struct GDTptr GDTpointer;

/* This will be a function in start.asm. We use this to properly
*  reload the new segment registers */
extern void gdt_flush();

//einen Eintrag in der GDT machen
void setGDTgate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	/* Anfangssdresse des Eintrags speichern */
	GDT[num].base_low = (base & 0xFFFF);	//die ersten 2 Bytes für die Base speichern
	GDT[num].base_middle = (base >> 16) & 0xFF;	//das dritte Byte speichern
	GDT[num].base_high = (base >> 24) & 0xFF;	//das vierte Byte speichern

	/* Größe des Eintrags speichern */
	GDT[num].limit_low = (limit & 0xFFFF);	//die ersten 2 Bytes
	GDT[num].granularity = ((limit >> 16) & 0x0F);	//das letzte Halb-Byte

	/* Körnung/Granularity speichern */
  GDT[num].granularity |= (gran & 0xF0);	//Halb-Byte speichern
	GDT[num].access = access;
}

char assembleAccessbyte(char enable, char privilege, char code, char read_write){
	char accessbyte = 0x00;
		
	//Present bit: Für aktive Einträge muss es 1 sein.
	//0000 0000b
	//^
	if (enable == 1){
		accessbyte |= 0x80;	//0x80 = 1000 0000b
	}
	
	//Privilege: Gibt den Ring an.
	//0000 0000
	// ^^
	if (privilege == 1) {
		accessbyte |= 0x20;	//0x20 = 0010 0000b
	}	else if (privilege == 2) {
		accessbyte |= 0x40;	//0x40 = 0100 0000b
	}	else if(privilege == 3) {
		accessbyte |= 0x60; //0x60 = 0110 0000b
	}
	
	//1: immer 1
	//0000 0000
	//   ^
	accessbyte |= 0x10;	//muss immer 1 sein
	
	//Executable Code oder Data
	//0000 0000
	//     ^
	if (code == 1){
		accessbyte |= 0x08;
	}
	
	//Direction: Zeigt an, in welche Richtung die GDT wächst. (0: nach oben; 1: nach unten)
	//Einfach auf 0 lassen - ist anschaulicher.
	//0000 0000
	//      ^
	
	//Read/Write
	//Bei Code: Ob es lesbar ist	(Write immer verboten)
	//Bei Data: Ob es schreibbar ist	(Read immer erlaubt)
	//0000 0000
	//       ^
	if (read_write == 1){
		accessbyte |= 0x02;
	}
	
	//Accessed Bit: Wird intern vom Prozessor verwendet - auf 0 lassen
	//0000 0000
	//        ^
	
	return accessbyte;
}

char assembleFlagbyte(char gran, char size)
{
	char flagbyte=0x00;
	
	//Körnung: 0: Byte; 1: 4KiB
	if(gran==1) {
		flagbyte |= 0x40;
	}
	
	//Size Bit: 0: 16bit; 1: 32bit
	if(size==1) {
		flagbyte |= 0x80;
	}
	
	return flagbyte;
}


/* Should be called by main. This will setup the special GDT
*  pointer, set up the first 3 entries in our GDT, and then
*  finally call gdt_flush() in our assembler file in order
*  to tell the processor where the new GDT is and update the
*  new segment registers */
/*
 - Initialisiert einen Zeiger auf die GDT
 - Setzt drei GDT-Einträge auf
 - Ruft eine ASM-Funktion auf, um dem Prozessor die GDT mitzuteilen
 */
void initGDT()
{
	/* GDT-Adresse und Größe speichern */
	GDTpointer.limit = (sizeof(struct GDTentry) * 3) - 1;
	GDTpointer.base = &GDT;

	/* NULL-Descriptor setzen */
	setGDTgate(0, 0, 0, 0, 0);

	/* The second entry is our Code Segment. The base address
	*  is 0, the limit is 4GBytes, it uses 4KByte granularity,
	*  uses 32-bit opcodes, and is a Code Segment descriptor.
	*  Please check the table above in the tutorial in order
	*  to see exactly what each value means */
	
	char accessbyte;
	char flagbyte;
	
	accessbyte = assembleAccessbyte(1, 0, 1, 1);
	flagbyte = assembleFlagbyte(1, 1);
	setGDTgate(1, 0, 0xFFFFFFFF, accessbyte, flagbyte);

	/* The third entry is our Data Segment. It's EXACTLY the
	*  same as our code segment, but the descriptor type in
	*  this entry's access byte says it's a Data Segment */
	accessbyte = assembleAccessbyte(1, 0, 0, 1);
	setGDTgate(2, 0, 0xFFFFFFFF, accessbyte, flagbyte);

	gdt_flush();	//per ASM-Aufruf dem Prozessor die neue GDT bekannt machen
}
