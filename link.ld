/*
 * This file is part of gardOS 1.0 generic.
 *
 * Copyright (c) 2025 Gabriel Sîrbu
 *
 * This software is distributed under the terms of the GNU General Public License.
 * See LICENSE for more details.
 */
OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS
{
  . = 0x100000;

  .text ALIGN(4K) : {
    *(.multiboot)
    *(.text)
    *(.rodata)
  } :text

  .data ALIGN(4K) : {
    *(.data)
  } :data

  .bss ALIGN(4K) : {
    *(.bss)
    *(COMMON)
  } :data

  /DISCARD/ : {
    *(.note.GNU-stack)
  }
}

PHDRS
{
  text PT_LOAD FLAGS(5); /* Read + Execute */
  data PT_LOAD FLAGS(6); /* Read + Write */
}
