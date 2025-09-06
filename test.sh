set -xue

cmake --build build
./build/asm input.s
./compare.sh

echo "OK"
