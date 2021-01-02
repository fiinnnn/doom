#include "i_video.h"

#include <SDL2/SDL.h>

#include "def.h"
#include "log.h"

struct context {
    int window_width;
    int window_height;

    struct SDL_Window* sdl_window;
    struct SDL_Renderer* renderer;
    struct SDL_Texture* texture;
};

static struct context context;

int i_video_init(int width, int height)
{
    context.window_width = width;
    context.window_height = height;

    SDL_WindowFlags flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    LOG_DEBUG("Creating window (%dx%d)", width, height);
    context.sdl_window = SDL_CreateWindow("DOOM", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            flags);

    if (!context.sdl_window) {
        LOG_ERROR("Failed to create window: %s", SDL_GetError());
        return 0;
    }

    LOG_DEBUG("Creating renderer");
    context.renderer = SDL_CreateRenderer(context.sdl_window, -1, SDL_RENDERER_ACCELERATED);

    if (!context.renderer) {
        LOG_ERROR("Failed to create renderer: %s", SDL_GetError());
        return 0;
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(context.renderer, &info);
    LOG_INFO("Renderer: %s", info.name);

    SDL_RenderSetLogicalSize(context.renderer, width, height);

    LOG_DEBUG("Creating rendertarget (%dx%d)", SCREENWIDTH, SCREENHEIGHT);
    context.texture = SDL_CreateTexture(context.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREENWIDTH, SCREENHEIGHT);

    if (!context.texture) {
        LOG_ERROR("Failed to create texture: %s", SDL_GetError());
        return 0;
    }

    LOG_INFO("Display initialized");

    return 1;
}

void i_video_shutdown()
{
    LOG_INFO("Destroying display");

    if (context.texture) {
        LOG_DEBUG("Destroying texture");
        SDL_DestroyTexture(context.texture);
    }

    if (context.renderer) {
        LOG_DEBUG("Destroying renderer");
        SDL_DestroyRenderer(context.renderer);
    }

    if (context.sdl_window) {
        LOG_DEBUG("Destroying window");
        SDL_DestroyWindow(context.sdl_window);
    }

    SDL_Quit();
}

void i_video_draw_frame(uint32_t* buf)
{
    SDL_RenderClear(context.renderer);

    SDL_UpdateTexture(context.texture, NULL, (void*)buf,
            sizeof(uint32_t) * SCREENWIDTH);

    SDL_RenderCopy(context.renderer, context.texture, NULL, NULL);

    SDL_RenderPresent(context.renderer);
}
