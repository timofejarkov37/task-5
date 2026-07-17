/*
 * Программа замены байтовых последовательностей в файле.
 * Использование: replace <входной_файл> <выходной_файл> <искомая> <замена>
 * Для нулевого байта - \0, для слэша - \\
 *
 * Ярков Тимофей Сергеевич
 * МК-101
 */

#include <stdio.h>
#include <stdlib.h>
#include "replace.h"

// Точка входа: обрабатывает аргументы, вызывает библиотеку.
int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <search> <replace>\n", argv[0]);
        fprintf(stderr, "Use \\0 for null byte, \\\\ for backslash.\n");
        return 1;
    }

    int search_len = decode_sequence(argv[3], NULL, 0);
    if (search_len <= 0) {
        fprintf(stderr, "Error: empty or invalid search sequence.\n");
        return 1;
    }
    unsigned char* search = (unsigned char*)malloc((size_t)search_len);
    if (!search) { fprintf(stderr, "Memory allocation error.\n"); return 1; }
    decode_sequence(argv[3], search, search_len);

    int replace_len = decode_sequence(argv[4], NULL, 0);
    if (replace_len < 0) {
        fprintf(stderr, "Error: invalid replacement sequence.\n");
        free(search);
        return 1;
    }
    unsigned char* replace = NULL;
    if (replace_len > 0) {
        replace = (unsigned char*)malloc((size_t)replace_len);
        if (!replace) { free(search); fprintf(stderr, "Memory allocation error.\n"); return 1; }
        decode_sequence(argv[4], replace, replace_len);
    }

    FILE* f_in = fopen(argv[1], "rb");
    if (!f_in) {
        perror(argv[1]);
        free(search); free(replace);
        return 1;
    }

    FILE* f_out = fopen(argv[2], "wb");
    if (!f_out) {
        perror(argv[2]);
        fclose(f_in); free(search); free(replace);
        return 1;
    }

    int result = replace_in_files(f_in, f_out, search, search_len, replace, replace_len);

    fclose(f_in);
    fclose(f_out);
    free(search);
    free(replace);

    if (result != 0) {
        fprintf(stderr, "Error processing files.\n");
        remove(argv[2]);
        return 1;
    }

    printf("Done.\n");
    return 0;
}
