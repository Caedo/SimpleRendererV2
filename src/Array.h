#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>

template <typename T>
struct Slice {
    T* data;
    size_t length;

    T& operator[] (size_t index) {
        assert(index < length);
        return data[index];
    }
};

template <typename T>
Slice<T> MakeSlice(T* array, int start, int end);

template <typename T>
Slice<T> AllocateSlice(int length);

#endif