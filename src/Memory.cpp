#include "Memory.h"

// ============================
// Memory Arena
// ============================

MemoryArena CreateArena() {
    MemoryArena ret = {};

    // Reserving Virtual Memory. Set given memory to PAGE_NOACCESS, so it can be read/write after commit
    // @Win32 Call
    ret.baseAddres = VirtualAlloc(0, ARENA_SIZE_MAX, MEM_RESERVE, PAGE_NOACCESS);

    return ret;
}

void* PushArena(MemoryArena* arena, uint64_t size) {

    if(arena->allocatedOffset + size > arena->commitedOffset) {

        // align to commit size
        uint64_t commitSize = size + ARENA_SIZE_MAX - 1;
        commitSize -= commitSize % ARENA_SIZE_MAX;

        // @Win32
        VirtualAlloc((char*) arena->baseAddres + arena->commitedOffset, commitSize, MEM_COMMIT, PAGE_READWRITE);
        arena->commitedOffset += commitSize;
    }

    void* pointer = (char*) arena->baseAddres + arena->allocatedOffset;
    arena->allocatedOffset += size;

    return pointer;
}

void ClearArena(MemoryArena* arena) {
    // @win32
    // @TODO: Zero memory only in debug builds
    ZeroMemory(arena->baseAddres, arena->allocatedOffset);
    arena->allocatedOffset = 0;
}

void DestroyArena(MemoryArena* arena) {
    // @Win32
    VirtualFree(arena->baseAddres, 0, MEM_RELEASE);

    arena->baseAddres = 0;
    arena->allocatedOffset = 0;
    arena->commitedOffset = 0;
}

//===========================
// Slice
//===========================

template <typename T>
Slice<T> MakeSlice(T* array, int start, int end) {
    Slice<T> ret = {0};
    int len = end - start;

    if(len > 0) {
        ret.data = array + start;
        ret.length = len;
    }

    return ret;
}


template <typename T>
Slice<T> AllocateSlice(int length) {
    Slice<T> ret = {0};

    ret.length = length;
    ret.data = (T*) malloc(sizeof(T) * length);
    assert(ret.data);

    return ret;
}