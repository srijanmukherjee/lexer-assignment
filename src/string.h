#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>
#include <stdint.h>

#define INITIAL_STRING_CAPACITY 128

typedef struct {
    char *buf;
    size_t n;
    size_t capacity;
} string_t;

typedef string_t *String;

String new_string();
int string_append_char(String str, char c);
void free_string(String str);
char *string_c_str(String str);

#endif