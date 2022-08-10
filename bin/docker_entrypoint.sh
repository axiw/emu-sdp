cd /app
make install
gcc -O1 -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libasan -lrt *.c
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./fuzzer
cppcheck --enable=all  --check-config --suppress=missingIncludeSystem main.c
