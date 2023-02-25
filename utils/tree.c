#include "tree.h"
#define getName(var) #var
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

tree_node *tree_node_init(char *string)
{
    #if ABNF==0
        return tree_node_new(string, 0, strlen(string), NULL, HTTP_message);
    #elif ABNF==1
        return tree_node_new(string, 0, strlen(string), NULL, message);
    #endif
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
    if(parent==NULL && rootTree==NULL)
    {
        rootTree=node;
    }
    return node;
}
void update_length(tree_node* node){
    if(node!=NULL){
        int string_length = 0;
        for (size_t i = 0; i < node->childs_count; i++)
        {
            string_length+=node->childs[i]->length_string;
        }
        node->length_string=string_length;
        // printf("update_length:%s:%d\n",tree_node_string[node->type],node->length_string);
    }
}

tree_node *tree_node_add_child_node(tree_node *parent,tree_node *node)
{
    node->childs = realloc(node->childs, sizeof(tree_node) * (node->childs_count + 1)); //(tree_node *)
    node->childs[node->childs_count] = node;
    node->childs_count++;
    update_length(node->parent);
    // printf("ajoutch:%s<=%s childs:%d\n",tree_node_string[node->type],tree_node_string[type],node->childs_count);
    return node->childs[node->childs_count - 1];
}
tree_node *tree_node_add_child(tree_node *node, char *string, uint16_t start_string, uint16_t length_string, tree_node_type type)
{
    node->childs = realloc(node->childs, sizeof(tree_node) * (node->childs_count + 1)); //(tree_node *)
    node->childs[node->childs_count] = tree_node_new(string, start_string, length_string, node, type);
    node->childs_count++;
    update_length(node);
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
    if(node==NULL) return;
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        tree_node_free(node->childs[i]);
    }
    // printf("free:%s\n",tree_node_string[node->type]);
    // free(node->childs);
    if(node->parent&&node->parent->childs_count>0){
        node->parent->childs_count--;
        node->parent->childs = realloc(node->parent->childs, sizeof(tree_node) * (node->parent->childs_count));
    }
    free(node);
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
    // printf("%s (%u, %u) %d childs %p parent %p \n", tree_node_string[node->type], node->start_string, node->length_string, node->childs_count, node, node->parent);

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
// Path: src/utils/api.c
#include "../src/api.h"
void *getRootTree()
{
    return rootTree;
}
