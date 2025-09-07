set -xue

cmake --build build
./build/asm input_64.s
./compare.sh
