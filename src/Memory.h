#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define Bytes(n) n
#define Kilobytes(n) (1024 * (uint64_t)(n))
#define Megabytes(n) (1024 * (uint64_t)Kilobytes(n))
#define Gigabytes(n) (1024 * (uint64_t)Megabytes(n))

// Based on Ryan Fleury's implementation: https://github.com/ryanfleury/app_template/blob/master/source/memory.h
#define ARENA_SIZE_MAX Gigabytes(1)
#define ARENA_COMMIT_SIZE Kilobytes(4);

struct MemoryArena {
    void* baseAddres;
    uint64_t allocatedOffset;
    uint64_t commitedOffset;
};

void* PushArena(MemoryArena* arena, uint64_t size);

#endif