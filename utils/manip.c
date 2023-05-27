#include "manip.h"

void debug_http(char* string, int line) {
	printf(RED);
	printf("> src/parser.c:%-4d |%-20s|", line, string);
	printf(RESET "\n");
}

char *copy_sub_str(const unsigned char *source, uint16_t start, uint16_t length) {
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
int hex_to_int(char c) {
	if (isdigit(c)) {
		return c - '0';
	} else {
		return toupper(c) - 'A' + 10;
	}
}

char* url_decode(const char* src) {
	const char* p = src;
	char* dest = NULL;
	char* tmp = NULL;
	size_t size = 0;
	while (*p) {
		if (*p == '%') {
			int value = hex_to_int(*(p + 1)) * 16 + hex_to_int(*(p + 2));
			if (NULL == (tmp = realloc(dest, size + 2))) {
				fprintf(stderr, "realloc problem\n");
                exit(3);
			}
			dest = tmp;
			dest[size] = (char)value;
			size++;
			p += 3;
		} else {
			if (NULL == (tmp = realloc(dest, size + 2))) {
				fprintf(stderr, "realloc problem\n");
                exit(3);
			}
			dest = tmp;
			dest[size] = *p;
			size++;
			p++;
		}
	}
	if (dest) {
		dest[size] = '\0';
	}
	return dest;
}

char *remove_dot_segments(const char *input) {
    char *output = malloc(strlen(input) + 1);
    char *out_ptr = output;
    const char *in_ptr = input;

    while (*in_ptr) {
        if (strncmp(in_ptr, "../", 3) == 0 || strncmp(in_ptr, "./", 2) == 0) {
            in_ptr += (*in_ptr == '.' ? 2 : 3);
        } else if (strncmp(in_ptr, "/./", 3) == 0 || strcmp(in_ptr, "/.") == 0) {
            in_ptr += 2;
        } else if (strncmp(in_ptr, "/../", 4) == 0 || strcmp(in_ptr, "/..") == 0) {
            in_ptr += 3;
            if (out_ptr > output) {
                out_ptr--;
                while (out_ptr > output && *out_ptr != '/') {
                    out_ptr--;
                }
            }
        } else if (strcmp(in_ptr, ".") == 0 || strcmp(in_ptr, "..") == 0) {
            break;
        } else {
            const char *next_slash = strchr(in_ptr + 1, '/');
            size_t segment_len = next_slash ? (size_t)(next_slash - in_ptr) : strlen(in_ptr);
            memcpy(out_ptr, in_ptr, segment_len);
            out_ptr += segment_len;
            in_ptr += segment_len;
        }
    }

    *out_ptr = '\0';
    return output;
}
bool isin(char* str, char* list[]){
	for(int i=0; i<sizeof(list)/sizeof(list[0]); i++){
		if(strcmp(str, list[i])==0){
			return true;
		}
	}
	return false;
}