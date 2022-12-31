#include <stdint.h>

typedef struct
{
    size_t max; /// Number of byte allocated for
    size_t len; /// Number of byte in use
    uint8_t *data;
} bytearray;

/// Initialize bytearray
void bytearray_init(bytearray *barray);

/// Disposes bytearray
void bytearray_free(bytearray *barray);

/// Appends data to bytearray
int32_t bytearray_append(bytearray *barray, const void *from, const size_t size);