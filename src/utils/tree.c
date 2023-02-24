#include "tree.h"
#include "api.h"
#define getName(var) #var
tree_node* rootTree=NULL;
int get_type(char *string)
{
    for (unsigned long i = 0; i < sizeof(tree_node_string) / sizeof(char *); i++)
    {
        if (strcmp(string, tree_node_string[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

tree_node *tree_node_main_new(char *string)
{
    return tree_node_new(string, 0, strlen(string), NULL, HTTP_message);
}

tree_node *tree_node_new(char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, tree_node_type type)
{
    tree_node *node = (tree_node *)malloc(sizeof(tree_node));
    node->string = string;
    node->start_string = start_string;
    node->length_string = length_string;
    node->parent = parent;
    node->childs = NULL;
    node->childs_count = 0;
    node->type = type;
    if(parent==NULL)
    {
        rootTree=node;
    }
    return node;
}

tree_node *tree_node_add_child(tree_node *node, char *string, uint16_t start_string, uint16_t length_string, tree_node_type type)
{
    node->childs = realloc(node->childs, sizeof(tree_node) * (node->childs_count + 1)); //(tree_node *)
    node->childs[node->childs_count] = tree_node_new(string, start_string, length_string, node, type);
    node->childs_count++;
    // printf("ajoutch:%s<=%s childs:%d\n",tree_node_string[node->type],tree_node_string[type],node->childs_count);
    return node->childs[node->childs_count - 1];
}

tree_node *tree_node_find_type(tree_node *node, int _type,_Token **r)
{
    if ((int)node->type == _type)
    {
        printf("find:%s\n", tree_node_string[node->type]);
        // tree_node_print(node,0);
        return node;
    }
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        tree_node *result = tree_node_find_type(node->childs[i], _type,r);
        if (result != NULL)
        {
            add_token(r,result);//ajouter a liste chainee
        }
    }
    return NULL;
}

tree_node *tree_node_get_child_by_index(tree_node *node, uint16_t index)
{
    if (index < node->childs_count)
    {
        return node->childs[index];
    }
    return NULL;
}

tree_node *tree_node_get_child_by_string(tree_node *node, char *string)
{
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        if (strcmp(node->childs[i]->string, string) == 0)
        {
            return node->childs[i];
        }
    }
    return NULL;
}

void tree_node_free(tree_node *node)
{
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        tree_node_free(node->childs[i]);
    }
    // printf("free:%s\n",tree_node_string[node->type]);
    // free(node->childs);
    free(node);
}

void print_sub_str(char *string, int start_index, int end_index)
{
    for (int i = start_index; i < end_index && string[i] != '\0'; i++)
    {
        printf("%c", string[i]);
    }
}

void tree_node_print(tree_node *node, uint16_t level)
{
    if (node == NULL)
    {
        printf("NULL\n");
        return;
    }
    for (uint16_t i = 0; i < level; i++)
    {
        printf(" ");
    }
    printf("%s (%u, %u) %d childs %p parent %p \n", tree_node_string[node->type], node->start_string, node->length_string, node->childs_count, node, node->parent);

    printf("[%d:%s] = \"", level, tree_node_string[node->type]);
    print_sub_str(node->string, node->start_string, node->length_string);
    printf("\"\n");
}

// print all nodes in tree with format:
// [depth:name] = string
void tree_node_print_all(tree_node *node, uint16_t level)
{
    // printf("[%d:%s] = \"%s\"\n", level, node->string, node->string);
    tree_node_print(node, level);
    if (node == NULL)
    {
        return;
    }
    // printf("childs:%d (%s)\n",node->childs_count,tree_node_string[node->type]);
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        // printf("%d=>%s\n",level,tree_node_string[node->childs[i]->type]);
        tree_node_print_all(node->childs[i], level + 1);
    }
}
char *copy_string(const char *source,uint16_t start,uint16_t length)
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

// Path: src/utils/api.c
#include "api.h"
void *getRootTree()
{
    return rootTree;
}
