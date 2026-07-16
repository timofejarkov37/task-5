/*
 * Реализация библиотеки замены байтовых последовательностей.
 *
 * Ярков Тимофей Сергеевич
 * МК-101
 */

#include "replace.h"
#include <string.h>


// Парсит escape-последовательности \0 и \\ в строке input в байтовый массив.
int decode_sequence(const char* input, unsigned char* output, int max_len) {
    int out_len = 0;
    int i = 0;

    while (input[i] != '\0') {
        if (output != NULL && out_len >= max_len)
            return -1;

        if (input[i] == '\\') {
            if (input[i + 1] == '\0') {
                return -1;
            } else if (input[i + 1] == '0') {
                if (output) output[out_len] = '\0';
                out_len++;
                i += 2;
            } else if (input[i + 1] == '\\') {
                if (output) output[out_len] = '\\';
                out_len++;
                i += 2;
            } else {
                return -1;
            }
        } else {
            if (output) output[out_len] = (unsigned char)input[i];
            out_len++;
            i++;
        }
    }

    return out_len;
}