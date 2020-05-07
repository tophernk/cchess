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
    assert_memory_equal(piece_get_current_position(piece), "a2", 2);

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
    char *fen = "8/8/8/8/1p6/2P5/P7/8 w - - 0 0";
    config_fen_in(config, fen);

    piece_t *piece = config_get_piece(config, WHITE, "a2");
    assert_memory_equal(piece_get_available_position(piece, 0), "a4a3--------------------------------------------------", 54);
    piece = config_get_piece(config, BLACK, "b4");
    assert_memory_equal(piece_get_available_position(piece, 0), "b3c3--------------------------------------------------", 54);

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, "a2");
    move_set_to_position(move, "a4");
    move_set_piece_type(move, PAWN_W);

    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "a4");
    assert_memory_equal(piece_get_available_position(piece, 0), "a5----------------------------------------------------", 54);
    piece = config_get_piece(config, BLACK, "b4");
    assert_memory_equal(piece_get_available_position(piece, 0), "a3b3c3------------------------------------------------", 54);

    move_set_from_position(move, "b4");
    move_set_to_position(move, "a3");
    move_set_piece_type(move, PAWN_B);
    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "a4");
    assert_null(piece);
    piece = config_get_piece(config, BLACK, "a3");
    assert_memory_equal(piece_get_available_position(piece, 0), "a2----------------------------------------------------", 54);

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

    piece_t *piece = config_get_piece(config, WHITE, "e1");
    assert_true(config_valid_move(config, piece, 6, 7));

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, "e1");
    move_set_to_position(move, "g1");
    move_set_piece_type(move, KING_W);

    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "f1");
    assert_non_null(piece);
    assert_true(piece_get_type(piece) == ROOK_W);

    config_dtor(config);
    free(move);
    free(config);
}

static void test_config_long_castle(void **state) {
    config_t *config = config_new();
    char *fen = "8/1p6/8/8/8/8/8/R3K3 w Q - 0 0";
    config_fen_in(config, fen);

    piece_t *piece = config_get_piece(config, WHITE, "e1");
    assert_true(config_valid_move(config, piece, 2, 7));

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, "e1");
    move_set_to_position(move, "c1");
    move_set_piece_type(move, KING_W);

    config_execute_move(config, move);

    piece = config_get_piece(config, WHITE, "d1");
    assert_non_null(piece);
    assert_true(piece_get_type(piece) == ROOK_W);

    config_dtor(config);
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

    move_t *move = move_new();
    move_ctor(move);
    move_set_from_position(move, "a3");
    move_set_to_position(move, "a4");
    move_set_piece_type(move, PAWN_W);

    piece_moved = config_execute_move(config, move);
    assert_true(MOVE_EXECUTED(piece_moved));

    config_t *copy = config_new();
    config_ctor(copy);
    config_copy(config, copy);

    piece_moved = config_move_cpu(config);
    assert_true(piece_moved);
    assert_memory_not_equal(config->board, copy->board, 2 * BOARD_SIZE * sizeof(piece_type_t));

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
