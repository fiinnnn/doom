#include "w_wad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "log.h"
#include "c_mem.h"

wadinfo_t* headers;
void** wads;

int numlumps;
lumpinfo_t* lumpinfo;

/*
 * loads the specified WAD file
 */
void add_file(char* filename, int i)
{
    FILE* handle = fopen(filename, "rb");
    if (!handle) {
        LOG_ERROR("Unable to open WAD: %s", filename);
        return;
    }

    LOG_INFO("Adding WAD: %s", filename);

    fseek(handle, 0, SEEK_END);
    long filelength = ftell(handle);
    wads[i] = c_malloc(filelength, LT_STATIC);

    fseek(handle, 0, SEEK_SET);
    fread(wads[i], sizeof(uint8_t), filelength, handle);
    fclose(handle);

    wadinfo_t header = *(wadinfo_t*) wads[i];

    if (strncmp(header.id, "IWAD", 4) && strncmp(header.id, "PWAD", 4)) {
        LOG_ERROR("%s doesn't have IWAD or PWAD id", filename);
        return;
    }

    direntry_t* directory = (direntry_t*) (wads[i] + header.infotableofs);

    int start_idx = numlumps;
    numlumps += header.numlumps;

    lumpinfo = c_realloc(lumpinfo, numlumps * sizeof(lumpinfo_t), LT_STATIC);
    if (!lumpinfo) {
        LOG_ERROR("Unable to realloc lumpinfo");
        numlumps -= header.numlumps;
        return;
    }

    lumpinfo_t* lump = &lumpinfo[start_idx];

    for (int j = start_idx; j < numlumps; j++,lump++,directory++) {
        lump->wad_index = i;
        lump->position = directory->filepos;
        lump->size = directory->size;
        strncpy(lump->name, directory->name, 8);
    }
}

void w_init_files(char** files)
{
    numlumps = 0;

    int numfiles = 0;
    for (; files[numfiles]; numfiles++) {}

    headers = c_malloc(numfiles * sizeof(wadinfo_t), LT_STATIC);
    wads = c_malloc(numfiles * sizeof(void*), LT_STATIC);

    int i = 0;
    for (; *files; files++)
        add_file(*files, i++);

    if (!numlumps) {
        LOG_ERROR("No lumps loaded");
        exit(-1);
    }
}

int w_get_lump_num(char* name)
{
    union {
        char s[9];
        int  x[2];
    } lname;
    lname.s[8] = 0;

    strncpy(lname.s, name, 8); // convert name to ints for fast comparison

    lumpinfo_t* lump = lumpinfo + numlumps;

    while (lump-- != lumpinfo) {
        if (*(int*) lump->name == lname.x[0]
         && *(int*) &lump->name[4] == lname.x[1]) {
            return lump - lumpinfo;
        }
    }

    return -1;
}

int w_get_lump_length(int lump)
{
    if (lump >= numlumps) {
        LOG_ERROR("Lump index out of bounds");
        exit(-1);
    }

    return lumpinfo[lump].size;
}

void* w_load_lump(int lump)
{
    if (lump >= numlumps) {
        LOG_ERROR("Lump index out of bounds");
        exit(-1);
    }

    uint8_t* buf = c_malloc(w_get_lump_length(lump), LT_STATIC);

    lumpinfo_t* l = lumpinfo+lump;
    uint8_t* ptr = (uint8_t*) (wads[l->wad_index] + l->position);
    for (int i = 0; i < l->size; i++)
        buf[i] = ptr[i];

    return (void*) buf;
}

void w_log_lumpinfo()
{
    LOG_INFO("Available lumps:");
    for (int i = 0; i < numlumps; i++) {
        lumpinfo_t lump = lumpinfo[i];
        LOG_INFO("NAME: %8.8s POS: %8d SIZE: %d",
                lump.name, lump.position, lump.size);
    }
}
