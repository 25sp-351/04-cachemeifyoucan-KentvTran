#include "cut_list.h"

#include <stdio.h>
#include <stdlib.h>

#include "piece_values.h"
#include "vec.h"

CutList *new_cutlist(Vec length_and_values, PieceLength length) {
    CutList *cl           = malloc(sizeof(CutList));
    cl->groups            = new_vec(sizeof(PieceGroup));

    PieceLengthValue *pvv = vec_items(length_and_values);
    for (int ix = 0; ix < vec_length(length_and_values); ix++) {
        PieceGroup pg = (PieceGroup){.pv = pvv[ix], .count = 0};
        vec_add(cl->groups, &pg);
    }

    cl->remainder   = length;
    cl->total_value = 0;
    return cl;
}

CutList *cutlist_copy(CutList *cl) {
    CutList *ncl     = malloc(sizeof(CutList));
    ncl->groups      = vec_copy(cl->groups);
    ncl->remainder   = cl->remainder;
    ncl->total_value = cl->total_value;
    return ncl;
}

void cutlist_free(CutList *cl) {
    vec_free(cl->groups);
    free(cl);
}

bool cutlist_can_add_piece(CutList *cl, PieceLengthValue pv) {
    return pv.length <= cl->remainder;
}

CutList *cutlist_add_piece(CutList *cl, PieceLengthValue pv) {
    if (!cutlist_can_add_piece(cl, pv))
        return cl;

    PieceGroup *groups = vec_items(cl->groups);

    cl->remainder -= pv.length;
    cl->total_value += pv.value;

    // Increment piece count if we're already cutting this length
    for (int group_ix = 0; group_ix < cl->groups->length; group_ix++) {
        if (pvs_equal(&groups[group_ix].pv, &pv)) {
            groups[group_ix].count++;
            return cl;
        }
    }

    PieceGroup g = (PieceGroup){.pv = pv, .count = 1};
    vec_add(cl->groups, &g);
    return cl;
}

void cutlist_print(CutList *cl) {
    PieceGroup *groups = vec_items(cl->groups);
    for (int ix = 0; ix < cl->groups->length; ix++) {
        if (groups[ix].count) {
            printf("%2d @ %4d = %4d\n", groups[ix].count, groups[ix].pv.length,
                   groups[ix].pv.value);
        }
    }
    printf("Remainder: %5d\n", cl->remainder);
    printf("Value:     %5d\n", cl->total_value);
}

CutList *choose_best_cuts(CutList *starting_cutlist, Vec pv) {
    CutList *best_option_so_far = cutlist_copy(starting_cutlist);
    // Handle allocation failure
    if (!best_option_so_far) {
        cutlist_free(starting_cutlist);
        return NULL;
    }

    PieceLengthValue *length_to_try = vec_items(pv);
    for (size_t ix = 0; ix < vec_length(pv); ix++) {
        if (!cutlist_can_add_piece(starting_cutlist, length_to_try[ix]))
            continue;

        CutList *new_cutlist = cutlist_copy(starting_cutlist);
        // Handle allocation failure
        if (!new_cutlist) {
            cutlist_free(best_option_so_far);
            cutlist_free(starting_cutlist);
            return NULL;
        }

        cutlist_add_piece(new_cutlist, length_to_try[ix]);

        CutList *maybe_better_option = choose_best_cuts(new_cutlist, pv);
        // Handle Recursion Error
        if (!maybe_better_option) {
            cutlist_free(new_cutlist);
            continue;
        }

        if (maybe_better_option->total_value >
            best_option_so_far->total_value) {
            cutlist_free(best_option_so_far);
            best_option_so_far = maybe_better_option;
        } else {
            cutlist_free(maybe_better_option);
        }
    }

    cutlist_free(starting_cutlist);
    return best_option_so_far;
}

CutList *optimal_cutlist_for(Vec pv, PieceLength total_length) {
    return choose_best_cuts(new_cutlist(pv, total_length), pv);
}
