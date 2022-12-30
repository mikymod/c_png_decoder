#include <stdint.h>

#include "byte_array.h"

/// @brief
/// @param fp
/// @param pixels
/// @param width
/// @param height
/// @param channels
void parse_png(FILE *fp, bytearray *pixels, uint32_t *width, uint32_t *height, uint32_t *channels);
