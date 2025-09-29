clear
echo "----BUILDING----"
gcc -std=gnu99 -Wall -Wextra -Wpedantic -g3 -lX11 source/* -Isource -o build/run && echo && echo "---- RUNNING ----" && ./build/run
