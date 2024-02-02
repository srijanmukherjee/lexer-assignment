#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdbool.h>
#include <stddef.h>

#define ST_INITIAL_CAPACITY 8

typedef struct {
    const char **entries;
    size_t capacity;
    size_t n;
} ST;

ST *st_create();
size_t st_insert(ST *st, const char *value);
const char *st_get(ST *st, size_t id);

#endif