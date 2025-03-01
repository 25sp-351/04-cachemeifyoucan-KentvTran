#ifndef PIECE_VALUES_H
#define PIECE_VALUES_H

#include <stdbool.h>
#include <stdio.h>

#include "vec.h"

typedef int PieceLength;

typedef struct {
    int length;
    int value;
} PieceLengthValue;

typedef struct {
    PieceLengthValue pv;
    int count;
} PieceGroup;

// Returns NULL if there is an error in the input
// Returns them sorted by value then length (both descending)
Vec read_piece_values(FILE *file);
bool _list_contains_length(Vec ptr, PieceLength len);
void print_piece_values(Vec v);
bool pvs_equal(PieceLengthValue *pv1, PieceLengthValue *pv2);
int compare_piece_values(PieceLengthValue *a, PieceLengthValue *b);
#endif  // PIECE_VALUES_H
