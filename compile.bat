echo off

nasm  -f bin  -I source/include/ -o  bootsect.bin source/bootsect.asm
nasm  -f aout -I source/include/ -o  start.o      source/start.asm

gcc -Wall -c -o libc.o	source/libc.c	-I./source/include -ffreestanding -I./source/library
gcc -Wall -c -o main.o	source/main.c	-I./source/include	-ffreestanding 
gcc -Wall -c -o idt.o	source/idt.c	-I./source/include	-ffreestanding 
gcc -Wall -c -o gdt.o	source/gdt.c	-I./source/include
gcc -Wall -c -o isrs.o	source/isrs.c	-I./source/include
gcc -Wall -c -o irq.o	source/irq.c	-I./source/include
gcc -Wall -c -o kb.o	source/kb.c	-I./source/include
gcc -Wall -c -o task.o	source/task.c	-I./source/include
gcc -Wall -c -o timer.o	source/timer.c	-I./source/include
gcc -Wall -c -o mouse.o	source/mouse.c	-I./source/include

ld -e start -Ttext 0x10000 -o kernel.o start.o main.o libc.o gdt.o idt.o isrs.o irq.o kb.o timer.o task.o 

objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

del *.o

del *.img

makeboot a.img bootsect.bin kernel.bin

del *.bin
