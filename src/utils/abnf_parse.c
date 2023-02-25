#include "abnf_parse.h"

char *gettoken(char *ptr, int *tokentype, int hexmode)
{
    *tokentype = ERROR;

    switch (*ptr++)
    {
    case '\0':
        return (NULL);

    case '\r':
        if (*ptr == '\n')
            ++ptr;
    case '\n':
        *tokentype = NEWLINE;
        break;

    case ';':
        *tokentype = COMMENT;
        while (*ptr != '\0' && *ptr != '\n' && *ptr != '\r')
            ++ptr;
        break;

    case '/':
        *tokentype = ALTERNATE;
        break;

    case '*':
        *tokentype = REPEAT;
        break;

    case '(':
        *tokentype = BEGINGROUP;
        break;

    case ')':
        *tokentype = ENDGROUP;
        break;

    case '[':
        *tokentype = BEGINOPTION;
        break;

    case ']':
        *tokentype = ENDOPTION;
        break;

    case '-':
        *tokentype = RANGE;
        break;

    case '.':
        *tokentype = LITLIST;
        break;

    case '=':
        *tokentype = RULEDEF;
        if (*ptr == '/')
        {
            *tokentype = AMENDRULE;
            ++ptr;
        }
        break;

    case '%':
        switch (*ptr++)
        {
        case 'x':
        case 'X':
            *tokentype = HEXMODE;
            break;
        case 'b':
        case 'B':
            *tokentype = BINMODE;
            break;
        case 'd':
        case 'D':
            *tokentype = DECMODE;
            break;
        default:
            return ("Invalid literal");
        }
        break;

    case '"':
        while (*ptr && *ptr != '"')
            ++ptr;
        if (*ptr != '"')
        {
            return ("Unbalanced double quotes");
        }
        ++ptr;
        *tokentype = QUOTED;
        break;

    case '<':
        while (*ptr && *ptr != '>')
            ++ptr;
        if (*ptr != '>')
        {
            return ("Unbalanced angle brakets");
        }
        ++ptr;
        *tokentype = PROSE;
        break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        if (hexmode)
            goto GETHEX;
        while (isdigit(*ptr))
            ++ptr;
        *tokentype = NUMBER;
        break;

    default:
        /* deal with hex digits */
        if (hexmode && isxdigit(ptr[-1]))
        {
        GETHEX:
            while (isxdigit(*ptr))
                ++ptr;
            *tokentype = HEXNUMBER;
            break;
        }

        if (!isalpha(ptr[-1]))
        {
            return ("Invalid character");
        }

        /* deal with rule names */
        while (isalnum(*ptr) || *ptr == '-')
            ++ptr;
        *tokentype = RULENAME;
        break;
    }

    return (ptr);
}
struct rulename *hashfind(const char *ptr, int len) {
    int i, hash;
    struct rulename **pptr;

    /* lookup in hash table */
    hash = len;
    for (i = 0; i < len; ++i) {
        hash = (hash * 13 + TOLOWER(ptr[i])) % HTSIZE;
    }

    pptr = &htable[hash];
    while (*pptr && ((*pptr)->len != len || strncasecmp(ptr, (*pptr)->data, len) != 0)) {
        pptr = &(*pptr)->next;
    }

    /* create new entry, if needed */
    if (!*pptr) {
        *pptr = calloc(sizeof(struct rulename) + len, 1);

        if (!*pptr) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }

        (*pptr)->len = len;
        memcpy((*pptr)->data, ptr, len);
    }

    return (*pptr);
}