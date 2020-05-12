#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "logger.h"

static void test_logger() {
    cchess_log_file("test %d", 123);

    FILE *file = fopen(CCHESS_LOG, "rb");
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    rewind(file);
    char *result;
    result = (char *) malloc(sizeof(char) * f_size);
    fread(result, sizeof(char), f_size, file);

    assert_memory_equal(result, "test 123", f_size);

    fclose(file);
    remove(CCHESS_LOG);
    free(result);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_logger)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
