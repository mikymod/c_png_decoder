#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    size_t max; /// Number of byte allocated for
    size_t len; /// Number of byte in use
    uint8_t *data;
} bytearray;

/// @brief
/// @param barray
void bytearray_init(bytearray *barray);

/// @brief
/// @param barray
void bytearray_free(bytearray *barray);

/// @brief
/// @param barray
/// @param from
/// @param size
int32_t bytearray_append(bytearray *barray, const void *from, const size_t size);