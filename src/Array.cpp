#include "Array.h"

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