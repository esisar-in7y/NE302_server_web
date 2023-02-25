#include "parser.h"
int validateRequest(char *req, int len)
{
    int res;
    tree = (Node *)malloc(sizeof(Node));
    initNode(tree, "HTTP-message");
    res = validateHttpMessage(&req, tree);
    setValues(tree);
    if (!res)
        deleteChildren(tree);

    return res;
}
