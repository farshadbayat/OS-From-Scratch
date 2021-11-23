######################Variable##########################
ASM = nasm 
CC  = gcc
GXX = gxx
LD  = ld
DIR = src
O2B = objcopy
######################Entery Point######################
ENTERY  = start

######################Directory#########################
INCLUDE = Include
LIBRARY	= Library

######################C & C++ Files#####################
GCCLIB=CTYPE.C MATH.C STRING.C STDIO.C CONIO.C MEM.C TIME.C
OSLIB =COLOR.C FONT.C SYSTEM.C CONSOLE.C ZFS.C FAT.C FS.C  DRAW2D.C DRAW3D.C ImageBMP.C COMPONENT.C#  #COMPONENT.CPP
DEVLIB=KEYBOARD.C HDDIO.C GRAPHICS.C MOUSE.C CMOS.C TIMER.C SHEET.C MMU.C IO.C  IDE.C  SOUND.C SERIAL.C PCI.C PNP.C DEV.C PCNET32.C # 
SRC = libc.c idt.c gdt.c isrs.c irq.c task.c 

##Get Object File String
GCCOBJS=$(GCCLIB:%.C=%.o)
OSOBJS=$(OSLIB:%.C=%.o)
DEVOBJS=$(DEVLIB:%.C=%.o)
OBJ = $(SRC:%.c=%.o) CKernel.o COMPONENT1.o

LIB =
INC =
CFLAGS   = -c -Wall -I ${INCLUDE} -nostartfiles -ffreestanding -nostdlib -fno-builtin
CPPFLAGS = -c -Wall -nostartfiles -fno-exceptions -I $(INCLUDE) $(LIBRARY) 
LDFLAGS= -Ttext 0x10000 -e $(ENTERY)
######################compile Role######################

all:ASMObj  object GCCObj OSObject DEVObject link image

clean:
	del *.o
	del *.bin 
	del *.img
	del *.TMP

link:
	$(LD)  ${LDFLAGS} -o kernel.o start.o  $(GCCOBJS) $(OSOBJS) ${DEVOBJS} ${OBJ}
	  
 image:	
	$(O2B) -R .note -R .comment -S -O binary kernel.o kernel.bin
	del *.img
	makeboot ParsOS.img bootsect.bin kernel.bin
#########################compile ASM####################
ASMObj: bootsect.o start.o
#########################compile Gcc Lib####################
 GCCObj: ${GCCOBJS}
#########################compile OS Lib#####################
 OSObject: ${OSOBJS}
#########################compile DEV Lib####################
 DEVObject: ${DEVOBJS}
#########################compile CPU Lib####################
 object: ${OBJ} 	


##############Now Start Compile My Command################## 
###Start Compile ASM##
bootsect.o: bootsect.asm
	${ASM} -fbin  -I ${INCLUDE}/ bootsect.asm -o bootsect.bin 		
start.o: start.asm 
	${ASM} -faout -I ${INCLUDE}/ start.asm -o start.o 
###Start Compile Gcc Lib##
 %.o: $(LIBRARY)/GCCLibrary/%.C
	$(CC) ${CFLAGS} $<
###Start Compile  OS Lib##
%.o: $(LIBRARY)/OSLibrary/%.C
		$(CC) ${CFLAGS} $<	
###Start Compile DEV Lib##
%.o: $(LIBRARY)/DEVLibrary/%.C
		$(CC) ${CFLAGS} $<
###Start Compile CPU Lib##
 %.o:%.c	
	$(CC) ${CFLAGS} $<
	
 CKernel.o: CKernel.cpp
	$(GXX) ${CPPFLAGS} CKernel.cpp
	
COMPONENT1.o: Library/COMPONENT1.CPP
	$(GXX) ${CPPFLAGS} Library/COMPONENT1.CPP
	
 libc.o: libc.c	$(MAKEDEP) $(LIB) $(INC)
	$(CC) ${CFLAGS} libc.c	-ffreestanding -I ${LIBRARY} 	


######################Dependency######################

