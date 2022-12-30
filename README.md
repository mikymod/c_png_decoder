clang.exe -o png_decoder.exe .\c\linked_list.c .\c\png.c .\c\main.c .\c\byte_array.c -L.\zlib\ -I.\zlib\
 -lzlibwapi -L.\sdl\lib -I.\sdl\include -lSDL2