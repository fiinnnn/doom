#include "i_video.h"

#include <SDL2/SDL.h>

#include "def.h"
#include "log.h"

struct window {
    int width;
    int height;

    struct SDL_Window* sdl_window;
    struct SDL_Renderer* renderer;
    struct SDL_Texture* texture;
};

static struct window window;

int i_video_init(int width, int height)
{
    window.width = width;
    window.height = height;

    SDL_WindowFlags flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    LOG_DEBUG("Creating window (%dx%d)", width, height);
    window.sdl_window = SDL_CreateWindow("DOOM", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            flags);

    if (!window.sdl_window) {
        LOG_ERROR("Failed to create window: %s", SDL_GetError());
        return 0;
    }

    LOG_DEBUG("Creating renderer");
    window.renderer = SDL_CreateRenderer(window.sdl_window, -1, SDL_RENDERER_ACCELERATED);

    if (!window.renderer) {
        LOG_ERROR("Failed to create renderer: %s", SDL_GetError());
        return 0;
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(window.renderer, &info);
    LOG_INFO("Renderer: %s", info.name);

    SDL_RenderSetLogicalSize(window.renderer, width, height);

    LOG_DEBUG("Creating rendertarget (%dx%d)", SCREENWIDTH, SCREENHEIGHT);
    window.texture = SDL_CreateTexture(window.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREENWIDTH, SCREENHEIGHT);

    if (!window.texture) {
        LOG_ERROR("Failed to create texture: %s", SDL_GetError());
        return 0;
    }

    LOG_INFO("Display initialized");

    return 1;
}

void i_video_shutdown()
{
    LOG_INFO("Destroying display");

    if (window.texture) {
        LOG_DEBUG("Destroying texture");
        SDL_DestroyTexture(window.texture);
    }

    if (window.renderer) {
        LOG_DEBUG("Destroying renderer");
        SDL_DestroyRenderer(window.renderer);
    }

    if (window.sdl_window) {
        LOG_DEBUG("Destroying window");
        SDL_DestroyWindow(window.sdl_window);
    }

    SDL_Quit();
}

void i_video_draw_frame(uint32_t* buf)
{
    SDL_RenderClear(window.renderer);

    SDL_UpdateTexture(window.texture, NULL, (void*)buf,
            sizeof(uint32_t) * SCREENWIDTH);

    SDL_RenderCopy(window.renderer, window.texture, NULL, NULL);

    SDL_RenderPresent(window.renderer);
}
