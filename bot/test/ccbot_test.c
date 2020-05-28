#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"
#include "ccbot.h"

static void test_config_cpu_move() {
    config_t *config = config_new();
    char *fen = "1n2k3/p7/8/8/8/8/1P6/2B1KB2 w  - 0 0";
    config_fen_in(config, fen);

    int piece_moved = ccbot_execute_move(config);
    assert_true(piece_moved);

    config_dtor(config);
    free(config);
}

static void test_config_cpu_move_from_standard_starting_position() {
    config_t *config = config_new();
    // a3 has been played
    config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 0");

    int piece_moved = ccbot_execute_move(config);
    assert_true(piece_moved);

    config_dtor(config);
    free(config);
}

static void test_config_multiple_cpu_moves() {
    config_t *config = config_new();
    config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");

    int piece_moved = ccbot_execute_move(config);
    assert_true(piece_moved);
    piece_moved = ccbot_execute_move(config);
    assert_true(piece_moved);

    config_t *copy = config_new();
    config_ctor(copy);
    config_copy(config, copy);

    piece_moved = ccbot_execute_move(config);
    assert_true(piece_moved);

    config_dtor(copy);
    config_dtor(config);
    free(copy);
    free(config);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_config_cpu_move),
            cmocka_unit_test(test_config_cpu_move_from_standard_starting_position),
            cmocka_unit_test(test_config_multiple_cpu_moves)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
