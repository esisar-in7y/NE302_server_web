#include "api.h"

void add_token(_Token **head, void *node) {
    _Token *new_token = (_Token*) malloc(sizeof(_Token));
    new_token->node = node;
    new_token->next = NULL;
    
    if (*head == NULL) {
        *head = new_token;
    } else {
        _Token *current_token = *head;
        while (current_token->next != NULL) {
            current_token = current_token->next;
        }
        current_token->next = new_token;
    }
}


_Token *searchTree(void *start, char *name)
{
    if (start == NULL)
    {
        start = getRootTree();
    }
    _Token *r = NULL;
    tree_node_find_type(start, get_type(name),&r);
    return r;
}
char *getElementTag(void *node, int *len)
{
    if (node == NULL)
    {
        return NULL;
    }
    char *str = tree_node_string[((tree_node *)node)->type];
    *len = strlen(str);
    return str;
}
char *getElementValue(void *node, int *len)
{
    if (node == NULL)
    {
        return NULL;
    }
    tree_node* tnode = (tree_node *)node;
    char *str = copy_sub_str(tnode->string, tnode->start_string, tnode->length_string);
    *len = strlen(str);
    return str;
}
void purgeElement(_Token **r){
    _Token *current_token = *r;
    while (current_token != NULL) {
        _Token *tmp = current_token;
        current_token = current_token->next;
        free(tmp);
    }
    *r = NULL;
}
void purgeTree(void *root){
    if(root == NULL){
        return;
    }
    tree_node_free(root);
    root = NULL;
}