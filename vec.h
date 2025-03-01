#ifndef VEC_H
#define VEC_H

#include <stddef.h>

typedef struct vec {
    void* base;
    size_t element_size;
    size_t allocated;
    size_t length;
}* Vec;

Vec new_vec(size_t element_size);
Vec vec_copy(Vec v);
void vec_free(Vec v);
size_t vec_length(Vec v);
void vec_add(Vec v, void* item);
void* vec_items(Vec v);

#endif
