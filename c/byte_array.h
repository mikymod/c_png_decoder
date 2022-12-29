#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    size_t max; /* Number of chars allocated for */
    size_t len; /* Number of chars in use */
    uint8_t *data;
} bytearray;

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

int bytearray_append(bytearray *barray, const void *from, const size_t size)
{
    if (barray->len + size > barray->max)
    {
        const size_t len = barray->len + size;
        size_t max;
        void *data;

        /* Example policy: */
        if (len < 8)
            max = 8; /* At least 8 chars, */
        else if (len < 4194304)
            max = (3 * len) / 2; /* grow by 50% up to 4,194,304 bytes, */
        else
            max = (len | 2097151) + 2097153 - 24; /* then pad to next multiple of 2,097,152 sans 24 bytes. */

        data = realloc(barray->data, max);
        if (!data)
        {
            /* Not enough memory available. Old data is still valid. */
            printf("Not enough memory available");
            return -1;
        }

        barray->max = max;
        barray->data = (uint8_t *)data;
    }

    /* Copy appended data; we know there is room now. */
    memmove(barray->data + barray->len, from, size);
    barray->len += size;

    return 0;
}