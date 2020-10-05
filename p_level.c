#include "p_level.h"

#include "p_level_defs.h"
#include "w_level_lumps.h"
#include "w_wad.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

blockmap_t blockmap;

int numvertices;
vertex_t* vertices;

int numsegs;
seg_t* segs;

int numsectors;
sector_t* sectors;

int numnodes;
node_t* nodes;

int numlines;
line_t* lines;

int numsides;
side_t* sides;

void load_blockmap(int lump)
{
    int16_t* lumpdata = w_load_lump(lump);

    blockmap.x = (float)lumpdata[0];
    blockmap.y = (float)lumpdata[1];
    blockmap.cols = (float)lumpdata[2];
    blockmap.rows = (float)lumpdata[3];

    uint16_t* offsets = lumpdata+4;

    int numblocks = blockmap.cols * blockmap.rows;
    blockmap.blocklists = malloc(numblocks * sizeof(int16_t*));

    for (int i = 0; i < numblocks; i++) {
        // every blocklist starts with 0x0000 and ends with 0xFFFF
        // so skip to the first index
        int off = offsets[i] + 1;
        // count number of lines in block
        int nlines = 0;
        while (lumpdata[off++] != -1)
            nlines++;

        blockmap.blocklists[i] = malloc((nlines + 1) * sizeof(int16_t));

        // fill blocklist, terminate with -1
        off = offsets[i] + 1;
        for (int j = 0; j < nlines; j++) {
            blockmap.blocklists[i][j] = lumpdata[off++];
        }
        blockmap.blocklists[i][nlines] = -1;
    }

    free(lumpdata);
}

void load_vertices(int lump)
{
    numvertices = w_get_lump_length(lump) / sizeof(vertexlump_t);
    vertices = malloc(numvertices * sizeof(vertex_t));

    uint8_t* lumpdata = w_load_lump(lump);

    vertexlump_t* vl = (vertexlump_t*) lumpdata;
    vertex_t* v = vertices;

    for (int i = 0; i < numvertices; i++, vl++, v++) {
        v->x = (float)vl->x;
        v->y = (float)vl->y;
    }

    free(lumpdata);
}

void load_sectors(int lump)
{
    numsectors = w_get_lump_length(lump) / sizeof(sectorlump_t);
    sectors = malloc(numsectors * sizeof(sector_t));

    uint8_t* lumpdata = w_load_lump(lump);

    sectorlump_t* sl = (sectorlump_t*) lumpdata;
    sector_t* s = sectors;

    for (int i = 0; i < numsectors; i++, sl++, s++) {
        s->floorheight = (float) sl->floorheight;
        s->ceilingheight = (float) sl->ceilingheight;
        s->floortex = 0;    // TODO: load textures
        s->ceilingtex = 0;
        s->lightlevel = sl->lightlevel;
        s->special = sl->special;
        s->tag = sl->tag;
    }

    free(lumpdata);
}

void load_sidedefs(int lump)
{
    numsides = w_get_lump_length(lump) / sizeof(sidedeflump_t);
    sides = malloc(numsides * sizeof(side_t));

    uint8_t* lumpdata = w_load_lump(lump);

    sidedeflump_t* sdl = (sidedeflump_t*) lumpdata;
    side_t* sd = sides;

    for (int i = 0; i < numsides; i++, sdl++, sd++) {
        sd->xoff = (float) sdl->xoff;
        sd->yoff = (float) sdl->yoff;
        sd->toptex = 0;  // TODO: load textures
        sd->bottex = 0;
        sd->midtex = 0;
        sd->sector = &sectors[sdl->sector];
    }

    free(lumpdata);
}

void load_linedefs(int lump)
{
    numlines = w_get_lump_length(lump) / sizeof(linedeflump_t);
    lines = malloc(numlines * sizeof(line_t));

    uint8_t* lumpdata = w_load_lump(lump);

    linedeflump_t* ldl = (linedeflump_t*) lumpdata;
    line_t* ld = lines;

    for(int i = 0; i < numlines; i++, ldl++, ld++) {
        ld->v1 = &vertices[ldl->v1];
        ld->v2 = &vertices[ldl->v2];
        ld->flags = ldl->flags;
        ld->special = ldl->special;
        ld->tag = ldl->tag;
        ld->sidenum[0] = ldl->sidenum[0];
        ld->sidenum[1] = ldl->sidenum[1];

        if (ld->sidenum[0] != -1)
            ld->frontsector = sides[ld->sidenum[0]].sector;
        else
            ld->frontsector = 0;

        if (ld->sidenum[1] != -1)
            ld->backsector = sides[ld->sidenum[1]].sector;
        else
            ld->backsector = 0;
    }

    free(lumpdata);
}

void p_load_level(int episode, int map)
{
    char lumpname[9];
    sprintf(lumpname, "E%dM%d", episode, map);
    LOG_INFO("Loading level: %s", lumpname);

    int lumpnum = w_get_lump_num(lumpname);
    if (lumpnum < 0) {
        LOG_ERROR("Unable to load level");
        exit(-1);
    }

    // don't change this order
    load_blockmap(lumpnum+ML_BLOCKMAP);
    load_vertices(lumpnum+ML_VERTEXES);
    load_sectors(lumpnum+ML_SECTORS);
    load_sidedefs(lumpnum+ML_SIDEDEFS);
    load_linedefs(lumpnum+ML_LINEDEFS);
    //load_subsectors(lumpnum+ML_SSECTORS);
    //load_nodes(lumpnum+ML_NODES);
    //load_segs(lumpnum+ML_SEGS);

    // TODO: load reject matrix
}
