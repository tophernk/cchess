#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"

static void test_config_eval(void **state) {
    config_t *config = config_new();
    config_ctor(config);


}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_config_eval)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
