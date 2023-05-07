#include "mime.h"

const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}
const char *get_file_type(const char *filename) {
    const char *extension = get_file_extension(filename);
    size_t num_mime_types = sizeof(MIME) / sizeof(MIME[0]);

    for (size_t i = 0; i < num_mime_types; i++) {
        if (strcasecmp(extension, MIME[i][0] + 1) == 0) {
            return MIME[i][1];
        }
    }

    return "application/octet-stream";
}