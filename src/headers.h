#include "../utils/global.h"
void populate_version(tree_node* root, _headers_request* header_req);
void populate_connection(tree_node* root, _headers_request* header_req);
void populate_content_length(tree_node* root, _headers_request* header_req);
void populate_transfer_encoding(tree_node* root, _headers_request* header_req);
void populate_method(tree_node* root, _headers_request* header_req);
void populate_range(tree_node* root, _headers_request* header_req);
void populate_host(tree_node* root, _headers_request* header_req);
void send_headers(int clientId,_headers_response* header_response) 