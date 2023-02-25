#include "manip.h"
char *copy_sub_str(const char *source,uint16_t start,uint16_t length)
{
    char *buffer = malloc(sizeof(char)*(length+1));
    if (buffer == NULL)
    {
        return NULL;
    }
    for (size_t i = 0;source[i+start] != '\0' && i < length;i++)
    {
        buffer[i] = source[i+start];
        buffer[i+1] = '\0';
    }
    buffer = realloc(buffer, (length+1)*sizeof(char));
    return buffer;
}


void print_sub_str(char *string, int start_index, int length)
{
    for (int i = start_index; i < start_index+length && string[i] != '\0'; i++)
    {
        printf("%c", string[i]);
    }
}
