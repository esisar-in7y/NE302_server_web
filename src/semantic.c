#include "semantic.h"

int getstatus(tree_node* root, _headers_request* header_req) {
	//TODO mettre des fonctions
	// On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
	populate_method(root, header_req);
	if (header_req->methode == NULL) {
		return 501;
	}

	populate_version(root, header_req);
	// On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
	if (header_req->version == NULL) {
		return 505;
	}

	populate_transfer_encoding(root, header_req);
	// On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
	if (header_req->transfert_encoding != NULL) {
		if (header_req->version == HTTP1_0) {
			return 400;
		}
		if (strcasecmp(header_req->transfert_encoding, "chunked") != 0 && ) {
			return 501;
		}
	}

	if (t != NULL) {
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


int checkVersion(tree_node* root, _headers_request* header_req) {
	populate_version(root,	header_req);
	return header_req->version;
}


int checkConnection(tree_node* root, _headers_request* header_req) {
	populate_connection(root,header_req);
	populate_content_length(root,header_req);
	populate_transfer_encoding(root,header_req);

	if (checkVersion(root, header_req) == HTTP1_1) {
		if (header_req->connection!=0) {
			if (header_req->transfert_encoding == NULL && header_req->content_length == NULL) {
				return 400;
			}
			header_req->connection=1;
		}
	} else if (checkVersion(root, header_req) == HTTP1_0) {
		if (header_req->connection==1) {
			if (searchTree(root, "Transfer-Encoding") == NULL && searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}else{
			header_req->connection=0;
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

int checkHostHeader(tree_node* root, _headers_request* header_req) {
	populate_version(root,header_req);
	populate_host(root,header_req);
	if (header_req->version == HTTP1_1) {
		if (searchTree(root, "Host") == NULL) {
			return 400;
		}
	}
	return 0;
}

char* getHost(tree_node* root) {
	return get_first_value(root,"uri_host");
}

//? ca marche donc voila strcasestr(field_value,mime_type)
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
			char* token;
			//! aucune idée si ca marche mais ca devrait
			if(have_separators(field_value,mime_type)){
				return true;
			}
		}
		temp = temp->next;
	}
	return isfirst;
}

bool keepAlive(tree_node* root) {
	// Function send true if the connection is keep-alive, false otherwise
	_Token* node_connection_token = (_Token*)searchTree(root, "connection_option");	 //"connection_option");
	// Si Connection, on regarde la value de Connection
	if (node_connection_token != NULL) {
		tree_node* node_connection = node_connection_token->node;
		char* connection = getElementValue(node_connection, node_connection->length_string);
		return (strcasecmp(connection, "keep-alive")==0) || (strcasecmp(connection, "keepalive") == 0);
	} else if (checkVersion(root) == 0) {
		return false;
	} else {
		return true;
	}
}

