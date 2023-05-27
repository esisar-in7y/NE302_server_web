#define max(x,y) (((x) >= (y)) ? (x) : (y))
#include "mime.h"
#define CHUNK_SIZE 50
#define MAX_SIZE 10240
#include <stdio.h>
#include <magic.h>
// const char *get_mime_type(const char* filepath) {
//     const char* mime_type = NULL;
//     magic_t magic_cookie;

//     magic_cookie = magic_open(MAGIC_MIME_TYPE);
//     if (magic_cookie == NULL) {
//         printf("Unable to initialize magic library\n");
//         return mime_type;
//     }

//     if (magic_load(magic_cookie, NULL) != 0) {
//         printf("Cannot load magic database - %s\n", magic_error(magic_cookie));
//         magic_close(magic_cookie);
//         return mime_type;
//     }

//     mime_type = magic_file(magic_cookie, filepath);
//     if (mime_type == NULL) {
//         printf("Failed to get mime type - %s\n", magic_error(magic_cookie));
//     }

//     magic_close(magic_cookie);
//     return mime_type;
// }

int get_file_size(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filepath);
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fclose(file);
    return size;
}

const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}
bool is_text_plain_chunk(const unsigned char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (!isprint(data[i]) && !isspace(data[i])) {
            return false;
        }
    }
    return true;
}
bool is_text_plain(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filepath);
        return "unknown";
    }

    unsigned char buffer[CHUNK_SIZE];
    size_t total_read_size = 0;
    bool is_text = true;
    size_t read_size = 0;

    while (total_read_size < MAX_SIZE) {
        read_size = fread(buffer, 1, CHUNK_SIZE, file);
        if (read_size != 0) {
            if (!is_text_plain_chunk(buffer, read_size)) {
                is_text = false;
                break;
            }
        }else{
            read_size = CHUNK_SIZE;
        }
        total_read_size += read_size;
    }

    fclose(file);
    return is_text;
}

const char *get_mime_type(const char *filepath) {
    const char *extension = get_file_extension(filepath);
    size_t num_mime_types = sizeof(MIME) / sizeof(MIME[0]);

    for (size_t i = 0; i < num_mime_types; i++) {
        if (strcasecmp(extension, MIME[i][0] + 1) == 0) {
            return MIME[i][1];
        }
    }
    if (is_text_plain(filepath)) {
        return "text/plain";
    }
    return "application/octet-stream";
}