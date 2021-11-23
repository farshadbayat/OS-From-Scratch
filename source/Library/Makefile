######################Variable######################
ASM = nasm 
CC  = gcc -I..\include
LD  = ld

INCLUDE = ..\include
LIBRARY	= library
ENTERY  = start
LIB =  MATH.C CTYPE.C STRING.C  STDIO.C  CONIO.C  SYSTEM.C  MEM.C  HDDIO.C	 GRAPHICS.C  MOUSE.C  TIME.C  CMOS.C  FONT.C  FAT.C
INC =$(INCLUDE)/CTYPE.H $(INCLUDE)/MATH.H $(INCLUDE)/STRING.H $(INCLUDE)/STDIO.H $(INCLUDE)/CONIO.H $(INCLUDE)/SYSTEM.H $(INCLUDE)/MEM.H $(INCLUDE)/HDDIO.H	$(INCLUDE)/GRAPHICS.H $(INCLUDE)/MOUSE.H $(INCLUDE)/TIME.H $(INCLUDE)/CMOS.H $(INCLUDE)/FONT.H $(INCLUDE)/FAT.H
OBJ = $(LIB:%.C=%.o) 
CFLAGS = -c -Wall -I ${INCLUDE}
LDFLAGS= -Ttext 0x10000 -e $(ENTERY)
######################compile Role######################

all : ${OBJ}

clean:
	del *.o
	del *.a
	del *.img 
		
%.o: %.c	
	gcc ${CFLAGS} $< -ffreestanding -I ${INCLUDE}

#libs.a : $(OBJ) $(MAKEDEP)
#	ar rcs $@ $(OBJ)


######################Dependency######################

