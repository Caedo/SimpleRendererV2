#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <assert.h>

#define Bytes(n) n
#define Kilobytes(n) (1024 * (uint64_t)(n))
#define Megabytes(n) (1024 * (uint64_t)Kilobytes(n))
#define Gigabytes(n) (1024 * (uint64_t)Megabytes(n))

// ======================================
// Memory arena 
// ======================================

// Based on Ryan Fleury's implementation

#define ARENA_SIZE_MAX Gigabytes(1)
#define ARENA_COMMIT_SIZE Kilobytes(4);

struct MemoryArena {
    void* baseAddres;
    uint64_t allocatedOffset;
    uint64_t commitedOffset;
};

void* PushArena(MemoryArena* arena, uint64_t size);
void ClearArena(MemoryArena* arena);
void DestroyArena(MemoryArena* arena);

// ======================================
// Slice 
// ======================================

template <typename T>
struct Slice {
    T* data;
    ptrdiff_t length;

    T& operator[] (int index) {
        assert(index < length);
        return data[index];
    }
};

template <typename T>
Slice<T> MakeSlice(T* array, int start, int end);

template <typename T>
Slice<T> AllocateSlice(int length);


#endif