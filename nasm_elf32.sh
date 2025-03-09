cat nasm_header input.s > tmp && nasm tmp -f elf32 -o nasm_elf32.bin && rm tmp
x86_64-elf-objdump -d nasm_elf32.bin
