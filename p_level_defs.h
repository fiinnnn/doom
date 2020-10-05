/*
 * level data type definitions
 */

#ifndef P_LEVEL_DEFS_H
#define P_LEVEL_DEFS_H

#include <stdint.h>

typedef struct {
    float x;
    float y;
} vertex_t;

struct line_s; // forward decl for sector

typedef struct
{
    float floorheight;
    float ceilingheight;

    int16_t floortex;
    int16_t ceilingtex;

    int16_t lightlevel;

    int16_t special;
    int16_t tag;

    // linedefs
    int             numlines;
    struct line_s** lines;

    //TODO: store things in sector
} sector_t;

typedef struct {
    // texture offset
    float xoff;
    float yoff;

    // texture indices
    int16_t toptex;
    int16_t bottex;
    int16_t midtex;

    // facing sector
    sector_t* sector;
} side_t;

typedef struct line_s {
    // start (v1) and end (v2) vertices
    vertex_t* v1;
    vertex_t* v2;

    // animations
    int16_t flags;
    int16_t special;
    int16_t tag;

    // sidedefs (sidenum[1] = -1 if one sided)
    int16_t sidenum[2];

    // shortcut so you don't have to through sidedefs
    sector_t* frontsector;
    sector_t* backsector;
} line_t;

/* linedef flags */
#define ML_BLOCKING         1   // solid obstacle
#define ML_BLOCKMONSTERS    2   // only block monsters

#define ML_TWOSIDED         4   // backside won't be present if not set

#define ML_DONTPEGTOP       8   // texture alignment
#define ML_DONTPEGBOTTOM    16

#define ML_SECRET           32  // don't draw as two sided on automap

#define ML_SOUNDBLOCK       64  // blocks sound

#define ML_DONTDRAW         128 // don't draw line on the automap

#define ML_MAPPED           256 // set if seen by player, draw line in automap

typedef struct {
    sector_t* sector;

    // defines list of visible line segments in the subsector
    int16_t numsegs;
    int16_t firstseg;
} subsector_t;

typedef struct {
    vertex_t* v1;
    vertex_t* v2;

    int angle;

    float offset;

    side_t* sidedef;
    line_t* linedef;
} seg_t;

typedef struct {
    // partition line (x,y) to (x+dx,y+dy)
    float x;
    float y;
    float dx;
    float dy;

    // right and left child bounding box
    // (top, bottom, left, right)
    float bbox[2][4];

    // child node indices
    // if NF_SUBSECTOR is set the child is a leaf
    uint16_t children[2];
} node_t;

typedef struct {
    int x;
    int y;
    int cols;
    int rows;

    // cols * rows number of blocklists each containing line indices,
    // terminated by -1
    int16_t** blocklists;
} blockmap_t;

#endif // P_LEVEL_DEFS_H
