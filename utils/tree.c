#include "tree.h"
#define getName(var) #var
int get_type_index(const char *string)
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
    tree_node *node = NULL;
    #if ABNF==0
        node=tree_node_new(string, 0, 0, NULL, "HTTP_message");
    #elif ABNF==1
        node=tree_node_new(string, 0, 0, NULL, "message");//strlen(string)
    #endif
    rootTree=node;
    return node;
}
tree_node *tree_node_tmp(tree_node *parent){
    return tree_node_new(parent->string, get_start(parent), 0, NULL, "tmp");
}
tree_node *tree_node_new(char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, const char* type) //tree_node_type type)
{
    tree_node *node = (tree_node *)malloc(sizeof(tree_node));
    node->string = string;
    node->start_string = start_string;
    node->length_string = length_string;
    node->parent = parent;
    node->childs = NULL;
    node->childs_count = 0;
    node->type = get_type_index(type);
    // if(parent==NULL && rootTree==NULL)
    // {
    //     rootTree=node;
    // }
    return node;
}

int get_start(tree_node* node){
    return node->start_string+node->length_string;
}

void update_length_parents(tree_node* node){
    tree_node* parent = node;
    while (parent!=NULL)
    {
        int string_length = 0;
        // tree_node_print(parent, 0);
        for (size_t i = 0; i < parent->childs_count; i++)
        {
            string_length+=parent->childs[i]->length_string;
        }
        parent->length_string=string_length;
        // if(string_length>=112){
        //     printf("string_length:%d\n",string_length);
        // }
        // tree_node_print_all(parent,0);
        // if(rootTree->length_string>=112){
        //     tree_node_print_all(rootTree,0);
        // }
        // tree_node_print(rootTree, 0);
        // printf("newlen:%d\n",parent->length_string);
        parent=parent->parent;
    }
}

void tree_node_add_child_node(tree_node *parent,tree_node *node)
{
    parent->childs = realloc(parent->childs, sizeof(tree_node) * (parent->childs_count + 1)); //(tree_node *)
    node->parent=parent;
    parent->childs[parent->childs_count] = node;
    parent->childs_count++;
    update_length_parents(parent);
    // printf("ajoutch:%s<=%s childs:%d\n",tree_node_string[node->type],tree_node_string[type],node->childs_count);
    return;
}
tree_node *tree_node_add_child(tree_node *parent, char *string, uint16_t start_string, uint16_t length_string, const char* type) //tree_node_type type)
{

    parent->childs_count++;
    parent->childs = realloc(parent->childs, sizeof(tree_node) * parent->childs_count); //(tree_node *)
    parent->childs[parent->childs_count-1] = tree_node_new(string, start_string, length_string, parent, type);
    update_length_parents(parent);
    // printf("ajoutch:%s<=%s childs:%d\n",tree_node_string[node->type],tree_node_string[type],node->childs_count);
    return parent->childs[parent->childs_count - 1];
}

void tree_node_find_type(tree_node *node, int _type,_Token **r)
{
    if (node->type == _type)
    {
        // printf("found:%s\n", tree_node_string[node->type]);
        // tree_node_print(node,0);
        add_token(r,node);//ajouter a liste chainee
    }
    for (uint16_t i = 0; i < node->childs_count; i++)
    {
        tree_node_find_type(node->childs[i], _type,r);
    }
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
int find_node_index(tree_node *node)
{
    if (node->parent == NULL)
    {
        return -1;
    }
    for (uint16_t i = 0; i < node->parent->childs_count; i++)
    {
        if (node->parent->childs[i] == node)
        {
            return i;
        }
    }
    return -1;
}

void tree_node_free(tree_node *node)
{
    if(node==NULL) return;
    while (node->childs_count > 0)
    {
        tree_node_free(node->childs[0]);
    }
    // printf("free:%s\n",tree_node_string[node->type]);
    if(node->parent && node->parent->childs_count>0){
        tree_node* parent=node->parent;
        int index=find_node_index(node);
        // printf("free:%s:%d childs:%d\n",tree_node_string[node->type],index,parent->childs_count);
        free(parent->childs[index]);
        for (uint16_t i = index; i < parent->childs_count-1; i++)
        {
            parent->childs[i]=parent->childs[i+1];
        }
        parent->childs_count--;
        if(parent->childs_count==0){
            free(parent->childs);
            parent->childs=NULL;
        }else{
            parent->childs = realloc(parent->childs, sizeof(tree_node) * parent->childs_count);
        }
        update_length_parents(parent);
    }else{
        if(node==rootTree){
            rootTree=NULL;
        }
        free(node);
    }
}
tree_node* tree_node_add_node(tree_node* parent,char* name){
    return tree_node_add_child(parent, parent->string, get_start(parent), 0, name);
}
void move_childs(tree_node* from,tree_node* to){
    for (uint16_t i = 0; i < from->childs_count; i++){
        tree_node_add_child_node(to,from->childs[i]);
    }
    from->childs_count=0;

    // for (uint16_t i = 0; i < from->childs_count; i++)
    // {
    //     // tree_node_add_child(to,from->childs[i]->string,from->childs[i]->start_string,from->childs[i]->length_string,"tmp")->type=from->childs[i]->type;
    //     tree_node_add_child_node(to,from->childs[i]);
    // }
    // from->childs_count=0;
    // // free(from->childs);
    // from->childs=NULL;
    // from->start_string=get_start(to);
    // from->length_string=0;
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
    printf("\" -> |%d|=%d\n",node->start_string,node->length_string);
}

// print all nodes in tree with format:
// [depth:name] = string
void tree_node_print_all(tree_node *node, uint16_t level)
{
    // printf("[%d:%s] = \"%s\"\n", level, node->string, node->string);
    tree_node_print(node, level);
    // printf("childs:%d (%s)\n",node->childs_count,tree_node_string[node->type]);
    for (uint16_t i = 0; node != NULL && i < node->childs_count; i++)
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
