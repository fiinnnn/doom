#include "c_video.h"

#include "i_video.h"
#include "c_mem.h"
#include "def.h"
#include "w_wad.h"
#include "log.h"

uint8_t* framebuffers[5];
uint32_t colors[256];
uint32_t* buf;

void c_video_init(int width, int height) 
{
    // initialize video implementation
    if(!i_video_init(1280, 720)) {
        LOG_ERROR("Failed to initialize display");
        exit(1);
    }

    // allocate framebuffers
    uint8_t* base = c_malloc(SCREENWIDTH * SCREENHEIGHT * 4, LT_STATIC);
    for (int i = 0; i < 4; i++) {
        framebuffers[i] = base + i * SCREENWIDTH * SCREENHEIGHT;
    }

    buf = c_malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(uint32_t), LT_STATIC);

    // set standard palette
    c_video_set_palette(w_load_lump(w_get_lump_num("PLAYPAL"))); // TODO: cache this
}

void c_video_shutdown()
{
    i_video_shutdown();
}

void c_video_set_palette(uint8_t* palette) 
{
    for (int i = 0; i < 256; i++) {
        colors[i]  = (*palette++)<<24;
        colors[i] += (*palette++)<<16;
        colors[i] += (*palette++)<<8;
        colors[i] += 0xFF;
    }
}

void c_video_display_frame()
{
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) {
        buf[i] = colors[framebuffers[0][i]];
    }

    i_video_draw_frame(buf);
}