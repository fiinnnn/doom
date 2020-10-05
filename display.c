#include "display.h"

#include <SDL2/SDL.h>

#include "def.h"
#include "log.h"

int display_init(struct display* d)
{
    SDL_WindowFlags flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    LOG_DEBUG("Creating window (%dx%d)", d->width, d->height);
    d->window = SDL_CreateWindow("DOOM",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            d->width,
            d->height,
            flags);

    if (!d->window) {
        LOG_ERROR("Failed to create window: %s", SDL_GetError());
        return 0;
    }

    LOG_DEBUG("Creating renderer");
    d->renderer = SDL_CreateRenderer(d->window, -1, SDL_RENDERER_ACCELERATED);

    if (!d->renderer) {
        LOG_ERROR("Failed to create renderer: %s", SDL_GetError());
        return 0;
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(d->renderer, &info);
    LOG_INFO("Renderer: %s", info.name);

    SDL_RenderSetLogicalSize(d->renderer, d->width, d->height);

    LOG_DEBUG("Creating rendertarget (%dx%d)", SCREENWIDTH, SCREENHEIGHT);
    d->texture = SDL_CreateTexture(d->renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREENWIDTH, SCREENHEIGHT);

    if (!d->texture) {
        LOG_ERROR("Failed to create texture: %s", SDL_GetError());
        return 0;
    }

    LOG_INFO("Display initialized");

    return 1;
}

void display_destroy(struct display* d)
{
    if (!d)
        return;

    LOG_INFO("Destroying display");

    if (d->texture) {
        LOG_DEBUG("Destroying texture");
        SDL_DestroyTexture(d->texture);
    }

    if (d->renderer) {
        LOG_DEBUG("Destroying renderer");
        SDL_DestroyRenderer(d->renderer);
    }

    if (d->window) {
        LOG_DEBUG("Destroying window");
        SDL_DestroyWindow(d->window);
    }

    SDL_Quit();

    free(d);
}

void display_draw_frame(struct display* d, uint32_t* buf)
{
    SDL_RenderClear(d->renderer);

    SDL_UpdateTexture(d->texture, NULL, (void*)buf,
            sizeof(uint32_t) * SCREENWIDTH);

    SDL_RenderCopy(d->renderer, d->texture, NULL, NULL);

    SDL_RenderPresent(d->renderer);
}
