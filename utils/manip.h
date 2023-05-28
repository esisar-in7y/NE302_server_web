#ifndef MANIP_H
#define MANIP_H
#include "global.h"
#include "tree.h"
#include "../src/api.h"
void debug_http(char* string, int line);
char *copy_sub_str(const unsigned char *source,uint16_t start,uint16_t length);
void print_sub_str( unsigned char *string, int start_index, int end_index);
int  strncasecmp(const char *, const char *, size_t);
char* trim_space(char *str);
char* url_decode(const char* src);
char *remove_dot_segments(const char *input);
bool isin(char* str, char* list[]);
bool have_separators(char* string, char *value);
char* get_first_value(tree_node* root,char* search);
char* getFieldValueFromFieldName(tree_node* root, char* field_name);
#endif