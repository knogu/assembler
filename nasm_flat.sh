cat nasm_header input.s > tmp && nasm tmp -o nasm_flat.bin && rm tmp
hexdump nasm_flat.bin
