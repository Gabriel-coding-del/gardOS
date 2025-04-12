
 ; This file is part of gardOS 1.0 generic.
 ;
 ; Copyright (c) 2025 Gabriel Sîrbu
 ;
 ; This software is distributed under the terms of the GNU General Public License.
 ; See LICENSE for more details.

bits 32

section .text
    align 4
    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002 + 0x00)

global start
extern kmain

start:
    cli
    mov esp, stack_space
    call kmain
    hlt

section .bss
align 4
stack_space: resb 8192
;;;;;
section .note.GNU-stack noalloc noexec nowrite progbits
