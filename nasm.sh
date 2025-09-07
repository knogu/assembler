cat nasm_header_64 input_64.s > tmp && nasm tmp -o nasm_flat.bin
hexdump -C nasm_flat.bin
