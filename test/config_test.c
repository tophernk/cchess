#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"
#include "config_p.h"

static void test_config_add_piece(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    piece_t *piece = NULL;

    piece = config_get_piece(config, WHITE, "a2");
    assert_null(piece);

    config_add_piece(config, PAWN_W, 0, 6, WHITE, 0);

    piece = config_get_piece(config, WHITE, "a2");
    assert_non_null(piece);
    assert_int_equal(piece_get_type(piece), PAWN_W);
    assert_int_equal(position_get_x(piece_get_current_position(piece)), 0);
    assert_int_equal(position_get_y(piece_get_current_position(piece)), 6);

    config_dtor(config);
    free(config);
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

    config_dtor(config);
    free(config);
}

static void test_config_valid_move_pawn(void **state) {
    config_t *config = config_new();
    config_ctor(config);

    config_add_piece(config, PAWN_W, 0, 6, WHITE, 0);
    config_update_available_positions(config);

    piece_t *piece = config_get_piece(config, WHITE, "a2");
    assert_true(config_valid_move(config, piece, 0, 5));
    assert_true(config_valid_move(config, piece, 0, 4));

    config_add_piece(config, PAWN_B, 1, 5, BLACK, 0);
    config_update_available_positions(config);

    assert_true(config_valid_move(config, piece, 1, 5));

    config_dtor(config);
    free(config);
}

static void test_config_valid_move_en_passant(void **state) {
    config_t *config = config_new();
    char *fen = "8/8/8/8/1p6/8/P7/8 w - - 0 0";
    config_fen_in(config, fen);

    position_t *from_position = position_new();
    position_set_x(from_position, 0);
    position_set_y(from_position, 6);

    position_t *to_position = position_new();
    position_set_x(to_position, 0);
    position_set_y(to_position, 4);

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, from_position);
    move_set_to_position(move, to_position);
    move_set_piece_type(move, PAWN_W);

    config_execute_move(config, move);

    piece_t *piece = config_get_piece(config, BLACK, "b4");
    assert_true(config_valid_move(config, piece, 0, 5));

    position_t *en_passant_position = position_new();
    position_set_x(en_passant_position, 0);
    position_set_y(en_passant_position, 5);

    position_t *black_position = position_new();
    position_set_x(black_position, 1);
    position_set_y(black_position, 4);

    move_set_from_position(move, black_position);
    move_set_to_position(move, en_passant_position);
    move_set_piece_type(move, PAWN_B);
    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "a4");
    assert_null(piece);

    position_dtor(from_position);
    position_dtor(to_position);
    position_dtor(black_position);
    position_dtor(en_passant_position);
    free(from_position);
    free(to_position);
    free(black_position);
    free(en_passant_position);

    move_dtor(move);
    free(move);

    config_dtor(config);
    free(config);
}

void test_config_cpu_move(void **state) {
    config_t *config = config_new();
    char *fen = "1n2k3/p7/8/8/8/8/1P6/2B1KB2 w K - 0 0";
    config_fen_in(config, fen);

    int piece_moved = config_move_cpu(config);
    assert_true(piece_moved);

    config_dtor(config);
    free(config);
}

static void test_config_copy(void **state) {
    config_t *config = config_new();
    config_ctor(config);
    config_t *copy = config_new();
    config_ctor(copy);

    config_enable_short_castles(config, BLACK);
    config_enable_long_castles(config, BLACK);
    config_enable_short_castles(config, WHITE);
    config_enable_long_castles(config, WHITE);
    config->enpassant[0] = 'a';
    config->enpassant[1] = '3';

    config_copy(config, copy);

    assert_int_equal(config->check_black, copy->check_black);
    assert_int_equal(config->check_white, copy->check_white);
    assert_int_equal(config->short_castles_black, copy->short_castles_black);
    assert_int_equal(config->short_castles_white, copy->short_castles_white);
    assert_int_equal(config->long_castles_black, copy->long_castles_black);
    assert_int_equal(config->long_castles_white, copy->long_castles_white);
    assert_memory_equal(config->board, copy->board, sizeof(BOARD_SIZE * 2 * sizeof(piece_type_t)));
    assert_memory_equal(config->enpassant, copy->enpassant, sizeof(char) * 2);

    config_dtor(copy);
    free(copy);
    config_dtor(config);
    free(config);
}

static void test_config_short_castle(void **state) {
    config_t *config = config_new();
    char *fen = "8/1p6/8/8/8/8/8/4K2R w K - 0 0";
    config_fen_in(config, fen);

    position_t *king_position = position_new();
    position_set_x(king_position, 4);
    position_set_y(king_position, 7);

    position_t *castle_position = position_new();
    position_set_x(castle_position, 6);
    position_set_y(castle_position, 7);

    piece_t *piece = config_get_piece(config, WHITE, "e1");
    assert_true(config_valid_move(config, piece, 6, 7));

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, king_position);
    move_set_to_position(move, castle_position);
    move_set_piece_type(move, KING_W);

    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "f1");
    assert_non_null(piece);
    assert_true(piece_get_type(piece) == ROOK_W);

    position_dtor(king_position);
    position_dtor(castle_position);
    move_dtor(move);
    config_dtor(config);
    free(king_position);
    free(castle_position);
    free(move);
    free(config);
}

static void test_config_long_castle(void **state) {
    config_t *config = config_new();
    char *fen = "8/1p6/8/8/8/8/8/R3K3 w Q - 0 0";
    config_fen_in(config, fen);

    position_t *king_position = position_new();
    position_set_x(king_position, 4);
    position_set_y(king_position, 7);

    position_t *castle_position = position_new();
    position_set_x(castle_position, 2);
    position_set_y(castle_position, 7);

    piece_t *piece = config_get_piece(config, WHITE, "e1");
    assert_true(config_valid_move(config, piece, 2, 7));

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, king_position);
    move_set_to_position(move, castle_position);
    move_set_piece_type(move, KING_W);

    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "d1");
    assert_non_null(piece);
    assert_true(piece_get_type(piece) == ROOK_W);

    position_dtor(king_position);
    position_dtor(castle_position);
    move_dtor(move);
    config_dtor(config);
    free(king_position);
    free(castle_position);
    free(move);
    free(config);
}

void test_config_cpu_move_from_standard_starting_position(void **state) {
    config_t *config = config_new();
    // a3 has been played
    config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 0");

    int piece_moved = config_move_cpu(config);
    assert_true(piece_moved);

    config_dtor(config);
    free(config);
}

void test_config_multiple_cpu_moves(void **state) {
    config_t *config = config_new();
    // a3 has been played
    config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 0");

    int piece_moved = config_move_cpu(config);
    assert_true(piece_moved);

    position_t *from_position = position_new();
    position_set_x(from_position, 0);
    position_set_y(from_position, 5);

    position_t *to_position = position_new();
    position_set_x(to_position, 0);
    position_set_y(to_position, 4);

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, from_position);
    move_set_to_position(move, to_position);
    move_set_piece_type(move, PAWN_W);

    piece_moved = config_execute_move(config, move);
    assert_true(MOVE_EXECUTED(piece_moved));

    config_t *copy = config_new();
    config_ctor(copy);
    config_copy(config, copy);

    piece_moved = config_move_cpu(config);
    assert_true(piece_moved);
    assert_memory_not_equal(config->board, copy->board, 2 * BOARD_SIZE * sizeof(piece_type_t));

    position_dtor(from_position);
    position_dtor(to_position);
    free(from_position);
    free(to_position);
    move_dtor(move);
    free(move);
    config_dtor(copy);
    config_dtor(config);
    free(copy);
    free(config);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_config_add_piece),
            cmocka_unit_test(test_config_eval),
            cmocka_unit_test(test_config_valid_move_pawn),
            cmocka_unit_test(test_config_copy),
            cmocka_unit_test(test_config_valid_move_en_passant),
            cmocka_unit_test(test_config_short_castle),
            cmocka_unit_test(test_config_long_castle),
            cmocka_unit_test(test_config_cpu_move),
            cmocka_unit_test(test_config_cpu_move_from_standard_starting_position),
            cmocka_unit_test(test_config_multiple_cpu_moves)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
