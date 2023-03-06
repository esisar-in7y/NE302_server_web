#ifndef _TREE_H
#define _TREE_H
#include "global.h"
#include "../src/api.h"
#if ABNF==0
__attribute__((unused)) static char* tree_node_string[]={
    "absolute_path",
    "Accept",
    "Accept_header",
    "case_insensitive_string",
    "comment",
    "Connection",
    "Connection_header",
    "connection_option",
    "ctext",
    "header_field",
    "Host",
    "host",
    "Host_header",
    "HTTP_message",
    "HTTP_name",
    "HTTP_version",
    "media_range",
    "method",
    "origin_form",
    "OWS",
    "product",
    "product_version",
    "reg_name",
    "request_line",
    "request_target",
    "RWS",
    "start_line",
    "tchar",
    "token",
    "unreserved",
    "uri_host",
    "User_Agent",
    "User_Agent_header",
    "__alpha",
    "__crlf",
    "__digit",
    "__num",
    "__range",
    "__sp"
};


typedef enum
{
    absolute_path=0,
    Accept,
    Accept_header,
    case_insensitive_string,
    comment,
    Connection,
    Connection_header,
    connection_option,
    ctext,
    header_field,
    Host,
    host,
    Host_header,
    HTTP_message,
    HTTP_name,
    HTTP_version,
    media_range,
    method,
    origin_form,
    OWS,
    product,
    product_version,
    reg_name,
    request_line,
    request_target,
    RWS,
    start_line,
    tchar,
    token,
    unreserved,
    uri_host,
    User_Agent,
    User_Agent_header,
    __alpha,
    __crlf,
    __digit,
    __num,
    __range,
    __sp
} tree_node_type;
#elif ABNF==1
__attribute__((unused)) static char* tree_node_string[]={
    "nombre",
    "ponct",
    "separateur",
    "debut",
    "fin",
    "mot",
    "message",
    "alpha",
    "digit"
};

// typedef enum
// {
//     nombre=0,
//     ponct,
//     separateur,
//     debut,
//     fin,
//     mot,
//     message,
//     alpha,
//     digit
// } tree_node_type;
#endif
typedef struct _tree_node
{
    uint16_t start_string;
    uint16_t length_string;
    uint16_t childs_count;
    char *string;
    // tree_node_type type;
    int type;
    struct _tree_node *parent;
    struct _tree_node **childs;
} tree_node;
//global variable
__attribute__((unused)) static tree_node* rootTree=NULL;

tree_node *tree_node_init(char *string);
tree_node *tree_node_new(char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, const char* type);//tree_node_type type
tree_node *tree_node_add_child(tree_node *parent, char *string, uint16_t start_string, uint16_t length_string, const char* type); //tree_node_type type
tree_node *tree_node_get_child(tree_node *parent, char *string, uint16_t start_string, uint16_t length_string);
tree_node *tree_node_get_child_by_index(tree_node *node, uint16_t index);
tree_node *tree_node_get_child_by_string(tree_node *node, char *string);
void tree_node_add_child_node(tree_node *parent,tree_node *node);
void tree_node_free(tree_node *node);
void tree_node_print(tree_node *node, uint16_t level);
void tree_node_print_all(tree_node *node, uint16_t level);
int get_type_index(char* string);
void update_length_parents(tree_node* node);
tree_node *tree_node_find_type(tree_node *node, int _type,_Token **r);
int get_start(tree_node* node);
#endif