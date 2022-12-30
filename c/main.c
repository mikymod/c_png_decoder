#define SDL_MAIN_HANDLED
#include <SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>

#include "png.h"

static const char *png_signature = "\x89PNG\r\n\x1a\n";

int main(int argc, char **argv)
{
    const char *filename = "basn6a08.png";
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

    PNGChunkNode *chunk_list = NULL;
    parse_png(fp, &chunk_list);
    PNG_CHUNKS_DEBUG_PRINT(PPCAST(&chunk_list));

    IHDRChunk ihdr = parse_ihdr(&chunk_list);

    bytearray pixels;
    bytearray_init(&pixels);
    parse_idat(&chunk_list, ihdr, &pixels);

    int ret = -1;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        goto quit;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        goto quit;
    }

    int width = ihdr.width;
    int height = ihdr.height;
    int channels = 4;
    if (!pixels.data)
    {
        SDL_Log("Unable to open image");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        goto quit;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture)
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        bytearray_free(&pixels);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        goto quit;
    }

    SDL_UpdateTexture(texture, NULL, pixels.data, width * channels);
    bytearray_free(&pixels);

    ret = 0;

    int running = 1;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Rect target_rect = {100, 100, 256, 256};
        SDL_RenderCopy(renderer, texture, NULL, &target_rect);
        SDL_RenderPresent(renderer);
    }

quit:
    fclose(fp);
    bytearray_free(&pixels);
    SDL_Quit();
    return ret;
}