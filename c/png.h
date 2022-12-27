#include <stdint.h>

typedef struct png_chunk
{
    uint32_t length;
    uint8_t type[5];
    uint8_t *data;
    uint32_t crc;
} PngChunk;

typedef struct ihdr_header
{
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression;
    uint8_t filter;
    uint8_t interlace;
} IHDRHeader;
