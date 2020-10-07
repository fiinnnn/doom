#include "c_mem.h"

#include "log.h"

typedef struct memblock_s
{
    uint32_t lifetime;
    void* ptr;
    struct memblock_s* next;
} memblock_t;

memblock_t* startblock;
memblock_t* endblock;

void append_block(memblock_t* blk)
{
    if (!startblock) {
        startblock = blk;
        endblock = blk;
        return;
    }

    endblock->next = blk;
    endblock = blk;
}

void* c_malloc(size_t size, uint32_t lifetime)
{
    memblock_t* blk = malloc(sizeof(memblock_t));
    blk->lifetime = lifetime;
    blk->ptr = malloc(size);
    blk->next = NULL;
    append_block(blk);
    return blk->ptr;
}

void* c_realloc(void* ptr, size_t size, uint32_t lifetime)
{
    if (ptr == NULL)
        return c_malloc(size, lifetime);

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    memblock_t* cur = startblock;
    while (cur) {
        if (cur->ptr == ptr)
            break;
        cur = cur->next;
    }

    cur->lifetime = lifetime;
    cur->ptr = realloc(ptr, size);
    return cur->ptr;
}

void remove_block(memblock_t* blk, memblock_t* prev)
{
    if (startblock == endblock) { // current block is start and end
        startblock = endblock = NULL;
    }
    else if (startblock == blk) { // current block is start
        startblock = blk->next;
    }
    else if (endblock == blk) { // current block is end
        endblock = prev;
        prev->next = NULL;
    }
    else { // current block is somewhere in between
        prev->next = blk->next;
    }
    free(blk->ptr);
    free(blk);
}

void c_free(void* ptr)
{
    if (!startblock) {
        LOG_WARN("c_free: Pointer not found, possible double free");
        return;
    }

    memblock_t* cur = startblock;
    memblock_t* prev = NULL;
    while (cur) {
        if (cur->ptr == ptr)
            break;
        prev = cur;
        cur = cur->next;
    }

    if (cur)
        remove_block(cur, prev);
    else
        LOG_WARN("c_free: Pointer not found, possible double free");
}

void c_free_lifetime(uint32_t lifetime)
{
    if (!startblock)
        return;

    memblock_t* cur = startblock;
    memblock_t* next;
    memblock_t* prev = NULL;
    while (cur) {
        next = cur->next;
        if (cur->lifetime >= lifetime) {
            remove_block(cur, prev);
        }
        else
            prev = cur;
        cur = next;
    }
}

void c_free_all()
{
    if (!startblock)
        return;

    memblock_t* cur = startblock;
    memblock_t* next;
    while (cur) {
        next = cur->next;
        free(cur->ptr);
        free(cur);
        cur = next;
    }
    startblock = NULL;
}
