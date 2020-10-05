#include "am_map.h"

#include "def.h"
#include "r_state.h"

#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>

static int grid = 0;      // toggles grid drawing
static int all_lines = 0; // toggles drawing all lines

// colors
#define GRID_COLOR          0x4F4F4FFF
#define WALL_COLOR          0xB00000FF
#define SECRET_COLOR        0x00B000FF
#define FLOORCHANGE_COLOR   0xBF7B4BFF
#define CEILINGCHANGE_COLOR 0xFFFF00FF
#define MISC_LINEDEF_COLOR  0x707070FF

// map pan / zoom amounts
#define PAN_INC      4
#define ZOOMIN_MUL   1.02
#define ZOOMOUT_MUL (1/1.02)

// point and line in screen space
typedef struct {
    int x, y;
} spoint_t;

typedef struct {
    spoint_t a, b;
} sline_t;

// point and line in map space
typedef struct {
    float x, y;
} mpoint_t;

typedef struct {
    mpoint_t a, b;
} mline_t;

static uint32_t* fb;

// map position in screen space
static int am_x = 0;
static int am_y = 0;
static int am_w = SCREENWIDTH;
static int am_h = SCREENHEIGHT - 32; // leave space for status bar

// viewport in map space
static float vp_x, vp_y; // lower left corner
static float vp_w, vp_h; // dimensions

// map bounds
static float min_x, min_y;
static float max_x, max_y;

// map scale
static float scale;
static float scale_inv;

// zoom / pan update
static mpoint_t pan_inc;
static float scale_mul;

// map space to screen space conversion
#define MTOS(x)  ((x)*scale)
#define STOM(x)  ((x)*scale_inv)
#define MXTOS(x) (am_x + MTOS((x)-vp_x))
#define MYTOS(y) (am_y + (am_h - MTOS((y) - vp_y)))

/*
 * level initialization
 */
void init_level()
{
    // find level boundaries
    min_x = min_y = INT_MAX;
    max_x = max_y = INT_MIN;

    for (int i = 0; i < numvertices; i++) {
        if (vertices[i].x < min_x)
            min_x = vertices[i].x;
        else if (vertices[i].x > max_x)
            max_x = vertices[i].x;

        if (vertices[i].y < min_y)
            min_y = vertices[i].y;
        else if (vertices[i].y > max_y)
            max_y = vertices[i].y;
    }
}

void am_init(uint32_t* buf)
{
    init_level();

    fb = buf;

    scale = 0.2;
    scale_inv = 1.0/scale;

    pan_inc.x = 0;
    pan_inc.y = 0;
    scale_mul = 1.0;

    vp_w = STOM(am_w); // TODO: zoom
    vp_h = STOM(am_h);

    vp_x = min_x; // TODO: center on player
    vp_y = min_y;
}

void am_handle_event(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                pan_inc.y = STOM(PAN_INC);
                break;
            case SDL_SCANCODE_DOWN:
                pan_inc.y = -STOM(PAN_INC);
                break;
            case SDL_SCANCODE_LEFT:
                pan_inc.x = -STOM(PAN_INC);
                break;
            case SDL_SCANCODE_RIGHT:
                pan_inc.x = +STOM(PAN_INC);
                break;
            case SDL_SCANCODE_KP_PLUS:
                scale_mul = ZOOMIN_MUL;
                break;
            case SDL_SCANCODE_KP_MINUS:
                scale_mul = ZOOMOUT_MUL;
                break;
        }
    }
    else if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                pan_inc.y = 0;
                break;
            case SDL_SCANCODE_DOWN:
                pan_inc.y = 0;
                break;
            case SDL_SCANCODE_LEFT:
                pan_inc.x = 0;
                break;
            case SDL_SCANCODE_RIGHT:
                pan_inc.x = 0;
                break;
            case SDL_SCANCODE_KP_PLUS:
                scale_mul = 1.0;
                break;
            case SDL_SCANCODE_KP_MINUS:
                scale_mul = 1.0;
                break;
            case SDL_SCANCODE_G:
                grid = !grid;
                break;
            case SDL_SCANCODE_D:
                all_lines = !all_lines;
                break;
        }
    }
}

void update_pos()
{
    vp_x += pan_inc.x;
    vp_y += pan_inc.y;

    // TODO: bounds check
}

void update_scale()
{
    scale *= scale_mul;
    scale_inv = 1.0/scale;

    // TODO: bounds check

    vp_x += vp_w / 2;
    vp_y += vp_h / 2;
    vp_w = STOM(am_w);
    vp_h = STOM(am_h);
    vp_x -= vp_w / 2;
    vp_y -= vp_h / 2;
}

void am_update()
{
    if (pan_inc.x || pan_inc.y)
        update_pos();

    if (scale_mul != 1.0)
        update_scale();
}

/*
 * Bresenham line drawing in screen space
 */
void draw_sline(sline_t* l, uint32_t c)
{
    int x0 = l->a.x;
    int y0 = l->a.y;
    int x1 = l->b.x;
    int y1 = l->b.y;

    int dx = x1 - x0;
    int ax = 2 * abs(dx);
    int sx = dx < 0 ? -1 : 1;

    int dy = y1 - y0;
    int ay = 2 * abs(dy);
    int sy = dy < 0 ? -1 : 1;

#define PLOT(x, y) fb[(y)*am_w+(x)]=c

    int d;

    if (ax > ay) {
        d = ay - ax / 2;
        while (1) {
            PLOT(x0, y0);
            if (x0 == x1) return;
            if (d >= 0) {
                y0 += sy;
                d -= ax;
            }
            x0 += sx;
            d += ay;
        }
    }
    else {
        d = ax - ay / 2;
        while (1) {
            PLOT(x0, y0);
            if (y0 == y1) return;
            if (d >= 0) {
                x0 += sx;
                d -= ay;
            }
            y0 += sy;
            d += ax;
        }
    }
}

/*
 * Cohen-Sutherland map space line clipping and conversion to screen space
 */
int clip_mline(mline_t* ml, sline_t* sl)
{
    // outcodes
    enum {
        INSIDE = 0, // 0000
        LEFT = 1,   // 0001
        RIGHT = 2,  // 0010
        BOTTOM = 4, // 0100
        TOP = 8,    // 1000
    };

    int outcodea = INSIDE;
    int outcodeb = INSIDE;

    // trivial rejects
    if (ml->a.y < vp_y)
        outcodea |= BOTTOM;
    else if (ml->a.y > vp_y + vp_h)
        outcodea |= TOP;

    if (ml->b.y < vp_y)
        outcodeb |= BOTTOM;
    else if (ml->b.y > vp_y + vp_h)
        outcodeb |= TOP;

    if (outcodea & outcodeb)
        return 0;

    if (ml->a.x < vp_x)
        outcodea |= LEFT;
    else if (ml->a.x > vp_x + vp_w)
        outcodea |= RIGHT;

    if (ml->b.x < vp_x)
        outcodeb |= LEFT;
    else if (ml->b.x > vp_x + vp_w)
        outcodeb |= RIGHT;

    if (outcodea & outcodeb)
        return 0;

    // transform to screen space
    sl->a.x = MXTOS(ml->a.x);
    sl->a.y = MYTOS(ml->a.y);
    sl->b.x = MXTOS(ml->b.x);
    sl->b.y = MYTOS(ml->b.y);

    // screen space outcode calculation
#define OUTCODE(o, x, y) \
    (o) = INSIDE; \
    if ((y) < 0) (o) |= TOP; \
    else if ((y) > am_h) (o) |= BOTTOM; \
    if ((x) < 0) (o) |= LEFT; \
    else if ((x) > am_w) (o) |= RIGHT;

    OUTCODE(outcodea, sl->a.x, sl->a.y);
    OUTCODE(outcodeb, sl->b.x, sl->b.y);

    int dx, dy;
    spoint_t p;
    while (outcodea | outcodeb) {
        int out;
        if (outcodea)
            out = outcodea;
        else
            out = outcodeb;

        dx = sl->b.x - sl->a.x;
        dy = sl->b.y - sl->a.y;

        if (out & LEFT) { // left of viewport
            p.x = 0;
            p.y = sl->a.y + (dy * (-sl->a.x)) / dx;
        }
        else if (out & RIGHT) { // right of viewport
            p.x = am_w - 1;
            p.y = sl->a.y + (dy * (am_w - 1 - sl->a.x)) / dx;
        }
        else if (out & BOTTOM) { // below viewport
            dy = -dy;
            p.x = sl->a.x + (dx * (sl->a.y - am_h)) / dy;
            p.y = am_h - 1;
        }
        else if (out & TOP) { // above viewport
            dy = -dy;
            p.x = sl->a.x + (dx * (sl->a.y)) / dy;
            p.y = 0;
        }

        if (out == outcodea) {
            sl->a = p;
            OUTCODE(outcodea, sl->a.x, sl->a.y);
        }
        else {
            sl->b = p;
            OUTCODE(outcodeb, sl->b.x, sl->b.y);
        }

        if (outcodea & outcodeb)
            return 0;
    }

    return 1;
}

/*
 * draws line after clipping
 */
void draw_mline(mline_t* ml, int c)
{
    sline_t sl;

    if (clip_mline(ml, &sl))
        draw_sline(&sl, c);
}

void draw_grid()
{
    int start, end;
    mline_t l;

    // vertical lines
    start = vp_x;
    if ((start - blockmap.x) % MAPBLOCKSIZE)
        start += MAPBLOCKSIZE - ((start - blockmap.x) % MAPBLOCKSIZE);
    end = vp_x + vp_w;

    l.a.y = vp_y;
    l.b.y = vp_y + vp_h;
    for (int x = start; x < end; x += MAPBLOCKSIZE) {
        l.a.x = x;
        l.b.x = x;
        draw_mline(&l, GRID_COLOR);
    }

    // horizontal lines
    start = vp_y;
    if ((start - blockmap.y) % MAPBLOCKSIZE)
        start += MAPBLOCKSIZE - ((start - blockmap.y) % MAPBLOCKSIZE);
    end = vp_y + vp_h;

    l.a.x = vp_x;
    l.b.x = vp_x + vp_w;
    for (int y = start; y < end; y += MAPBLOCKSIZE) {
        l.a.y = y;
        l.b.y = y;
        draw_mline(&l, GRID_COLOR);
    }
}

void draw_walls()
{
    mline_t l;
    for (int i = 0; i < numlines; i++) {
        l.a.x = lines[i].v1->x;
        l.a.y = lines[i].v1->y;
        l.b.x = lines[i].v2->x;
        l.b.y = lines[i].v2->y;

        if (lines[i].flags & ML_DONTDRAW && !all_lines)
            continue;

        if (!lines[i].backsector)
            draw_mline(&l, WALL_COLOR);
        else {
            if (lines[i].flags & ML_SECRET) {
                if (all_lines) // draw secret walls in different color
                    draw_mline(&l, SECRET_COLOR);
                else
                    draw_mline(&l, WALL_COLOR);
            }
            else if (lines[i].frontsector->floorheight !=
                     lines[i].backsector->floorheight) {
                draw_mline(&l, FLOORCHANGE_COLOR);
            }
            else if (lines[i].frontsector->ceilingheight !=
                     lines[i].backsector->ceilingheight) {
                draw_mline(&l, CEILINGCHANGE_COLOR);
            }
            else if (all_lines) {
                draw_mline(&l, MISC_LINEDEF_COLOR);
            }
        }
    }
}

void am_draw()
{
    if (grid)
        draw_grid();

    draw_walls();
}
