/*
 * memory management
 */

#ifndef C_MEM_H
#define C_MEM_H

#include <stdlib.h>
#include <stdint.h>

/*
 * memblock lifetimes
 */
#define LT_STATIC 0     // runtime
#define LT_LEVEL  1     // until level exit

/*
 * allocate size bytes with given lifetime
 */
void* c_malloc(size_t size, uint32_t lifetime);

/*
 * reallocate memory block to new size
 */
void* c_realloc(void* ptr, size_t size, uint32_t lifetime);

/*
 * free memory
 */
void c_free(void* ptr);

/*
 * free all blocks with lifetime
 */
void c_free_lifetime(uint32_t lifetime);

/*
 * free all allocated blocks
 */
void c_free_all();

#endif // C_MEM_H
