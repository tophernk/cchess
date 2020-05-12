#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "move.h"

static void test_move_ctor() {
    move_t *move = move_new();
    move_ctor(move);

    assert_false(position_valid(move_get_from_position(move)));
    assert_false(position_valid(move_get_to_position(move)));
    assert_true(move_get_piece_type(move) == NONE);

    free(move);
}

static void test_move_compare() {
    move_t *a = move_new();
    move_ctor(a);
    move_t *b = move_new();
    move_ctor(b);

    move_t *as[1];
    as[0] = a;
    move_t *bs[1];
    bs[0] = b;

    assert_true(move_cmpr(as, bs, 1) == 0);

    move_set_score(a, 1);
    assert_true(move_cmpr(as, bs, 1) == 1);

    free(b);
    free(a);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_move_ctor),
            cmocka_unit_test(test_move_compare)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}