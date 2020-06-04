#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"
#include "ccbot.h"

static void test_ccbot_move() {
    config_t *config = config_new();
    char *in = "1n2k3/p7/8/8/8/8/1P6/2B1KB2 w  - 0 0";
    config_fen_in(config, in);

    ccbot_execute_move(config);
    char out[100];
    config_fen_out(config, out);
    assert_string_not_equal(in, out);

    config_dtor(config);
    free(config);
}

static void test_ccbot_move_from_standard_starting_position() {
    config_t *config = config_new();
    // a3 has been played
    char *in = "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 0";
    config_fen_in(config, in);

    ccbot_execute_move(config);
    char out[100];
    config_fen_out(config, out);
    assert_string_not_equal(in, out);

    config_dtor(config);
    free(config);
}

static void test_ccbot_multiple_moves() {
    config_t *config = config_new();
    config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");

    int move_executed = ccbot_execute_move(config);
    assert_true(move_executed);

    move_executed = ccbot_execute_move(config);
    assert_true(move_executed);

    move_executed = ccbot_execute_move(config);
    assert_true(move_executed);

    config_dtor(config);
    free(config);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_ccbot_move),
            cmocka_unit_test(test_ccbot_move_from_standard_starting_position),
            cmocka_unit_test(test_ccbot_multiple_moves)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
