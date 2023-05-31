#include "headers.h"

void populate_version(tree_node* root, _headers_request* header_req) {
	if (header_req->version == 0) {
		char* version = get_first_value(root, "HTTP_version");
		if (strcasecmp(version, "HTTP/1.1") == 0) {			// HTTP/1.1
			header_req->version = HTTP1_1;
		} else if (strcasecmp(version, "HTTP/1.0") == 0) {	// HTTP/1.0
			header_req->version = HTTP1_0;
		}
		free(version);
	}
}

void populate_connection(tree_node* root, _headers_request* header_req) {
	if (header_req->connection == 0) {
		char* connection = get_first_value(root, "Connection");
		if (connection != NULL) {
			if (strcasecmp(connection, "close") == 0) {
				header_req->connection = CLOSE;
			} else if (strcasecmp(connection, "keep-alive") == 0 || strcasecmp(connection, "keepalive") == 0 || strcasecmp(connection, "keep alive") == 0) {
				header_req->connection = KEEP_ALIVE;
			}
		}
		free(connection);
	}
}

void populate_content_length(tree_node* root, _headers_request* header_req) {
	if (header_req->content_length == NULL) {
		char* content_length = get_first_value(root, "Content-Length");
		if (content_length != NULL) {
			header_req->content_length = (long int*)calloc(1, sizeof(long int*));
			*header_req->content_length = atoi(content_length);
		}
		free(content_length);
	}
}
// Populate
void populate_transfert_encoding(tree_node* root, _headers_request* header_req) {
	if (!(header_req->transfert_encoding.initialized)) {
		header_req->transfert_encoding.initialized = true;
		_Token* node_token = searchTree(root, "Transfer-Encoding");	 // TODO multiple transfer encoding
		tree_node* node;
		while (node_token != NULL) {
			header_req->transfert_encoding.isPresent = true;
			node = (tree_node*)node_token->node;
			char* transfer_encoding = getElementValue(node, (unsigned int*)&node->length_string);
			printf("transfer_encoding:%s\n",transfer_encoding);
			if (transfer_encoding==NULL){
				free(transfer_encoding);
				continue;
			}
			if (have_separators(transfer_encoding, "chunked")) {
				header_req->transfert_encoding.CHUNKED = true;
			} if (have_separators(transfer_encoding, "identity")) {
				header_req->transfert_encoding.IDENTITY = true;
			} if (have_separators(transfer_encoding, "compress")) {
				header_req->transfert_encoding.COMPRESS = true;
			} if (have_separators(transfer_encoding, "deflate")) {
				header_req->transfert_encoding.DEFLATE = true;
			} if (have_separators(transfer_encoding, "gzip") || have_separators(transfer_encoding, "x-gzip")) {
				header_req->transfert_encoding.GZIP = true;
			} if (have_separators(transfer_encoding, "br")) {
				header_req->transfert_encoding.BR = true;
			}
			free(transfer_encoding);
			node_token = node_token->next;
		}
	}
}

void populate_accept_encoding(tree_node* root, _headers_request* header_req) {
	if (!(header_req->accept_encoding.initialized)) {
		header_req->accept_encoding.initialized = true;
		char* accept_encoding = getFieldValueFromFieldName(root,"Accept-Encoding");
		printf("accept_encoding:%s\n",accept_encoding);
		if(accept_encoding==NULL) {
			free(accept_encoding);
			return;
		}
		if (have_separators(accept_encoding, "compress")) {
			header_req->accept_encoding.COMPRESS = true;
		} if (have_separators(accept_encoding, "deflate")) {
			header_req->accept_encoding.DEFLATE = true;
		} if (have_separators(accept_encoding, "gzip")) {
			header_req->accept_encoding.GZIP = true;
		} if (have_separators(accept_encoding, "br")) {
			header_req->accept_encoding.BR = true;
		}
		free(accept_encoding);
	}
}

void populate_method(tree_node* root, _headers_request* header_req) {
	if (header_req->methode == 0) {
		char* method = get_first_value(root, "method");
		if (strcasecmp(method, "GET") == 0) {
			header_req->methode = GET;
		} else if (strcasecmp(method, "HEAD") == 0) {
			header_req->methode = HEAD;
		} else if (strcasecmp(method, "POST") == 0) {
			header_req->methode = POST;
		}
		free(method);
	}
}

void populate_host(tree_node* root, _headers_request* header_req) {
	if (header_req->host == NULL) {
		header_req->host = get_first_value(root, "uri_host");
	}
}

// void populate_range(tree_node* root, _headers_request* header_req){
// 	if (header_req->ranges == NULL) {
// 		char* range = get_first_value(root, "Range");
// 		if (range != NULL) {
// 			header_req->ranges = range;
// 		}
// 	}
// }


void populate_ranges(tree_node* root,_headers_request* header_req) {
    if (header_req->ranges == NULL) {
        char* range =  getFieldValueFromFieldName(root, "Range");
        if (range != NULL) {
            size_t length = strlen(range);
            header_req->ranges = (_Ranges*)calloc(1, sizeof(_Ranges));
            header_req->ranges->range = (_Range*)calloc(1, sizeof(_Range));
            header_req->ranges->next = NULL;
            _Ranges* current_range = header_req->ranges;

            // Parse the range string
            char* start_str = range + 6;  // Skip "bytes="
            char* end_str = strchr(start_str, '-');
            while (end_str != NULL) {
                // Extract start and end values
                int start = atoi(start_str);
                if(start<0){
                    start=-1;
                }
                int end = atoi(end_str + 1);
                if(end==0){
                    end=-1;
                }

                // Create a new _Range structure
                _Range* new_range = (_Range*)calloc(1,sizeof(_Range));
                new_range->start = start;
                new_range->end = end;

                // Set the new range in the current _Ranges node
                current_range->range = new_range;

                // Check for additional ranges
                start_str = strchr(end_str + 1, ',');
                if (start_str != NULL) {
                    // Allocate memory for the next _Ranges node
                    current_range->next = (_Ranges*)calloc(1, sizeof(_Ranges));
                    current_range = current_range->next;
                    current_range->range = (_Range*)calloc(1, sizeof(_Range));
                    current_range->next = NULL;

                    // Move the start_str pointer to the next range
                    start_str += 2;  // Skip ", "
                    end_str = strchr(start_str, '-');
                } else {
                    break;  // No more ranges, exit the loop
                }
            }
        }
		free(range);
    }
}
