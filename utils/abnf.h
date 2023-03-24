#include "global.h"

typedef struct _abnf_rule
{
    char* name;
    char* value;
    struct _abnf_rule* next;
} abnf_rule;
