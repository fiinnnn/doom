/*
 * automap
 */

#ifndef AM_MAP_H
#define AM_MAP_H

#include <stdint.h>

/*
 * initializes automap
 */
void am_init(uint32_t* buf);

/*
 * event handler
 */
union SDL_Event; // TODO: stop using SDL_Event
void am_handle_event(union SDL_Event* event);

/*
 * update
 */
void am_update();

/*
 * draws the automap for the currently active level
 */
void am_draw();

#endif // AM_MAP_H
