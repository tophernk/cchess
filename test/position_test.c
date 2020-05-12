#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "position.h"

static void test_position_equal() {
    assert_false(position_equal("a1", "a2"));
    assert_false(position_equal("a1", "b1"));
    assert_true(position_equal("a1", "a1"));
}

static void test_position_valid() {
    assert_false(position_valid("--"));
    assert_false(position_valid("a-"));
    assert_false(position_valid("-1"));
    assert_true(position_valid("a1"));
}

static void test_position_invalidate() {
    char position[2];
    position[0] = 'a';
    position[1] = '1';
    assert_true(position_valid(position));
    position_invalidate(position);
    assert_false(position_valid(position));
}

static void test_position_copy() {
    char a[2];
    a[0] = 'a';
    a[1] = '1';

    char b[2];
    b[0] = 'b';
    b[1] = '1';

    assert_false(position_equal(a, b));

    position_copy(a, b);

    assert_true(position_equal(a, b));
}

static void test_position_get_x_from_file() {
    assert_int_equal(position_to_x('a'), 0);
    assert_int_equal(position_to_x('h'), 7);
}

static void test_position_get_y_from_rank() {
    assert_int_equal(position_to_y('1'), 7);
    assert_int_equal(position_to_y('8'), 0);
}

static void test_position_get_file_from_x() {
    assert_int_equal(position_to_file(0), 'a');
    assert_int_equal(position_to_file(7), 'h');
}

static void test_position_get_rank_from_y() {
    assert_int_equal(position_to_rank(7), '1');
    assert_int_equal(position_to_rank(0), '8');
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_position_valid),
            cmocka_unit_test(test_position_invalidate),
            cmocka_unit_test(test_position_equal),
            cmocka_unit_test(test_position_get_x_from_file),
            cmocka_unit_test(test_position_get_y_from_rank),
            cmocka_unit_test(test_position_get_file_from_x),
            cmocka_unit_test(test_position_get_rank_from_y),
            cmocka_unit_test(test_position_copy)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
