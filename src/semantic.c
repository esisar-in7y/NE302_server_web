#include "semantic.h"

int getstatus(tree_node* root, _headers_request* header_req) {
	// TODO mettre des fonctions
	//  On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
	int status=0;
	populate_method(root, header_req);
	if (header_req->methode == 0) {
		return 501;
	}

	// On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
	if ((status = checkVersion(root, header_req)) > 0) {
		return status;
	}

	// On check si le header Connection est présent et on vérifie sa sémantique
	if((status = checkConnection(root, header_req)) > 0){
		return status;
	}

	// On check si le header Host est présent et on vérifie sa sémantique
	if((status = checkHostHeader(root, header_req)) > 0){
		return status;
	}

	// On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
	if ((status = checkTransfertEncoding(root, header_req)) > 0) {
		return status;
	}


// cecic n'a aucun SEEEENSSSSSSSSSSSSSSSSSSSSSSSSSSS

	// On check si le header Accept-Encoding est présent et on vérifie sa sémantique
	if((status = checkAcceptEncoding(root, header_req)) > 0){
		return status;
	}

	// On check si le header Referer est présent et on vérifie sa sémantique
	if((status = checkReferer(root, header_req)) > 0){
		return status;
	}

	return 0;
}

int checkReferer(tree_node* root, _headers_request* header_req) {
	char* Referer = getFieldValueFromFieldName(root, "Referer");
	if (strchr(Referer, '#') != NULL || strchr(Referer, '@') != NULL) {
		return 400;
	}
	return 0;
}

int checkVersion(tree_node* root, _headers_request* header_req) {
	populate_version(root,	header_req);
	if(header_req->version == 0){
		return 505;
	}
	return 0;
}


int checkConnection(tree_node* root, _headers_request* header_req) {
	populate_connection(root,header_req);
	populate_content_length(root,header_req);
	populate_transfert_encoding(root,header_req);

	if (checkVersion(root, header_req) == HTTP1_1) {
		if (header_req->connection!=0) {
			if (header_req->transfert_encoding.initialized == false && header_req->content_length == NULL) {
				return 400;
			}
			header_req->connection=1;
		}
	} else if (checkVersion(root, header_req) == HTTP1_0) {
		if (header_req->connection==1) {
			if (searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}else{
			header_req->connection=0;
		}
	}
	return 0;
}

int checkTransfertEncoding(tree_node* root, _headers_request* header_req) {
	populate_transfert_encoding(root,header_req);
	if(header_req->transfert_encoding.isPresent){
		if(header_req->version == HTTP1_0){
			return 400;
		}
	}
	return 0;
}

// **Accept-encoding header**

int checkAcceptEncoding(tree_node* root, _headers_request* header_req) {
	populate_accept_encoding(root,header_req);
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
			//! aucune idée si ca marche mais ca devrait
			if(have_separators(field_value,mime_type)){
				return true;
			}
		}
		temp = temp->next;
	}
	return isfirst;
}


