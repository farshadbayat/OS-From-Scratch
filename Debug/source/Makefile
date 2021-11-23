######################Variable######################
ASM = nasm 
CC  = gcc
GXX = gxx
LD  = ld
DIR = src
O2B = objcopy
######################Entery Point######################
ENTERY  = start

######################Directory######################
INCLUDE = Include
LIBRARY	= Library

GCCLIB=CTYPE.C MATH.C 
SRC = libc.c idt.c gdt.c isrs.c irq.c task.c 

##Get Object File String
GCCOBJ=$(GCCLIB:%.C=%.o)
OBJ = $(SRC:%.c=%.o) CKernel.o COMPONENT1.o

LIB =$(LIBRARY)/KEYBOARD.C $(LIBRARY)/STRING.C $(LIBRARY)/STDIO.C $(LIBRARY)/CONIO.C $(LIBRARY)/SYSTEM.C $(LIBRARY)/MEM.C #$(LIBRARY)/HDDIO.C $(LIBRARY)/COLOR.C $(LIBRARY)/GRAPHICS.C $(LIBRARY)/MOUSE.C $(LIBRARY)/TIME.C $(LIBRARY)/CMOS.C $(LIBRARY)/FONT.C $(LIBRARY)/FS.C  $(LIBRARY)/TIMER.C $(LIBRARY)/MMU.C $(LIBRARY)/SHEET.C $(LIBRARY)/IO.C $(LIBRARY)/IDE.C $(LIBRARY)/ZFS.C $(LIBRARY)/CONSOLE.C $(LIBRARY)/SOUND.C $(LIBRARY)/SERIAL.C  $(LIBRARY)/PCI.C $(LIBRARY)/PNP.C $(LIBRARY)/DEV.C $(LIBRARY)/PCNET32.C $(LIBRARY)/ImageBmp.C $(LIBRARY)/FAT.C $(LIBRARY)/DRAW3D.C  $(LIBRARY)/DRAW2D.C $(LIBRARY)/COMPONENT.C
INC =$(INCLUDE)/KEYBOARD.H $(INCLUDE)/STRING.H $(INCLUDE)/STDIO.H $(INCLUDE)/CONIO.H $(INCLUDE)/SYSTEM.H $(INCLUDE)/MEM.H #$(INCLUDE)/HDDIO.H $(INCLUDE)/COLOR.H $(INCLUDE)/GRAPHICS.H $(INCLUDE)/MOUSE.H $(INCLUDE)/TIME.H $(INCLUDE)/CMOS.H $(INCLUDE)/FONT.H $(INCLUDE)/FS.H 	$(INCLUDE)/TIMER.H $(INCLUDE)/MMU.H $(INCLUDE)/SHEET.H $(INCLUDE)/IO.H $(INCLUDE)/IDE.H $(INCLUDE)/ZFS.H $(INCLUDE)/CONSOLE.H $(INCLUDE)/SOUND.H $(INCLUDE)/SERIAL.H  $(INCLUDE)/PCI.H $(INCLUDE)/PNP.H $(INCLUDE)/DEV.H $(INCLUDE)/PCNET32.H $(INCLUDE)/ImageBmp.H $(INCLUDE)/FAT.H $(INCLUDE)/DRAW3D.H  $(INCLUDE)/DRAW2D.H $(INCLUDE)/COMPONENT.H
CFLAGS   = -c -Wall -I ${INCLUDE} -nostartfiles -ffreestanding -nostdlib -fno-builtin
CPPFLAGS = -c -Wall -nostartfiles -fno-exceptions -I $(INCLUDE) $(LIBRARY) 
LDFLAGS= -Ttext 0x10000 -e $(ENTERY)
######################compile Role######################

all:bootsect start object GCCObj link image

clean:
	del *.o
	del *.bin 
	del *.img

bootsect: bootsect.asm
	${ASM} -fbin  -I ${INCLUDE}/ bootsect.asm -o bootsect.bin 
		
start: start.asm 
	${ASM} -faout -I ${INCLUDE}/ start.asm -o start.o  

link:
	$(LD)  ${LDFLAGS} -o kernel.o start.o ${OBJ} $(GCCOBJ)
	  
 image:	
	$(O2B) -R .note -R .comment -S -O binary kernel.o kernel.bin
	del *.img
	makeboot ParsOS.img bootsect.bin kernel.bin

#########################compile Gcc Lib####################
 GCCObj: ${GCCOBJ}
#########################compile OS Lib####################
 object: ${OBJ} 	
 	
 %.o:%.c	
	$(CC) ${CFLAGS} $<
###Start Compile Gcc Lib##
 %.o: $(LIBRARY)/GCCLibrary/%.C
	$(CC) ${CFLAGS} $<	
	
 CKernel.o: CKernel.cpp
	$(GXX) ${CPPFLAGS} CKernel.cpp
	
COMPONENT1.o: Library/COMPONENT1.CPP
	$(GXX) ${CPPFLAGS} Library/COMPONENT1.CPP
	
 libc.o: libc.c	$(MAKEDEP) $(LIB) $(INC)
	$(CC) ${CFLAGS} libc.c	-ffreestanding -I ${LIBRARY} 	


######################Dependency######################

