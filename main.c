#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "def.h"
#include "log.h"
#include "w_wad.h"
#include "p_level.h"
#include "am_map.h"

struct display *d;

int init() {
    log_init();
    log_set_level(LOGLEVEL_TRACE);
    LOG_INFO("Logging initialized");

    char* files[3];
    files[0] = "wads/doom.wad";
    files[1] = NULL;
    files[2] = NULL;

    w_init_files(files);

    /*for (int i = 0; i < numlumps; i++) {
        lumpinfo_t lump = lumpinfo[i];
        LOG_DEBUG("NAME: %8.8s POS: %8d SIZE: %d",
                lump.name, lump.position, lump.size);
    }*/

    p_load_level(1, 1);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        LOG_ERROR("SDL_Init error: %s", SDL_GetError());
        return 0;
    }

    d = malloc(sizeof(struct display));
    d->width = 740;
    d->height = 500;

    if(!display_init(d)) {
        LOG_ERROR("Failed to initialize display");
        return 0;
    }

    return 1;
}

void main_loop() {
    uint32_t* buf = calloc(SCREENWIDTH * SCREENHEIGHT, sizeof(uint32_t));

    am_init(buf);

    int running = 1;
    SDL_Event event;
    while (running) {
        am_update();

        // clear buffer
        memset(buf, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(uint32_t));

        am_draw();

        // display
        display_draw_frame(d, buf);

        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_QUIT) ||
                (event.type == SDL_KEYDOWN &&
                 event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = 0;
                break;
            }

            am_handle_event(&event);
        }

        usleep(30000); // TODO: proper fixed timestep
    }
}

int main()
{
    if (!init())
        return EXIT_FAILURE;

    main_loop();

    display_destroy(d);

    return EXIT_SUCCESS;
}
