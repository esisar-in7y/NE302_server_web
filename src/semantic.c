#include "semantic.h"

int checkSemantics(tree_node* root) {
	// On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
	tree_node* node = searchTree(root, "method")->node;
	char* method = getElementValue(node,  node->length_string);
	if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0 && strcmp(method, "POST") != 0) {
		return 501;
	}
	// On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
	node =(tree_node*)  searchTree(root, "HTTP_version")->node;
	char* version = getElementValue(node,   node->length_string);
	if (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1") != 0) {
		return 505;
	}

	// On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
	_Token* t = searchTree(root, "Transfer_Encoding_header");
	if (t != NULL) {
		// If 1.0 and Transfer-Encoding header => 400 Bad Request
		t = searchTree(root, "HTTP_version");
		node =(tree_node*) t->node;
		char* version = getElementValue(node, node->length_string);
		if (strcmp(version, "HTTP/1.0") == 0) {
			return 400;
		}
		char* accepted_encodings[]={"gzip", "compress", "deflate", "br","chunked"};
		// If it's a transfer-coding it doesn't understand => 501 Not Implemented
		t = searchTree(root, "transfert_coding");
		// Parcourir la liste chainée de transfert_coding et vérifier que chaque élément est chunked gzip compress deflate
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = (tree_node*)t->node;
			char* transferEncoding = getElementValue(node, node->length_string);
			if(!isin(transferEncoding, accepted_encodings)) {
				return 501;
			}
		}

		// If chunked more than once => 400 Bad Request
		int count = 0;
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = (tree_node*)t->node;
			char* transferEncoding = getElementValue(node, node->length_string);
			if (strcmp(transferEncoding, "chunked") == 0) {
				count++;
			}
		}
		if (count > 1) {
			return 400;
		}

		// If the last transfer coding is not chunked => 400 Bad Request
		t = searchTree(root, "transfert_coding");
		while (t->next != NULL) {
			t = t->next;
		}
		node =(tree_node*) t->node;
		char* transferEncoding = getElementValue(node, node->length_string);
		if (strcmp(transferEncoding, "chunked") != 0) {
			return 400;
		}

		// Referer Header
		t = searchTree(root, "header_field");
		_Token* fieldName;
		while (t->next != NULL) {
			fieldName = searchTree(t, "field_name");
			node = (tree_node*)fieldName->node;
			if (strcmp(getElementValue(node, node->length_string), "Referer") == 0) {
				// If the URL contains a fragment (indicated by a '#' symbol) or userinfo (indicated by a '@' symbol) => 400 Bad Request
				t = searchTree(root, "field_value");
				node =(tree_node*) t->node;
				char* absolutePath = getElementValue(node, node->length_string);
				if (strchr(absolutePath, '#') != NULL || strchr(absolutePath, '@') != NULL) {
					return 400;
				}
				break;
			}
			t = t->next;
		}

		// Content-Length Header
		t = searchTree(root, "Content-Length");
		if (t != NULL) {
			node =(tree_node*) t->node;
			char* contentLength = getElementValue(node, node->length_string);
			if (atoi(contentLength) < 0) {
				return 400;
			}
		}
	}
	return 200;
}


int checkConnection(tree_node* root) {
	_Token* tok = searchTree(root, "HTTP_version");
	_Token* tok2 = searchTree(root, "Connection");
	_Token* tok3 = searchTree(root, "Proxy-Connection");
	tree_node* node = (tree_node*)tok->node;
	tree_node* node2 = (tree_node*)tok2->node;
	tree_node* node3 = (tree_node*)tok3->node;
	char* version = getElementValue(node, node->length_string);
	char* connection = getElementValue(node2, node2->length_string);
	char* proxyConnection = getElementValue(node3, node3->length_string);
	if (strcmp(version, "HTTP/1.1")) {
		if (strcmp(connection, "close") != 0) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}
	} else if (strcmp(version, "HTTP/1.0")) {
		if (strcmp(connection, "keep-alive") == 0 || strcmp(connection, "Keep-Alive") == 0) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}
	} else if (strcmp(proxyConnection, "keep-alive") == 0 || strcmp(proxyConnection, "Keep-Alive") == 0) {
		if (strcmp(version, "HTTP/1.1")) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		} else if (strcmp(version, "HTTP/1.0")) {
			if (strcmp(connection, "close") != 0) {
				if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
					return 400;
				}
			}
		}
	}
	return NULL;
}

// **Accept-encoding header**

// Verif one of the content codings listed is the representation's content coding (si q!=0)  or \*/\*else =>  415 Unsupported Media Type
// Without Accept-encoding => everything is considered as acceptable so do nothing
// If representation has no content coding => acceptable

int checkAcceptEncoding(tree_node* root) {
	char* accepted_encodings[]={"gzip", "compress", "deflate", "br","chunked"};
	_Token* tok = searchTree(root, "header_field");
    while (tok != NULL) {
        tree_node* node =(tree_node*) searchTree(tok->node, "field_name")->node;
        char* name = getElementValue(node, node->length_string);
        if (strcmp(name, "Accept-Encoding") == 0) {
            node = searchTree(tok->node, "field_value")->node;
            char* value =getElementValue(node, node->length_string);
			if(!isin(value, accepted_encodings)){
				return 415;
			}
        }
        tok = tok->next;
    }
	return NULL;
}

// **Host header**

// Overlap with Request-target for URI

// If version 1.1 and no Host header => 400 Bad Request

// If request-target include authority component and Host have a value different to this component => 400 Bad Request

// If several Host header => 400 Bad Request

int checkHostHeader(tree_node* root) {
	tree_node* node_http_version = (tree_node*) searchTree(root, "HTTP_version")->node;
	char* http_version = getElementValue(node_http_version, node_http_version->length_string);
	if (strcmp(http_version, "HTTP/1.1") == 0) {
		_Token* tok2 = searchTree(root, "Host");
		if (tok2 == NULL) {
			return 400;
		}
	}
	return NULL;
}