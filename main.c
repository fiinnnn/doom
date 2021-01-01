#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "c_video.h"
#include "def.h"
#include "log.h"
#include "c_mem.h"
#include "w_wad.h"
#include "p_level.h"
#include "am_map.h"

int init() {
    log_init();
    log_set_level(LOGLEVEL_TRACE);
    LOG_INFO("Logging initialized");

    char* files[3];
    files[0] = "wads/doom.wad";
    files[1] = NULL;
    files[2] = NULL;

    w_init_files(files);

    //w_log_lumpinfo();

    p_load_level(1, 1);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        LOG_ERROR("SDL_Init error: %s", SDL_GetError());
        return 0;
    }

    c_video_init(1280, 720);

    return 1;
}

void main_loop() {
    am_init();

    int running = 1;
    SDL_Event event;
    while (running) {
        am_update();

        // clear buffer
        memset(framebuffers[0], 0, SCREENWIDTH * SCREENHEIGHT * sizeof(uint8_t));

        am_draw();

        // display
        c_video_display_frame();

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

    c_free_all();

    c_video_shutdown();

    return EXIT_SUCCESS;
}
