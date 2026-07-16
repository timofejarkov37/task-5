/*
 * Точка входа тестов. Принимает имя теста аргументом.
 *
 * Ярков Тимофей Сергеевич
 * МК-101
 */

#include <stdio.h>
#include <stdlib.h>

int run_test_by_name(const char* name);

// Запускает указанный тест по имени.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tests <test_name>\n");
        return EXIT_FAILURE;
    }

    int result = run_test_by_name(argv[1]);

    if (result == -1) {
        fprintf(stderr, "Unknown test: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (result == 0)
        printf("PASSED [%s]\n", argv[1]);

    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
