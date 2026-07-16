/*
 * Тесты библиотеки замены байтовых последовательностей.
 *
 * Ярков Тимофей Сергеевич
 * МК-101
 */

#include "replace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Вспомогательная функция: применяет замену к данным, возвращает результат в buf.
static int run_test_case(
    const unsigned char* input,  int input_len,
    const unsigned char* search, int search_len,
    const unsigned char* replace, int replace_len,
    unsigned char* buf, int buf_size)
{
    const char* path_in  = "tmp_input.dat";
    const char* path_out = "tmp_output.dat";

    FILE* f = fopen(path_in, "wb");
    if (!f) return -1;
    if (input_len > 0 && fwrite(input, 1, (size_t)input_len, f) != (size_t)input_len) {
        fclose(f); return -1;
    }
    fclose(f);

    FILE* f_in  = fopen(path_in,  "rb");
    FILE* f_out = fopen(path_out, "wb");
    if (!f_in || !f_out) {
        if (f_in)  fclose(f_in);
        if (f_out) fclose(f_out);
        return -1;
    }

    int ret = replace_in_files(f_in, f_out, search, search_len,
                               replace_len > 0 ? replace : NULL, replace_len);
    fclose(f_in);
    fclose(f_out);
    if (ret != 0) return -1;

    FILE* f_res = fopen(path_out, "rb");
    if (!f_res) return -1;
    int out_len = (int)fread(buf, 1, (size_t)buf_size, f_res);
    fclose(f_res);

    remove(path_in);
    remove(path_out);
    return out_len;
}

#define CHECK(cond, msg) \
    do { if (!(cond)) { printf("FAILED: %s\n", msg); return 1; } } while (0)

// Тест 1: простая замена "bc" -> "12" в строке "abcde".
static int test_simple_replace(void) {
    const unsigned char* in  = (unsigned char*)"abcde";
    const unsigned char* s   = (unsigned char*)"bc";
    const unsigned char* r   = (unsigned char*)"12";
    const unsigned char* exp = (unsigned char*)"a12de";
    unsigned char buf[32];
    int len = run_test_case(in, 5, s, 2, r, 2, buf, sizeof(buf));
    CHECK(len == 5, "length");
    CHECK(memcmp(buf, exp, 5) == 0, "content");
    return 0;
}

// Тест 2: замена в начале строки "ab" -> "XY" в "abcdef".
static int test_replace_at_start(void) {
    const unsigned char* in  = (unsigned char*)"abcdef";
    const unsigned char* s   = (unsigned char*)"ab";
    const unsigned char* r   = (unsigned char*)"XY";
    const unsigned char* exp = (unsigned char*)"XYcdef";
    unsigned char buf[32];
    int len = run_test_case(in, 6, s, 2, r, 2, buf, sizeof(buf));
    CHECK(len == 6, "length");
    CHECK(memcmp(buf, exp, 6) == 0, "content");
    return 0;
}

// Тест 3: замена в конце строки "ef" -> "ZZ" в "abcdef".
static int test_replace_at_end(void) {
    const unsigned char* in  = (unsigned char*)"abcdef";
    const unsigned char* s   = (unsigned char*)"ef";
    const unsigned char* r   = (unsigned char*)"ZZ";
    const unsigned char* exp = (unsigned char*)"abcdZZ";
    unsigned char buf[32];
    int len = run_test_case(in, 6, s, 2, r, 2, buf, sizeof(buf));
    CHECK(len == 6, "length");
    CHECK(memcmp(buf, exp, 6) == 0, "content");
    return 0;
}

// Тест 4: паттерн не найден – "xyz" в "hello world".
static int test_no_match(void) {
    const unsigned char* in  = (unsigned char*)"hello world";
    const unsigned char* s   = (unsigned char*)"xyz";
    const unsigned char* r   = (unsigned char*)"ABC";
    unsigned char buf[32];
    int len = run_test_case(in, 11, s, 3, r, 3, buf, sizeof(buf));
    CHECK(len == 11, "length");
    CHECK(memcmp(buf, in, 11) == 0, "content");
    return 0;
}

// Тест 5: срыв совпадения – "xxxy" с паттерном "xxy" -> "xX".
static int test_fallback(void) {
    const unsigned char* in  = (unsigned char*)"xxxy";
    const unsigned char* s   = (unsigned char*)"xxy";
    const unsigned char* r   = (unsigned char*)"X";
    const unsigned char* exp = (unsigned char*)"xX";
    unsigned char buf[32];
    int len = run_test_case(in, 4, s, 3, r, 1, buf, sizeof(buf));
    CHECK(len == 2, "length");
    CHECK(memcmp(buf, exp, 2) == 0, "content");
    return 0;
}

// Тест 6: самоперекрытие – "cdcdc" с паттерном "cdc" -> "Xdc".
static int test_self_overlap(void) {
    const unsigned char* in  = (unsigned char*)"cdcdc";
    const unsigned char* s   = (unsigned char*)"cdc";
    const unsigned char* r   = (unsigned char*)"X";
    const unsigned char* exp = (unsigned char*)"Xdc";
    unsigned char buf[32];
    int len = run_test_case(in, 5, s, 3, r, 1, buf, sizeof(buf));
    CHECK(len == 3, "length");
    CHECK(memcmp(buf, exp, 3) == 0, "content");
    return 0;
}

// Тест 7: нулевой байт в искомой строке (c\0d) заменить на 'Y'.
static int test_null_in_search(void) {
    unsigned char in[]  = { 0x63, 0x00, 0x64 }; // "c\0d"
    unsigned char s[]   = { 0x63, 0x00, 0x64 };
    unsigned char r[]   = { 'Y' };
    unsigned char exp[] = { 'Y' };
    unsigned char buf[32];
    int len = run_test_case(in, 3, s, 3, r, 1, buf, sizeof(buf));
    CHECK(len == 1, "length");
    CHECK(memcmp(buf, exp, 1) == 0, "content");
    return 0;
}

// Тест 8: нулевой байт в замене – "def" -> "p\0q".
static int test_null_in_replace(void) {
    unsigned char in[]  = { 'd', 'e', 'f' };
    unsigned char s[]   = { 'd', 'e', 'f' };
    unsigned char r[]   = { 0x70, 0x00, 0x71 }; // "p\0q"
    unsigned char exp[] = { 0x70, 0x00, 0x71 };
    unsigned char buf[32];
    int len = run_test_case(in, 3, s, 3, r, 3, buf, sizeof(buf));
    CHECK(len == 3, "length");
    CHECK(memcmp(buf, exp, 3) == 0, "content");
    return 0;
}

// Тест 9: удаление подстроки "123" из "abc123def" -> "abcdef".
static int test_empty_replace(void) {
    const unsigned char* in  = (unsigned char*)"abc123def";
    const unsigned char* s   = (unsigned char*)"123";
    const unsigned char* exp = (unsigned char*)"abcdef";
    unsigned char buf[32];
    int len = run_test_case(in, 9, s, 3, NULL, 0, buf, sizeof(buf));
    CHECK(len == 6, "length");
    CHECK(memcmp(buf, exp, 6) == 0, "content");
    return 0;
}

// Тест 10: пустой входной файл.
static int test_empty_input(void) {
    const unsigned char* in  = (unsigned char*)"";
    const unsigned char* s   = (unsigned char*)"abc";
    const unsigned char* r   = (unsigned char*)"def";
    unsigned char buf[32];
    int len = run_test_case(in, 0, s, 3, r, 3, buf, sizeof(buf));
    CHECK(len == 0, "length");
    return 0;
}

typedef struct { const char* name; int (*fn)(void); } Test;

static const Test tests[] = {
    { "simple_replace",          test_simple_replace          },
    { "replace_at_start",        test_replace_at_start        },
    { "replace_at_end",          test_replace_at_end          },
    { "no_match",                test_no_match                },
    { "fallback",                test_fallback                },
    { "self_overlap",            test_self_overlap            },
    { "null_in_search",          test_null_in_search          },
    { "null_in_replace",         test_null_in_replace         },
    { "empty_replace",           test_empty_replace           },
    { "empty_input",             test_empty_input             },
    { NULL, NULL }
};

// Находит тест по имени и запускает его.
int run_test_by_name(const char* name) {
    for (const Test* t = tests; t->name; t++) {
        if (strcmp(t->name, name) == 0)
            return t->fn();
    }
    return -1;
}