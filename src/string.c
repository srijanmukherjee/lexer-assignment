#include "string.h"

#include <stdlib.h>
#include <string.h>

String new_string() {
    String str = malloc(sizeof(string_t));
    str->buf = malloc(sizeof(char) * (INITIAL_STRING_CAPACITY + 1));
    str->capacity = INITIAL_STRING_CAPACITY;
    str->n = 0;
    return str;
}
int string_append_char(String str, char c) {
    if (str->n == str->capacity) {
        str->capacity += 128;
        str->buf = realloc(str->buf, sizeof(char) * (str->capacity + 1));
        if (str->buf == NULL) return 1;
    }

    str->buf[str->n++] = c;
    str->buf[str->n] = 0;
    return 0;
}
void free_string(String str) {
    if (str == NULL) return;
    free(str->buf);
    free(str);
}
char *string_c_str(String str) {
    if (str == NULL) return NULL;
    return strndup(str->buf, str->n);
}