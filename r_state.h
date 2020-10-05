/*
 * renderer internal state variables
 */

#ifndef R_STATE_H
#define R_STATE_H

#include "p_level_defs.h"

/* level data */
extern blockmap_t blockmap; // TODO: move somewhere else, blockmap isn't for rendering

extern int numvertices;
extern vertex_t* vertices;

extern int numsegs;
extern seg_t* segs;

extern int numsectors;
extern sector_t* sectors;

extern int numnodes;
extern node_t* nodes;

extern int numlines;
extern line_t* lines;

extern int numsides;
extern side_t* sides;

#endif // R_STATE_H
