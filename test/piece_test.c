#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "piece.h"

static void test_piece_ctor(void **state) {
    piece_t *piece = piece_new();
    piece_ctor(piece);

    assert_int_equal(NONE, piece_get_type(piece));
    assert_false(position_valid(piece_get_current_position(piece)));
    for (int i = 0; i < MAX_POSITIONS; i++) {
        assert_false(position_valid(piece_get_available_position(piece, i)));
    }

    piece_dtor(piece);
    free(piece);
}

static void test_piece_copy(void **state) {
    piece_t *piece = piece_new();
    piece_ctor(piece);
    piece_set_current_position(piece, 1, 1);
    piece_set_type(piece, BISHOP_W);
    piece_set_available_position(piece, 2, 2, 2);

    piece_t *copy = piece_new();
    piece_ctor(copy);

    assert_int_not_equal(piece_get_type(piece), piece_get_type(copy));
    assert_false(position_equal(piece_get_current_position(piece), piece_get_current_position(copy)));
    assert_false(position_equal(piece_get_available_position(piece, 2), piece_get_available_position(copy, 2)));

    piece_copy(piece, copy);

    assert_int_equal(piece_get_type(piece), piece_get_type(copy));
    assert_true(position_equal(piece_get_current_position(piece), piece_get_current_position(copy)));
    assert_true(position_equal(piece_get_available_position(piece, 2), piece_get_available_position(copy, 2)));

    piece_dtor(piece);
    free(piece);
    piece_dtor(copy);
    free(copy);
}

static void test_piece_get_color(void **state) {
    assert_int_equal(piece_get_color(PAWN_B), BLACK);
    assert_int_equal(piece_get_color(PAWN_W), WHITE);
}

static void test_piece_to_char(void **state) {
    assert_int_equal(piece_type_to_char(PAWN_B), 'P');
    assert_int_equal(piece_type_to_char(PAWN_W), 'P');
}

static void test_piece_invalidate_available_positions(void **state) {
    piece_t *piece = piece_new();
    piece_ctor(piece);

    for (int i = 0; i < MAX_POSITIONS; i++) {
        assert_false(position_valid(piece_get_available_position(piece, i)));
    }

    for (int i = 0; i < MAX_POSITIONS; i++) {
        piece_set_available_position(piece, i, i, i);
    }

    for (int i = 0; i < MAX_POSITIONS; i++) {
        assert_true(position_valid(piece_get_available_position(piece, i)));
    }

    piece_invalidate_available_positions(piece);

    for (int i = 0; i < MAX_POSITIONS; i++) {
        assert_false(position_valid(piece_get_available_position(piece, i)));
    }

    position_dtor(piece);
    free(piece);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_piece_ctor),
            cmocka_unit_test(test_piece_copy),
            cmocka_unit_test(test_piece_get_color),
            cmocka_unit_test(test_piece_to_char),
            cmocka_unit_test(test_piece_invalidate_available_positions)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
