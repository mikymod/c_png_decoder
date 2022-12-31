#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../zlib/zlib.h"
#include "png.h"

#include "linked_list.h"

static const char *png_signature = "\x89PNG\r\n\x1a\n";

/// Represents a generic png chunk
typedef struct png_chunk
{
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
} PNGChunk;

/// Represents a IHDR png chunk
typedef struct ihdr_chunk
{
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression;
    uint8_t filter;
    uint8_t interlace;
} IHDRChunk;

typedef struct png_chunk_node
{
    struct list_node node;
    PNGChunk chunk;
} PNGChunkNode;

///
int32_t paeth_predictor(const int32_t a, const int32_t b, const int32_t c)
{
    int32_t p = a + b - c;
    int32_t pa = abs(p - a);
    int32_t pb = abs(p - b);
    int32_t pc = abs(p - c);
    if (pa <= pb && pa <= pc)
    {
        return a;
    }
    else if (pb <= pc)
    {
        return b;
    }
    else
    {
        return c;
    }
}

///
int recon_a(const uint8_t *pixels, const int r, const int c, const int bytes_per_pixel, const int stride)
{
    if (c >= bytes_per_pixel)
    {
        return pixels[r * stride + c - bytes_per_pixel];
    }
    return 0;
}

///
int32_t recon_b(const uint8_t *pixels, const int r, const int c, const int bytes_per_pixel, const int stride)
{
    if (r > 0)
    {
        return pixels[(r - 1) * stride + c];
    }
    return 0;
}

///
int32_t recon_c(const uint8_t *pixels, const int r, const int c, const int bytes_per_pixel, const int stride)
{
    if (r > 0 && c >= bytes_per_pixel)
    {
        return pixels[(r - 1) * stride + c - bytes_per_pixel];
    }
    return 0;
}

///
PNGChunkNode *png_chunk_node_new(PNGChunk chunk)
{
    PNGChunkNode *node = (PNGChunkNode *)malloc(sizeof(PNGChunkNode));
    if (!node)
    {
        return NULL;
    }

    node->chunk = chunk;
    return node;
}

///
IHDRChunk parse_ihdr(PNGChunkNode **chunk_list)
{
    PNGChunk chunk = (*chunk_list)->chunk;
    IHDRChunk ihdr;
    ihdr.width = chunk.data[0] << 24 | chunk.data[1] << 16 | chunk.data[2] << 8 | chunk.data[3];
    ihdr.height = chunk.data[7] | chunk.data[6] << 8 | chunk.data[5] << 16 | chunk.data[4] << 24;
    ihdr.bit_depth = chunk.data[8];
    ihdr.color_type = chunk.data[9];
    ihdr.compression = chunk.data[10];
    ihdr.filter = chunk.data[11];
    ihdr.interlace = chunk.data[12];

    if (ihdr.compression != 0)
    {
        fprintf(stderr, "Invalid compression method\n");
    }
    if (ihdr.filter != 0)
    {
        fprintf(stderr, "Invalid filter method\n");
    }
    if (ihdr.color_type != 6)
    {
        fprintf(stderr, "We only support truecolor with alpha\n");
    }
    if (ihdr.bit_depth != 8)
    {
        fprintf(stderr, "We only support a bit depth of 8\n");
    }
    if (ihdr.interlace != 0)
    {
        fprintf(stderr, "We only support no interlacing\n");
    }

    return ihdr;
}

///
void parse_idat(PNGChunkNode **chunk_list, const IHDRChunk ihdr, bytearray *pixels)
{
    // Store idat data
    PNGChunkNode *current = *chunk_list;

    bytearray array;
    bytearray_init(&array);

    do
    {
        if (memcmp(current->chunk.type, "IDAT", 4) == 0)
        {
            int res = bytearray_append(&array, current->chunk.data, current->chunk.length);
        }
        current = current->node.next;
    } while (current != NULL);

    // Decompress data
    uint32_t bytes_per_pixel = 4;
    uint32_t stride = ihdr.width * bytes_per_pixel;
    unsigned long uncompressed_size = ihdr.height * (1 + stride);
    unsigned long compressed_max_size = compressBound(uncompressed_size);
    uint8_t *idat_data = (uint8_t *)malloc(uncompressed_size);
    int32_t result = uncompress(idat_data, &uncompressed_size, array.data, compressed_max_size);
    if (result != Z_OK)
    {
        fprintf(stderr, "Unable to uncompress: error %d\n", result);
        bytearray_free(&array);
        free(idat_data);
    }

    int i = 0;
    int32_t pixel = 0;
    for (size_t r = 0; r < ihdr.height; r++)
    {
        int32_t filter_type = idat_data[i];
        i++;
        for (size_t c = 0; c < stride; c++)
        {
            int32_t filt_x = idat_data[i];
            i++;
            switch (filter_type)
            {
            case 0:
                pixel = filt_x;
                break;
            case 1:
                pixel = filt_x + recon_a(pixels->data, r, c, bytes_per_pixel, stride);
                break;
            case 2:
                pixel = filt_x + recon_b(pixels->data, r, c, bytes_per_pixel, stride);
                break;
            case 3:
                pixel = filt_x + (recon_a(pixels->data, r, c, bytes_per_pixel, stride) + recon_b(pixels->data, r, c, bytes_per_pixel, stride)) / 2;
                break;
            case 4:
                pixel = filt_x + paeth_predictor(
                                     recon_a(pixels->data, r, c, bytes_per_pixel, stride),
                                     recon_b(pixels->data, r, c, bytes_per_pixel, stride),
                                     recon_c(pixels->data, r, c, bytes_per_pixel, stride));
                break;
            default:
                fprintf(stderr, "unknown filter type %d\n", filter_type);
            }

            bytearray_append(pixels, &pixel, 1);
        }
    }
}

int32_t png_decode(const char *filename, bytearray *pixels, uint32_t *width, uint32_t *height, uint32_t *channels)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Unable to open %s\n", filename);
        return -1;
    }

    char signature[8];
    size_t size = fread(&signature, 8, 1, fp);
    if (memcmp(signature, png_signature, 8) != 0)
    {
        fprintf(stderr, "Invalid PNG signature in %s\n", filename);
        return -1;
    }

    PNGChunkNode *chunk_list = NULL;

    PNGChunk chunk;
    while (memcmp(chunk.type, "IEND", 4) != 0)
    {
        uint8_t chunk_length[4];
        fread(&chunk_length, 1, 4, fp);
        chunk.length = (chunk_length[0] << 24) | (chunk_length[1] << 16) | (chunk_length[2] << 8) | chunk_length[3];

        fread(&chunk.type, 4, 1, fp);

        chunk.data = (uint8_t *)malloc(chunk.length);
        memset(chunk.data, 0, chunk.length);
        fread(chunk.data, 1, chunk.length, fp);

        uint8_t chunk_crc[4];
        fread(&chunk_crc, 1, 4, fp);
        chunk.crc = (chunk_crc[0] << 24) | (chunk_crc[1] << 16) | (chunk_crc[2] << 8) | chunk_crc[3];

        list_append(PPCAST(&chunk_list), PCAST(png_chunk_node_new(chunk)));
    }

    IHDRChunk ihdr = parse_ihdr(&chunk_list);
    *width = ihdr.width;
    *height = ihdr.height;
    *channels = 4; // FIXME: hardcoded
    parse_idat(&chunk_list, ihdr, pixels);
}
