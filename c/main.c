#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "png.h"
#include "zlib/zlib.h"

static const char *png_signature = "\x89PNG\r\n\x1a\n";

int main(int argc, char **argv)
{
    const char *filename = "elden_ring.png";
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

    for (;;)
    {
        PngChunk chunk;

        uint8_t chunk_length[4];
        fread(&chunk_length, 1, 4, fp);
        chunk.length = (chunk_length[0] << 24) | (chunk_length[1] << 16) | (chunk_length[2] << 8) | chunk_length[3];
        printf("chunk length: %d\n", chunk.length);

        fread(&chunk.type, 4, 1, fp);
        chunk.type[4] = '\0';
        printf("chunk type: %s\n", chunk.type);

        uint8_t *chunk_data = (uint8_t *)malloc(chunk.length);
        memset(chunk_data, 0, chunk.length);
        fread(chunk_data, 1, chunk.length, fp);

        // uint32_t crc = crc32(0, chunk_data, chunk_length);

        // uint32_t width = chunk_data[0] << 24 | chunk_data[1] << 16 | chunk_data[2] << 8 | chunk_data[3];
        // printf("width: %d\n", width);
        // uint32_t height = chunk_data[7] | chunk_data[6] << 8 | chunk_data[5] << 16 | chunk_data[4] << 24;
        // printf("height: %d\n", height);

        uint8_t chunk_crc[4];
        fread(&chunk_crc, 1, 4, fp);
        chunk.crc = (chunk_crc[0] << 24) | (chunk_crc[1] << 16) | (chunk_crc[2] << 8) | chunk_crc[3];
        printf("chunk crc: %x\n", chunk.crc);

        free(chunk_data);

        if (memcmp(chunk.type, "IEND", 4) == 0)
            break;
    }

    fclose(fp);
    printf("success");
    return 0;
}