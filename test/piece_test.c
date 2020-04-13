#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "piece.h"

void test_piece_ctor(void **state) {
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

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_piece_ctor)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
