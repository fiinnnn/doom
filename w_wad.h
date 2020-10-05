/*
 * WAD I/O
 */

#ifndef W_WAD_H
#define W_WAD_H

#include <stdint.h>
#include <stdio.h>

/*
 * WAD file format
 *
 * +-----------+
 * | HEADER    |
 * +-----------+
 * | LUMPS     | data
 * |           |
 * |           |
 * |           |
 * +-----------+
 * | DIRECTORY | lump name, size and location,
 * |           | one entry for each lump
 * +-----------+
 *
 * when loading WADs, all lumps descriptions are placed in a lumpinfo_t array
 * in the order they were loaded
 */

/* WAD header */
typedef struct {
    char        id[4];         // IWAD or PWAD, (internal and patch WAD)
    uint32_t    numlumps;      // number of lumps in directory
    uint32_t    infotableofs;  // location of the first directory entry
} wadinfo_t;

/* directory entry */
typedef struct {
    uint32_t    filepos;       // lump location
    uint32_t    size;          // lump size
    char        name[8];       // lump name
} direntry_t;

/* lump info struct */
typedef struct {
    char        name[8];       // lump name
    FILE*       handle;        // file containing lump
    uint32_t    position;      // lump position in file
    uint32_t    size;          // lump size
} lumpinfo_t;

extern int numlumps;
extern lumpinfo_t* lumpinfo;

/*
 * loads all provided files and fills lumpinfo
 */
void w_init_files(char** files);

/*
 * gets the lumpinfo index for the specified lump name,
 * returns -1 if not found
 */
int w_get_lump_num(char* name);

/*
 * returns the size of the specififed lump
 */
int w_get_lump_length(int lump);

/*
 * allocates a buffer, loads lump contents, and returns a pointer to the buffer
 */
void* w_load_lump(int lump);

#endif // W_WAD_H
