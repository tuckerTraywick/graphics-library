# clear
# echo "----BUILDING----"
# CFLAGS=$(pkg-config --cflags freetype2 x11 xft)
# LIBRARIES=$(pkg-config --libs freetype2 x11 xft)
# gcc -std=gnu99 -Wall -Wextra -Wpedantic -g3 $LIBRARIES -Isource $CFLAGS source/* -o build/run && echo && echo "---- RUNNING ----" && ./build/run

set -e
clear
make clean

echo "---- BUILDING SOURCE ----"
make build/run

echo -e "\n---- BUILDING TESTS ----"
make build/test

echo -e "\n---- RUNNING TESTS ----"
build/test

echo -e "\n---- RUNNING SOURCE ----"
build/run
