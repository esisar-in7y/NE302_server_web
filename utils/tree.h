#ifndef _TREE_H
#define _TREE_H
#include "global.h"
#include "../src/api.h"

__attribute__((unused)) static char *tree_node_string[] = {
    "%x23-5B",
    "%x5D-7E",
    ",",
    ".",
    "/",
    "\\",
    "%",
    "?",
    ":",
    "::",
    "=",
    ";",
    "1",
    "]","[",
    "2",
    "5",
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
    "Content-Length",
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
    "HEXALPHA",
    "host",
    "Host:",
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
    "Transfert-encoding:",
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

// global variable
__attribute__((unused)) static tree_node *rootTree = NULL;

tree_node *tree_node_init(char *string);
tree_node *tree_node_new(unsigned char *string, uint16_t start_string, uint16_t length_string, tree_node *parent, const char *type);        // tree_node_type type
tree_node *tree_node_add_child(tree_node *parent, unsigned char *string, uint16_t start_string, uint16_t length_string, const char *type);  // tree_node_type type
tree_node *tree_node_get_child(tree_node *parent,  unsigned char *string, uint16_t start_string, uint16_t length_string);
tree_node *tree_node_get_child_by_index(tree_node *node, uint16_t index);
// tree_node *tree_node_get_child_by_string(tree_node *node, char *string);
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