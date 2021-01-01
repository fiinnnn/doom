/*
 * video system (non implementation specific part)
 */

#include <stdint.h>

extern uint8_t* framebuffers[5];

/* 
 * initialize video system
 */
void c_video_init(int width, int height);

/*
 * shutdown video system
 */
void c_video_shutdown();

/*
 * set active palette
 */
void c_video_set_palette(uint8_t* palette);

/*
 * draw framebuffer 0
 */
void c_video_display_frame();