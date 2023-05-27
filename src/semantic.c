#include "semantic.h"

int checkSemantics(tree_node* root) {
	// On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
	tree_node* node = searchTree(root, "method")->node;
	char* method = getElementValue(node, node->length_string);
	if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "HEAD") != 0 && strcasecmp(method, "POST") != 0) {
		return 501;
	}
	// On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
	if (checkVersion(root) == 505) {
		return 505;
	}

	// On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
	_Token* t = searchTree(root, "Transfer_Encoding_header");
	if (t != NULL) {
		// If 1.0 and Transfer-Encoding header => 400 Bad Request
		t = searchTree(root, "HTTP_version");
		node = (tree_node*)t->node;
		char* version = getElementValue(node, node->length_string);
		if (strcasecmp(version, "HTTP/1.0") == 0) {
			return 400;
		}
		char* accepted_encodings[] = {"gzip", "compress", "deflate", "br", "chunked"};
		// If it's a transfer-coding it doesn't understand => 501 Not Implemented
		t = searchTree(root, "transfert_coding");
		// Parcourir la liste chainée de transfert_coding et vérifier que chaque élément est chunked gzip compress deflate
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = (tree_node*)t->node;
			char* transferEncoding = getElementValue(node, node->length_string);
			if (!isin(transferEncoding, accepted_encodings)) {
				return 501;
			}
		}

		// If chunked more than once => 400 Bad Request
		int count = 0;
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = (tree_node*)t->node;
			char* transferEncoding = getElementValue(node, node->length_string);
			if (strcasecmp(transferEncoding, "chunked") == 0) {
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
		node = (tree_node*)t->node;
		char* transferEncoding = getElementValue(node, node->length_string);
		if (strcasecmp(transferEncoding, "chunked") != 0) {
			return 400;
		}

		// Referer Header
		t = searchTree(root, "header_field");
		_Token* fieldName;
		while (t->next != NULL) {
			fieldName = searchTree(t, "field_name");
			node = (tree_node*)fieldName->node;
			if (strcasecmp(getElementValue(node, node->length_string), "Referer") == 0) {
				// If the URL contains a fragment (indicated by a '#' symbol) or userinfo (indicated by a '@' symbol) => 400 Bad Request
				t = searchTree(root, "field_value");
				node = (tree_node*)t->node;
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
			node = (tree_node*)t->node;
			char* contentLength = getElementValue(node, node->length_string);
			if (atoi(contentLength) < 0) {
				return 400;
			}
		}
	}
	return 0;
}

int checkVersion(tree_node* root) {
	tree_node* node = (tree_node*)searchTree(root, "HTTP_version")->node;
	char* version = getElementValue(node, node->length_string);
	if (strcasecmp(version, "HTTP/1.0") != 0 && strcasecmp(version, "HTTP/1.1") != 0) { // Version unsupported
		return 505;
	}
	else if (strcasecmp(version, "HTTP/1.1") == 0) { // HTTP/1.1
		return 1;
	}
	else { // HTTP/1.0
		return 0;
	}
}

int checkConnection(tree_node* root) {
	tree_node* nodeConnection = (tree_node*)searchTree(root, "Connection")->node;
	tree_node* nodeProxy = (tree_node*)searchTree(root, "Proxy-Connection")->node;
	char* connection = getElementValue(nodeConnection, nodeConnection->length_string);
	char* proxyConnection = getElementValue(nodeProxy, nodeProxy->length_string);
	if (checkVersion(root) == 1) {
		if (strcasecmp(connection, "close") != 0) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}
	} else if (checkVersion(root) == 0) {
		if (strcasecmp(connection, "keep-alive") == 0) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}
	} else if (strcasecmp(proxyConnection, "keep-alive") == 0) {
		if (checkVersion(root) == 1) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		} else if (checkVersion(root) == 0) {
			if (strcasecmp(connection, "close") != 0) {
				if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
					return 400;
				}
			}
		}
	}
	return 0;
}

// **Accept-encoding header**

// Verif one of the content codings listed is the representation's content coding (si q!=0)  or \*/\*else =>  415 Unsupported Media Type
// Without Accept-encoding => everything is considered as acceptable so do nothing
// If representation has no content coding => acceptable

int checkAcceptEncoding(tree_node* root) {
	char* accepted_encodings[] = {"gzip", "compress", "deflate", "br", "chunked"};
	_Token* tok = searchTree(root, "header_field");
	while (tok != NULL) {
		tree_node* node = (tree_node*)searchTree(tok->node, "field_name")->node;
		char* name = getElementValue(node, node->length_string);
		if (strcasecmp(name, "Accept-Encoding") == 0) {
			node = searchTree(tok->node, "field_value")->node;
			char* value = getElementValue(node, node->length_string);
			if (!isin(value, accepted_encodings)) {
				return 415;
			}
		}
		tok = tok->next;
	}
	return 0;
}

// **Host header**

// Overlap with Request-target for URI

// If version 1.1 and no Host header => 400 Bad Request

// If request-target include authority component and Host have a value different to this component => 400 Bad Request

// If several Host header => 400 Bad Request

int checkHostHeader(tree_node* root) {
	if (checkVersion(root) == 1) {
		_Token* tok2 = searchTree(root, "Host");
		if (tok2 == NULL) {
			return 400;
		}
	}
	return 0;
}

char* getHost(tree_node* root) {
	tree_node* node_host = (tree_node*)searchTree(root, "uri_host")->node;
	return getElementValue(node_host, node_host->length_string);
}

bool isAccepted(tree_node* root, char* mime_type) {
	_Token* temp = searchTree(root, "header_field");
	bool isfirst = true;
	while (temp != NULL) {
		tree_node* node_head_field = (tree_node*)temp->node;
		_Token* node_token = searchTree(node_head_field, "field_name");
		if (node_token == NULL) {
			temp = temp->next;
			continue;
		}
		node_head_field = (tree_node*)node_token->node;
		char* field_name = getElementValue(node_head_field, node_head_field->length_string);
		if (strcasecmp(field_name, "Accept") == 0) {
			isfirst = false;
			node_head_field = (tree_node*)searchTree(temp, "field_value")->node;
			char* field_value = getElementValue(node_head_field, node_head_field->length_string);
			if (isin(mime_type, strtok(field_value, ',;'))) {
				return true;
			}
		}
		temp = temp->next;
	}
	return isfirst;
}

bool keepAlive(tree_node* root){
	// Function send true if the connection is keep-alive, false otherwise
	tree_node* node_connection = searchTree(root, "Connection");
	tree_node* node_proxy_connection = searchTree(root, "Proxy-Connection");
	// Si Connection, on regarde la value de Connection
	if (node_connection != NULL){
		char* connection = getElementValue(node_connection, node_connection->length_string);
		return strcasecmp(connection, "keep-alive") == 0;
	// Sinon si Proxy-Connection, on regarde la value de Proxy-Connection
	} else if (node_proxy_connection != NULL){
		char* proxy_connection = getElementValue(node_proxy_connection, node_proxy_connection->length_string);
		return strcasecmp(proxy_connection, "keep-alive") == 0;
	// Sinon on regarde la version HTTP pour le comportement par defaut
	} else if (checkVersion(root) == 0){
		return false;
	} else {
		return true;
	}
}