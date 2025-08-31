cat nasm_header input.s > tmp && nasm tmp -o nasm_flat.bin
hexdump nasm_flat.bin > tmp_nasm
hexdump output.bin > tmp_my_asm
diff -u tmp_nasm tmp_my_asm
rm tmp*

