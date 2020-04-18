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

static void test_config_valid_move_pawn(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    config_add_piece(config, PAWN_W, 0, 6, WHITE, 0);
    config_update_available_positions(config);

    position_t *position = position_new();
    position_set_x(position, 0);
    position_set_y(position, 6);

    piece_t *piece = config_get_piece(config, WHITE, position);
    assert_true(config_valid_move(config, piece, 0, 5));
    assert_true(config_valid_move(config, piece, 0, 4));

    config_add_piece(config, PAWN_B, 1, 5, BLACK, 0);
    config_update_available_positions(config);

    assert_true(config_valid_move(config, piece, 1, 5));
}

static void test_config_valid_move_en_passant(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    config_add_piece(config, PAWN_W, 0, 6, WHITE, 0);
    config_add_piece(config, PAWN_B, 1, 4, BLACK, 0);
    config_update_available_positions(config);

    position_t *from_position = position_new();
    position_set_x(from_position, 0);
    position_set_y(from_position, 6);

    position_t *to_position = position_new();
    position_set_x(to_position, 0);
    position_set_y(to_position, 4);

    path_node_t *move = path_node_new();
    path_node_ctor(move);
    path_node_set_from_position(move, from_position);
    path_node_set_to_position(move, to_position);
    path_node_set_piece_type(move, PAWN_W);

    config_execute_move(config, move);

    position_t *black_position = position_new();
    position_set_x(black_position, 1);
    position_set_y(black_position, 4);

    piece_t *piece = config_get_piece(config, BLACK, black_position);
    assert_true(config_valid_move(config, piece, 0, 5));

    position_t *en_passant_position = position_new();
    position_set_x(en_passant_position, 0);
    position_set_y(en_passant_position, 5);

    path_node_set_from_position(move, black_position);
    path_node_set_to_position(move, en_passant_position);
    path_node_set_piece_type(move, PAWN_B);
    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, to_position);
    assert_null(piece);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_config_add_piece),
            cmocka_unit_test(test_config_eval),
            cmocka_unit_test(test_config_valid_move_pawn),
            cmocka_unit_test(test_config_valid_move_en_passant)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
