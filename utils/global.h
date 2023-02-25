#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#if defined(_WIN32)
    #include <stdint.h>
    typedef uint8_t u_int8_t;
    typedef uint16_t u_int16_t;
#endif
#include "manip.h"
