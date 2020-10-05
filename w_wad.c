#include "w_wad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "log.h"

int numlumps;
lumpinfo_t* lumpinfo;

/*
 * appends all lumps from the specified file to the lumpinfo array
 */
void add_file(char* filename)
{
    FILE* handle = fopen(filename, "rb");
    if (!handle) {
        LOG_ERROR("Unable to open WAD: %s", filename);
        return;
    }

    LOG_INFO("Adding WAD: %s", filename);

    wadinfo_t header;
    fread(&header, sizeof(header), 1, handle);

    if (strncmp(header.id, "IWAD", 4) && strncmp(header.id, "PWAD", 4)) {
        LOG_ERROR("%s doesn't have IWAD or PWAD id", filename);
        return;
    }

    direntry_t* directory;
    directory = alloca(header.numlumps * sizeof(direntry_t));
    fseek(handle, header.infotableofs, SEEK_SET);
    fread(directory, sizeof(direntry_t), header.numlumps, handle);

    int start_idx = numlumps;
    numlumps += header.numlumps;

    lumpinfo = realloc(lumpinfo, numlumps * sizeof(lumpinfo_t));
    if (!lumpinfo) {
        LOG_ERROR("Unable to realloc lumpinfo");
        numlumps -= header.numlumps;
        return;
    }

    lumpinfo_t* lump = &lumpinfo[start_idx];

    for (int i = start_idx; i < numlumps; i++,lump++,directory++) {
        lump->handle = handle;
        lump->position = directory->filepos;
        lump->size = directory->size;
        strncpy(lump->name, directory->name, 8);
    }

    // files stay open, consider loading WADs in memory instead
}

void w_init_files(char** files)
{
    numlumps = 0;

    for (; *files; files++)
        add_file(*files);

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

    void* buf = malloc(w_get_lump_length(lump));

    lumpinfo_t* l = lumpinfo+lump;
    fseek(l->handle, l->position, SEEK_SET);
    fread(buf, 1, l->size, l->handle);

    return buf;
}
