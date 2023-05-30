#include "../utils/global.h"
#include "../lib/request.h"
#include "answer.h"

void populate_version(tree_node* root, _headers_request* header_req);
void populate_connection(tree_node* root, _headers_request* header_req);
void populate_content_length(tree_node* root, _headers_request* header_req);
void populate_transfert_encoding(tree_node* root, _headers_request* header_req);
void populate_method(tree_node* root, _headers_request* header_req);
void populate_range(tree_node* root, _headers_request* header_req);
void populate_host(tree_node* root, _headers_request* header_req);
void populate_accept_encoding(tree_node* root, _headers_request* header_req);
void populate_ranges(tree_node* root, _headers_request* header_req);
void send_headers(_Response* response);