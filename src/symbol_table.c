#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>

ST *st_create() {
    ST *st = malloc(sizeof(ST));
    st->entries = malloc(sizeof(char *) * ST_INITIAL_CAPACITY);
    st->capacity = ST_INITIAL_CAPACITY;
    st->n = 0;
    return st;
}

size_t st_insert(ST *st, const char *value) {
    for (int i = 0; i < st->n; i++) {
        if (strcmp(st->entries[i], value) == 0) {
            return i;
        }
    }

    if (st->n == st->capacity) {
        st->capacity += 8;
        st->entries = realloc(st->entries, sizeof(char *) * st->capacity);
    }

    st->entries[st->n++] = value;
    return st->n - 1;
}
const char *st_get(ST *st, size_t id) {
    if (id < 0 || id >= st->n) return NULL;
    return st->entries[id];
}