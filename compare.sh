cat nasm_header input.s > tmp && nasm tmp -o nasm_flat.bin
hexdump nasm_flat.bin > tmp2
hexdump output.bin > tmp3
diff -u tmp2 tmp3
rm tmp*

