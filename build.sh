
 # This file is part of gardOS 1.0 generic.
 #
 # Copyright (c) 2025 Gabriel Sîrbu
 #
 # This software is distributed under the terms of the GNU General Public License.
 # See LICENSE for more details.

nasm -f elf32 kernel.asm -o kasm.o
gcc -m32 -c kernel.c -o kc.o
ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o 
qemu-system-x86_64 -kernel kernel
