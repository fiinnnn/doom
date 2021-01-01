/*
 * video implementation
 */

#ifndef I_VIDEO_H
#define I_VIDEO_H

#include <stdint.h>

int i_video_init(int width, int height);
void i_video_shutdown();

void i_video_draw_frame(uint32_t* buf);

#endif // I_VIDEO_H
