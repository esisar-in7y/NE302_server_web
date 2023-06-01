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
        node=tree_node_new((unsigned char*)string, 0, 0, NULL, "HTTP_message");
    #elif ABNF==1
        node=tree_node_new((unsigned char*)string, 0, 0, NULL, "message");//strlen(string)
    #endif
    rootTree=node;
    return node;
}
tree_node *tree_node_tmp(tree_node *parent){
    return tree_node_new(parent->string, get_start(parent), 0, NULL, "tmp");
}
tree_node *tree_node_new( unsigned char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, const char* type) //tree_node_type type)
{
    tree_node *node = (tree_node *)calloc(1,sizeof(tree_node));
    node->string = string;
    node->start_string = start_string;
    node->length_string = length_string;
    node->parent = parent;
    node->childs = NULL;
    node->childs_count = 0;
    node->type = get_type_index(type);
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
        for (size_t i = 0; i < parent->childs_count; i++)
        {
            string_length+=parent->childs[i]->length_string;
        }
        parent->length_string=string_length;
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
    return;
}
tree_node *tree_node_add_child(tree_node *parent,  unsigned char *string, uint16_t start_string, uint16_t length_string, const char* type) //tree_node_type type)
{

    parent->childs_count++;
    parent->childs = realloc(parent->childs, sizeof(tree_node) * parent->childs_count); //(tree_node *)
    parent->childs[parent->childs_count-1] = tree_node_new(string, start_string, length_string, parent, type);
    update_length_parents(parent);
    return parent->childs[parent->childs_count - 1];
}

void tree_node_find_type(tree_node *node, int _type,_Token **r)
{
    if (node->type == _type)
    {
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
    if(node->parent && node->parent->childs_count>0){
        tree_node* parent=node->parent;
        int index=find_node_index(node);
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
        node=NULL;
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
    free(from->childs);
}

void tree_node_print(tree_node *node, uint16_t level)
{
    if (node == NULL)
    {
        printf("NULL\n");
        return;
    }
    if(
        node->type==get_type_index("field_content") ||
        node->type==get_type_index("tchar") ||
        node->type==get_type_index("ALPHA") ||
        node->type==get_type_index("CRLF") ||
        node->type==get_type_index("OWS") ||
        node->type==get_type_index("SP") ||
        node->type==get_type_index("cookie_octet") ||
        node->type==get_type_index(":") ||
        node->type==get_type_index("vchar") ||
        node->type==get_type_index("pchar") ||
        node->type==get_type_index("/") ||
        node->type==get_type_index("unreserved") ||
        node->type==get_type_index("DIGIT") ||
        node->type==get_type_index(".") ||
        node->type==get_type_index("field_vchar")
        ){
        return;
    }
    for (uint16_t i = 0; i < level; i++)
    {
        printf(" ");
    }
    printf("[%d:%s] = \"", level, tree_node_string[node->type]);
    print_sub_str(node->string, node->start_string, node->length_string);
    printf("\"\n");
}

// print all nodes in tree with format:
// [depth:name] = string
void tree_node_print_all(tree_node *node, uint16_t level)
{
    tree_node_print(node, level);
    for (uint16_t i = 0; node != NULL && i < node->childs_count; i++)
    {
        tree_node_print_all(node->childs[i], level + 1);
    }
}


// Path: src/utils/api.c
#include "../src/api.h"
void *getRootTree()
{
    return rootTree;
}
