#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#if defined(_WIN32)
    #include <stdint.h>
    typedef uint8_t u_int8_t;
    typedef uint16_t u_int16_t;
#endif
#ifndef GLOBAL_H
#define GLOBAL_H
typedef struct _tree_node {
    uint16_t start_string;
    uint16_t length_string;
    uint16_t childs_count;
    unsigned char *string;
    // tree_node_type type;
    int type;
    struct _tree_node *parent;
    struct _tree_node **childs;
} tree_node;
#include "tree.h"
#include "manip.h"
#include "structures.h"
#endif

#define RED	  "\033[31m" /* Red */
#define RESET "\033[0m"
