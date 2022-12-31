#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "byte_array.h"

void bytearray_init(bytearray *barray)
{
    barray->max = 0;
    barray->len = 0;
    barray->data = NULL;
}

void bytearray_free(bytearray *barray)
{
    free(barray->data);
    barray->max = 0;
    barray->len = 0;
    barray->data = NULL;
}

int32_t bytearray_append(bytearray *barray, const void *from, const size_t size)
{
    if (barray->len + size > barray->max)
    {
        const size_t len = barray->len + size;
        size_t max;
        void *data;

        // Policy
        if (len < 8)
        {
            max = 8;
        }
        else if (len < 4194304)
        {
            // grow by 50% up to 4,194,304 bytes
            max = (3 * len) / 2;
        }
        else
        {
            // then pad to next multiple of 2,097,152 sans 24 bytes.
            max = (len | 2097151) + 2097153 - 24;
        }

        data = realloc(barray->data, max);
        if (!data)
        {
            fprintf(stderr, "Not enough memory available\n");
            return -1;
        }

        barray->max = max;
        barray->data = (uint8_t *)data;
    }

    memmove(barray->data + barray->len, from, size);
    barray->len += size;

    return 0;
}