/*
 * display an RGBA buffer
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct display {
    int width;
    int height;

    struct SDL_Window* window;
    struct SDL_Renderer* renderer;
    struct SDL_Texture* texture;
};

int display_init(struct display* d);
void display_destroy(struct display* d);

void display_draw_frame(struct display* d, uint32_t* buf);

#endif // DISPLAY_H
