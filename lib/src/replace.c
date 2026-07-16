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

typedef struct {
    FILE*                out;        // выходной файл
    const unsigned char* pattern;    // искомая последовательность
    int                  pat_len;    // её длина
    const unsigned char* repl;       // замена
    int                  repl_len;   // длина замены
    int                  state;      // сколько байт совпало
} ReplaceCtx;

// Обрабатывает один байт: продвигает автомат, сбрасывает хвост, пишет замену.
static int process_byte(ReplaceCtx* ctx, unsigned char byte) {
    while (ctx->state > 0 && byte != ctx->pattern[ctx->state]) {
        if (fwrite(&ctx->pattern[0], 1, 1, ctx->out) != 1) return -1;

        int shift = 1;
        int found_partial = 0;
        while (shift < ctx->state) {
            int match_ok = 1;
            for (int j = 0; j < ctx->state - shift; j++) {
                if (ctx->pattern[shift + j] != ctx->pattern[j]) {
                    match_ok = 0;
                    break;
                }
            }
            if (match_ok && byte == ctx->pattern[ctx->state - shift]) {
                for (int k = 1; k < shift; k++) {
                    if (fwrite(&ctx->pattern[k], 1, 1, ctx->out) != 1) return -1;
                }
                ctx->state = ctx->state - shift;
                found_partial = 1;
                break;
            }
            shift++;
        }

        if (found_partial) {
            break;
        } else {
            for (int k = 1; k < ctx->state; k++) {
                if (fwrite(&ctx->pattern[k], 1, 1, ctx->out) != 1) return -1;
            }
            ctx->state = 0;
        }
    }

    if (byte == ctx->pattern[ctx->state]) {
        ctx->state++;
        if (ctx->state == ctx->pat_len) {
            if (ctx->repl != NULL && ctx->repl_len > 0) {
                if (fwrite(ctx->repl, 1, (size_t)ctx->repl_len, ctx->out)
                        != (size_t)ctx->repl_len) return -1;
            }
            ctx->state = 0;
        }
    } else {
        if (fwrite(&byte, 1, 1, ctx->out) != 1) return -1;
    }

    return 0;
}

// Основная функция: читает блоками BLOCK_SIZE и выполняет замену.
int replace_in_files(FILE* f_in, FILE* f_out,
    const unsigned char* search, int search_len,
    const unsigned char* replace, int replace_len) {

    if (f_in == NULL || f_out == NULL || search == NULL || search_len <= 0)
        return -1;

    unsigned char buffer[BLOCK_SIZE];

    ReplaceCtx ctx;
    ctx.out      = f_out;
    ctx.pattern  = search;
    ctx.pat_len  = search_len;
    ctx.repl     = replace;
    ctx.repl_len = replace_len;
    ctx.state    = 0;

    int read_count;
    while ((read_count = (int)fread(buffer, 1, BLOCK_SIZE, f_in)) > 0) {
        for (int idx = 0; idx < read_count; idx++) {
            if (process_byte(&ctx, buffer[idx]) != 0)
                return -1;
        }
    }

    if (ctx.state > 0) {
        if (fwrite(ctx.pattern, 1, (size_t)ctx.state, f_out)
                != (size_t)ctx.state) return -1;
    }

    return 0;
}