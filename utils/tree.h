#ifndef _TREE_H
#define _TREE_H
#include "../src/api.h"
#include "global.h"
#if ABNF == 0
/* __attribute__((unused)) static char* tree_node_string[]={
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
}; */
__attribute__((unused)) static char *tree_node_string[] = {
    "%x23-5B",
    "%x5D-7E",
    ",",
    ".",
    "/",
    "\\",
    ":",
    "::",
    "=",
    "0-4",
    "0-5",
    "1-9",
    "absolute_path",
    "alpha",
    "ALPHA",
    "BWS",
    "chunked",
    "compress",
    "Connection",
    "Connection:",
    "Content-Length:",
    "Cookie:",
    "Content-Type:",
    "Expect:",
    "Connection_header",
    "connection_option",
    "Content_Length",
    "Content_Length_header",
    "Content_Type",
    "Content_Type_header",
    "Cookie_header",
    "cookie_name",
    "cookie_octet",
    "cookie_pair",
    "cookie_string",
    "cookie_value",
    "CRLF",
    "debut",
    "dec_octet",
    "deflate",
    "digit",
    "DIGIT",
    "DQUOTE",
    "Expect",
    "Expect_header",
    "field_content",
    "field_name",
    "field_value",
    "field_vchar",
    "fin",
    "gzip",
    "h16",
    "header_field",
    "HEXDIG",
    "host",
    "Host",
    "Host_header",
    "HTAB",
    "HTTP",
    "HTTP_message",
    "HTTP_name",
    "HTTP_version",
    "IPv4address",
    "IPv6address",
    "IPvFuture",
    "IP_literal",
    "ls32",
    "media_type",
    "message_body",
    "method",
    "mot",
    "nombre",
    "obs_fold",
    "obs_text",
    "origin_form",
    "OWS",
    "parameter",
    "pchar",
    "pct_encoded",
    "ponct",
    "port",
    "qdtext",
    "query",
    "quoted_pair",
    "quoted_string",
    "reg_name",
    "request_line",
    "request_target",
    "segment",
    "separateur",
    "SP",
    "start_line",
    "subtype",
    "sub_delims",
    "tchar",
    "token",
    "Transfer-Encoding",
    "transfert_coding",
    "Transfert_encoding",
    "Transfer_Encoding_header",
    "transfer_extension",
    "transfer_parameter",
    "type",
    "unreserved",
    "uri_host",
    "vchar",
    "tmp"
};

// typedef enum
// {
//     absolute_path=0,
//     Accept,
//     Accept_header,
//     case_insensitive_string,
//     comment,
//     Connection,
//     Connection_header,
//     connection_option,
//     ctext,
//     header_field,
//     Host,
//     host,
//     Host_header,
//     HTTP_message,
//     HTTP_name,
//     HTTP_version,
//     media_range,
//     method,
//     origin_form,
//     OWS,
//     product,
//     product_version,
//     reg_name,
//     request_line,
//     request_target,
//     RWS,
//     start_line,
//     tchar,
//     token,
//     unreserved,
//     uri_host,
//     User_Agent,
//     User_Agent_header,
//     __alpha,
//     __crlf,
//     __digit,
//     __num,
//     __range,
//     __sp
// } tree_node_type;
#elif ABNF == 1
__attribute__((unused)) static char *tree_node_string[] = {
    "nombre",
    "ponct",
    "separateur",
    "debut",
    "fin",
    "mot",
    "message",
    "alpha",
    "digit"};

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
typedef struct _tree_node {
    uint16_t start_string;
    uint16_t length_string;
    uint16_t childs_count;
    char *string;
    // tree_node_type type;
    int type;
    struct _tree_node *parent;
    struct _tree_node **childs;
} tree_node;
// global variable
__attribute__((unused)) static tree_node *rootTree = NULL;

tree_node *tree_node_init(char *string);
tree_node *tree_node_new(char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, const char *type);        // tree_node_type type
tree_node *tree_node_add_child(tree_node *parent, char *string, uint16_t start_string, uint16_t length_string, const char *type);  // tree_node_type type
tree_node *tree_node_get_child(tree_node *parent, char *string, uint16_t start_string, uint16_t length_string);
tree_node *tree_node_get_child_by_index(tree_node *node, uint16_t index);
tree_node *tree_node_get_child_by_string(tree_node *node, char *string);
void tree_node_add_child_node(tree_node *parent, tree_node *node);
void tree_node_free(tree_node *node);
void tree_node_print(tree_node *node, uint16_t level);
void tree_node_print_all(tree_node *node, uint16_t level);
int get_type_index(const char *string);
void update_length_parents(tree_node *node);
void tree_node_find_type(tree_node *node, int _type, _Token **r);
int get_start(tree_node *node);
void move_childs(tree_node *from, tree_node *to);
tree_node* tree_node_add_node(tree_node* parent,char* name);
tree_node *tree_node_tmp(tree_node *parent);
#endif