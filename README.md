# PNG decoder

To compile:
```
clang.exe -o png_decoder.exe .\c\main.c .\c\linked_list.c .\c\byte_array.c .\c\png.c -L.\zlib\ -I.\zlib\ -lzlibwapi -L.\sdl\lib -I.\sdl\include -lSDL2 
```

After that, run:
```
.\png_decoder.exe <filename>
```