/* Copyright 2026 JN Martineau F4IAE
 * https://github.com/jnmartineau
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

/* Linker script for freqmanager overlay app */

MEMORY
{
  /* 4 KiB of overlay RAM for code and data */
  RAM (rwx) : ORIGIN = 0x20000000 + 4096, LENGTH = 4096
}

ENTRY(app_main)

SECTIONS
{
    .text :
    {
        *(.text.entry)
        *(.text*)
        *(.rodata*)
        *(.glue_7*)
        *(.glue_7t*)
    } > RAM

    .data : { *(.data*) } > RAM
    .bss : { *(.bss*) *(COMMON) } > RAM
}

/* Never touch the first 4096 bytes of RAM (0x20000000-0x20000FFF). */
/* ASSERT(ORIGIN(RAM) >= 0x20001000, "overlay app at wrong address"); */
