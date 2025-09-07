cat nasm_header_64 input_64.s > tmp && nasm tmp -o nasm_flat.bin
hexdump -C nasm_flat.bin > tmp_nasm
hexdump -C output.bin > tmp_my_asm
diff -u tmp_nasm tmp_my_asm
rm tmp*
