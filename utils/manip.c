#include "manip.h"
char *copy_sub_str(const char *source, uint16_t start, uint16_t length) {
    char *buffer = malloc(sizeof(char) * (length + 1));
    if (buffer == NULL) {
        return NULL;
    }
    for (size_t i = 0; source[i + start] != '\0' && i < length; i++) {
        buffer[i] = source[i + start];
        buffer[i + 1] = '\0';
    }
    buffer = realloc(buffer, (length + 1) * sizeof(char));
    return buffer;
}

void print_sub_str( unsigned char *string, int start_index, int length) {
    for (int i = start_index; i < start_index + length && string[i] != '\0'; i++) {
        switch (string[i])
        {
        case '\r':
            printf("\\R");
            break;
        case '\n':
            printf("\\N");
            break;
        case '\t':
            printf("\\T");
            break;
        case ' ':
            printf("\\S");
            break;
        default:
            printf("%c", string[i]);
            break;
        }
    }
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
    unsigned char u1, u2;
    for (; n != 0; --n) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (tolower(u1) != tolower(u2)) {
            return tolower(u1) - tolower(u2);
        }
        if (u1 == '\0' || u2 == '\0') {
            return 0;
        }
    }
    return 0;
}

char *trim_space(char *str) {
    char *end;
    while (isspace(*str)) {
        str++;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }
    *(end + 1) = '\0';
    return str;
}