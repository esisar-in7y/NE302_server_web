#include "manip.h"
char *copy_sub_str(const char *source,uint16_t start,uint16_t length)
{
    char *buffer = malloc(sizeof(char));
    if (buffer == NULL)
    {
        return NULL;
    }
    for (size_t i = start;source[i] != '\0' && i < start+length;i++)
    {
        buffer = realloc(buffer, (i-start+1)*sizeof(char));
        if (buffer == NULL)
        {
            free(buffer);
            return NULL;
        }
        buffer[i-start] = source[i];
        buffer[i-start+1] = '\0';
    }
    return buffer;
}


void print_sub_str(char *string, int start_index, int length)
{
    for (int i = start_index; i < start_index+length && string[i] != '\0'; i++)
    {
        printf("%c", string[i]);
    }
}
