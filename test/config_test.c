#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"

static void test_config_add_piece(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    position_t *position = position_new();
    position_set_x(position, 0);
    position_set_y(position, 1);

    piece_t *piece = NULL;

    piece = config_get_piece(config, WHITE, position);
    assert_null(piece);

    config_add_piece(config, PAWN_W, 0, 1, WHITE, 0);

    piece = config_get_piece(config, WHITE, position);
    assert_non_null(piece);
    assert_int_equal(piece_get_type(piece), PAWN_W);
    assert_true(position_equal(piece_get_current_position(piece), position));
}

static void test_config_eval(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    config_add_piece(config, PAWN_W, 0, 1, WHITE, 0);
    assert_true(config_eval(config, WHITE) > config_eval(config, BLACK));

    config_add_piece(config, PAWN_B, 0, 6, BLACK, 0);
    assert_true(config_eval(config, WHITE) == config_eval(config, BLACK));

    config_add_piece(config, PAWN_B, 1, 6, BLACK, 1);
    assert_true(config_eval(config, WHITE) < config_eval(config, BLACK));
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_config_add_piece),
            cmocka_unit_test(test_config_eval)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
