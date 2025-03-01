#include "piece_values.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "vec.h"

bool _list_contains_length(Vec ptr, PieceLength len);

// by value then length, both descending
int compare_piece_values(PieceLengthValue *a, PieceLengthValue *b) {
    if (a->value != b->value)
        return b->value - a->value;
    return b->length - a->length;
}

Vec read_piece_values(FILE *file) {
    Vec val_list = new_vec(sizeof(PieceLengthValue));
    PieceLengthValue item;

    while (fscanf(file, "%d, %d\n", &item.length, &item.value) == 2) {

        if (item.length < 1 || item.value < 1 ||
            _list_contains_length(val_list, item.length)) {
            fprintf(stderr, "Invalid input in file: length = %d, value = %d\n",
                    item.length, item.value);
            vec_free(val_list);
            return NULL;
        }
        vec_add(val_list, &item);
    }

    if (vec_length(val_list) == 0) {
        fprintf(stderr, "No valid entries found in file\n");
        vec_free(val_list);
        return NULL;
    }

    qsort(vec_items(val_list), vec_length(val_list), sizeof(PieceLengthValue),
          (int (*)(const void *, const void *))compare_piece_values);

    return val_list;
}

bool _list_contains_length(Vec v, PieceLength len) {
    PieceLengthValue *items = vec_items(v);
    for (int ix = 0; ix < vec_length(v); ix++)
        if (items[ix].length == len)
            return true;
    return false;
}

void print_piece_values(Vec v) {
    PieceLengthValue *items = vec_items(v);
    for (int ix = 0; ix < v->length; ix++)
        printf("length %4d, value %4d\n", items[ix].length, items[ix].value);
}

bool pvs_equal(PieceLengthValue *pv1, PieceLengthValue *pv2) {
    return pv1->length == pv2->length && pv1->value == pv2->value;
}
