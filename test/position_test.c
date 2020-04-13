#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../position.h"

static void test_position_ctor(void **state) {
    position_t *position = position_new();
    position_ctor(position);

    assert_int_equal(position_get_x(position), -1);
    assert_int_equal(position_get_y(position), -1);

    position_dtor(position);
    free(position);
}

static void test_position_set_y(void **state) {
    position_t *position = position_new();

    int y = 1;
    position_set_y(position, y);

    assert_int_equal(position_get_y(position), y);

    position_dtor(position);
    free(position);
}

static void test_position_set_x(void **state) {
    position_t *position = position_new();

    int x = 1;
    position_set_x(position, x);

    assert_int_equal(position_get_x(position), x);

    position_dtor(position);
    free(position);
}

static void test_position_equal(void **state) {
    int x = 1;
    int y = 1;

    position_t *position_a = position_new();
    position_set_x(position_a, x);
    position_set_y(position_a, y);

    position_t *position_b = position_new();
    position_set_x(position_b, x);
    position_set_y(position_b, y);

    assert_true(position_equal(position_a, position_b));

    position_dtor(position_a);
    free(position_a);
    position_dtor(position_b);
    free(position_b);
}

static void test_position_valid(void **state) {
    position_t *position = position_new();
    position_ctor(position);
    assert_false(position_valid(position));

    position_set_x(position, 1);
    assert_false(position_valid(position));

    position_set_y(position, 1);
    assert_true(position_valid(position));

    position_set_y(position, -1);
    assert_false(position_valid(position));

    position_dtor(position);
    free(position);
}

static void test_position_invalidate(void **state) {
    position_t *position = position_new();
    position_set_x(position, 1);
    position_set_y(position, 1);
    assert_true(position_valid(position));
    position_invalidate(position);
    assert_false(position_valid(position));

    position_dtor(position);
    free(position);
}

static void test_position_copy(void **state) {
    position_t *position = position_new();
    position_set_x(position, 1);
    position_set_y(position, 1);

    position_t *copy = position_new();
    position_ctor(copy);

    assert_false(position_equal(position, copy));

    position_copy(position, copy);

    assert_true(position_equal(position, copy));

    position_dtor(position);
    free(position);
    position_dtor(copy);
    free(copy);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_position_ctor),
            cmocka_unit_test(test_position_set_x),
            cmocka_unit_test(test_position_set_y),
            cmocka_unit_test(test_position_valid),
            cmocka_unit_test(test_position_invalidate),
            cmocka_unit_test(test_position_equal),
            cmocka_unit_test(test_position_copy)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
