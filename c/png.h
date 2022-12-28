#include <stdint.h>
#include "linked_list.h"

#define PNG_CHUNKS_DEBUG_PRINT(head)                                                                            \
    {                                                                                                           \
        PNGChunkNode *node = *head;                                                                             \
        while (node)                                                                                            \
        {                                                                                                       \
            printf("length: %d,\ttype: %s,\tcrc: %x\n", node->chunk.length, node->chunk.type, node->chunk.crc); \
            node = (PNGChunkNode *)node->node.next;                                                             \
        }                                                                                                       \
        printf("---------\n");                                                                                  \
    }

/// Structures
typedef struct png_chunk
{
    uint32_t length;
    uint8_t type[5];
    uint8_t *data;
    uint32_t crc;
} PNGChunk;

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

/// Forward declarations

PNGChunkNode *png_chunk_node_new(PNGChunk chunk);
void parse_png(FILE *fp, PNGChunkNode **chunk_list);
void parse_ihdr(PNGChunkNode **chunk_list);

PNGChunkNode *png_chunk_node_new(PNGChunk chunk)
{
    PNGChunkNode *node = malloc(sizeof(PNGChunkNode));
    if (!node)
    {
        return NULL;
    }

    node->chunk = chunk;
    return node;
}

void parse_png(FILE *fp, PNGChunkNode **chunk_list)
{
    PNGChunk chunk;
    while (memcmp(chunk.type, "IEND", 4) != 0)
    {
        uint8_t chunk_length[4];
        fread(&chunk_length, 1, 4, fp);
        chunk.length = (chunk_length[0] << 24) | (chunk_length[1] << 16) | (chunk_length[2] << 8) | chunk_length[3];

        fread(&chunk.type, 4, 1, fp);
        chunk.type[4] = '\0';

        chunk.data = (uint8_t *)malloc(chunk.length);
        memset(chunk.data, 0, chunk.length);
        fread(chunk.data, 1, chunk.length, fp);

        // uint32_t crc = crc32(0, chunk_data, chunk_length);
        // uint32_t width = chunk_data[0] << 24 | chunk_data[1] << 16 | chunk_data[2] << 8 | chunk_data[3];
        // printf("width: %d\n", width);
        // uint32_t height = chunk_data[7] | chunk_data[6] << 8 | chunk_data[5] << 16 | chunk_data[4] << 24;
        // printf("height: %d\n", height);

        uint8_t chunk_crc[4];
        fread(&chunk_crc, 1, 4, fp);
        chunk.crc = (chunk_crc[0] << 24) | (chunk_crc[1] << 16) | (chunk_crc[2] << 8) | chunk_crc[3];

        list_append(PPCAST(&chunk_list), PCAST(png_chunk_node_new(chunk)));
    }
}

void parse_ihdr(PNGChunkNode **chunk_list)
{
    PNGChunk chunk = (*chunk_list)->chunk;
    IHDRChunk ihdr;
    ihdr.width = chunk.data[0] << 24 | chunk.data[1] << 16 | chunk.data[2] << 8 | chunk.data[3];
    printf("width: %d\n", ihdr.width);
    ihdr.height = chunk.data[7] | chunk.data[6] << 8 | chunk.data[5] << 16 | chunk.data[4] << 24;
    printf("height: %d\n", ihdr.height);
}