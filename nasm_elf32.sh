cat nasm_header input.s > tmp && nasm tmp -f elf32 -o nasm_flat.bin && rm tmp
