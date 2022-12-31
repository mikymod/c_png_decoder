#include <stdint.h>

#include "byte_array.h"

/// Decodes a png file
/// @param filename, the name of the file
/// @param pixels, the decoded pixels raw data
/// @param width, the width of the image
/// @param height, the height of the image
/// @param channels, the channel of the image
/// @return 0 if ok
int32_t png_decode(const char *filename, bytearray *pixels, uint32_t *width, uint32_t *height, uint32_t *channels);
