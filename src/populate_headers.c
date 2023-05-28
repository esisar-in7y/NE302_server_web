#include "headers.h"

void populate_version(tree_node* root, _headers_request* header_req) {
	if (header_req->version == NULL) {
		tree_node* node = (tree_node*)searchTree(root, "HTTP_version")->node;
		char* version = getElementValue(node, node->length_string);
		if (strcasecmp(version, "HTTP/1.1") == 0) {			// HTTP/1.1
			header_req->version = 1;
		} else if (strcasecmp(version, "HTTP/1.0") == 0) {	// HTTP/1.0
			header_req->version = 0;
		}
	}
}

void populate_connection(tree_node* root, _headers_request* header_req) {
	if (header_req->connection == NULL) {
		char* connection = get_first_value(root, "Connection");
		if (connection != NULL) {
			if (strcasecmp(connection, "close") == 0) {
				header_req->connection = 0;
			} else if (strcasecmp(connection, "keep-alive") == 0) {
				header_req->connection = 1;
			} else if (strcasecmp(connection, "keepalive") == 0) {
				header_req->connection = 1;
			} else if (strcasecmp(connection, "keep alive") == 0) {
				header_req->connection = 1;
			}
		}
	}
}

void populate_content_length(tree_node* root, _headers_request* header_req){
	if (header_req->content_length == NULL) {
		char* content_length = get_first_value(root, "Content-Length");
		if (content_length != NULL) {
			header_req->content_length = atoi(content_length);
		}
	}
}
// Populate
void populate_transfert_encoding(tree_node* root, _headers_request* header_req){
	if (!(header_req->transfert_encoding.initialized)) {
		header_req->transfert_encoding.initialized=true;
		_Token* node_token = searchTree(root, "Transfer-Encoding");	 // TODO multiple transfer encoding
		tree_node* node;
		while (node_token->next != NULL) {
			header_req->transfert_encoding.isPresent=true;
			node = (tree_node*)node_token->node;
			char* transfer_encoding = getElementValue(node, node->length_string);
			if (have_separators(transfer_encoding, "chunked") != NULL) {
				header_req->transfert_encoding.CHUNKED=true;
			} else if (have_separators(transfer_encoding, "compress") != NULL) {
				header_req->transfert_encoding.COMPRESS=true;
			} else if (have_separators(transfer_encoding, "deflate") != NULL) {
				header_req->transfert_encoding.DEFLATE=true;
			} else if (have_separators(transfer_encoding, "gzip") != NULL) {
				header_req->transfert_encoding.GZIP=true;
			} else if (have_separators(transfer_encoding, "br") != NULL) {
				header_req->transfert_encoding.BR=true;
			}
			node_token = node_token->next;
		}
	}
}

void populate_accept_encoding(tree_node* root, _headers_request* header_req){
	if (!(header_req->accept_encoding.initialized)) {
		header_req->accept_encoding.initialized=true;
		_Token* node_token = searchTree(root, "Accept-Encoding");	 // TODO multiple transfer encoding
		tree_node* node;
		while (node_token->next != NULL) {
			node = (tree_node*)node_token->node;
			char* accept_encoding = getElementValue(node, node->length_string);
			if (have_separators(accept_encoding, "compress") != NULL) {
				header_req->accept_encoding.COMPRESS=true;
			} else if (have_separators(accept_encoding, "deflate") != NULL) {
				header_req->accept_encoding.DEFLATE=true;
			} else if (have_separators(accept_encoding, "gzip") != NULL) {
				header_req->accept_encoding.GZIP=true;
			} else if (have_separators(accept_encoding, "br") != NULL) {
				header_req->accept_encoding.BR=true;
			}
			node_token = node_token->next;
		}
	}
}

void populate_method(tree_node* root, _headers_request* header_req){
	if (header_req->methode == NULL) {
		tree_node* node = searchTree(root, "method")->node;
		char* method = getElementValue(node, node->length_string);
		if (strcasecmp(method, "GET") == 0) {
			header_req->methode = GET;
		} else if (strcasecmp(method, "HEAD") == 0) {
			header_req->methode = HEAD;
		} else if (strcasecmp(method, "POST") == 0) {
			header_req->methode = POST;
		}
	}
}

void populate_host(tree_node* root, _headers_request* header_req){
	if (header_req->host == NULL) {
		header_req->host = get_first_value(root, "Host");
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