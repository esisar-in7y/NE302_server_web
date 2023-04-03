#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "api.h"

#define false 0
#if TST==0
int main(int argc, char *argv[])
{
    int res, fi;
    char *p = NULL, *addr;

    struct stat st;

    if (argc < 3)
    {
        printf("Usage: httpparser <file> <search>\nAttention <search> is case sensitive\n");
        exit(1);
    }
    /* ouverture du fichier contenant la requête */
    if ((fi = open(argv[1], O_RDWR)) == -1)
    {
        perror("open");
        return false;
    }
    if (fstat(fi, &st) == -1) /* To obtain file size */
        return false;
    if ((addr = mmap(NULL, st.st_size, PROT_WRITE, MAP_PRIVATE, fi, 0)) == NULL)
        return false;

    if (argc == 3)
    {
        p = argv[2];
        printf("searching for %s\n", p);
        while (*p)
        {
            if (*p == '-')
            {
                *p = '_';
            }
            p++;
        }
        p = argv[2];
    }

    if ((res = parseur(addr, st.st_size)))
    {
        _Token *r, *tok;
        void *root = NULL;
        root = getRootTree();
        printf("root=%p\n", root);
        r = searchTree(root, p);
        tok = r;
        while (tok)
        {
            int l;
            char *s;
            s = getElementValue(tok->node, &l);
            printf("FOUND [%.*s]\n", l, s);
            tok = tok->next;
            free(s);
        }
        purgeElement(&r);
        purgeTree(root);
    }
    close(fi);
    // free(addr);
    // return 1;
    return (res);
}
#endif