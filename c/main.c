#include <stdio.h>

#include "png.h"
#include "zlib/zlib.h"

static const char *png_signature = "\x89PNG\r\n\x1a\n";

int main(int argc, char **argv)
{
    const char *filename = "basn6a08.png";
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Unable to open %s\n", filename);
        return -1;
    }

    char signature[10];
    size_t size = fread(&signature, 1, strlen(png_signature), fp);
    if (strcmp(signature, png_signature) != 0)
    {
        fprintf(stderr, "Invalid PNG signature in %s\n", filename);
        return -1;
    }

    PNGChunkNode *chunk_list = NULL;
    parse_png(fp, &chunk_list);
    PNG_CHUNKS_DEBUG_PRINT(PPCAST(&chunk_list));

    IHDRChunk ihdr = parse_ihdr(&chunk_list);
    bytearray pixels;
    bytearray_init(&pixels);

    parse_idat(&chunk_list, ihdr, &pixels);

    fclose(fp);
    bytearray_free(&pixels);
    printf("success");

    return 0;
}