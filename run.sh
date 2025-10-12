clear
echo "----BUILDING----"
CFLAGS=$(pkg-config --cflags freetype2 x11)
LIBRARIES=$(pkg-config --libs freetype2 x11)
gcc -std=gnu99 -Wall -Wextra -Wpedantic -g3 $LIBRARIES -Isource $CFLAGS source/* -o build/run && echo && echo "---- RUNNING ----" && ./build/run
