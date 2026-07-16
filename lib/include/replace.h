/*
 * Заголовочный файл библиотеки замены байтовых последовательностей.
 *
 * Ярков Тимофей Сергеевич
 * МК-101
 */

#pragma once
#include <stdio.h>

int decode_sequence(const char* input, unsigned char* output, int max_len);

int replace_in_files(FILE* f_in, FILE* f_out,
    const unsigned char* search, int search_len,
    const unsigned char* replace, int replace_len);