#ifndef MANIP_H
#define MANIP_H
#include "global.h"
#include "api.h"
char *copy_sub_str(const char *source,uint16_t start,uint16_t length);
void print_sub_str(char *string, int start_index, int end_index);
#endif