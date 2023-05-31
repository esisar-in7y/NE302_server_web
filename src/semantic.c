#include "semantic.h"

int getstatus(tree_node* root, _headers_request* header_req) {
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

	// On check si le header Referer est présent et on vérifie sa sémantique
	if((status = checkReferer(root, header_req)) > 0){
		return status;
	}
	// mettre tout les populates ici pour avoir ce qu'on veut apres
	populate_accept_encoding(root,header_req);
	populate_ranges(root,header_req);
	return 0;
}

int checkReferer(tree_node* root, _headers_request* header_req) {
	char* Referer = getFieldValueFromFieldName(root, "Referer");
	if (Referer !=NULL && (strchr(Referer, '#') != NULL || strchr(Referer, '@') != NULL)) {
		free(Referer);
		return 400;
	}
	free(Referer);
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

	if (header_req->version == HTTP1_1) {
		if (header_req->connection!=CLOSE) {
			if (header_req->transfert_encoding.initialized == false && header_req->content_length == NULL) {
				return 400;
			}
			header_req->connection=KEEP_ALIVE;
		}
	} else if (header_req->version == HTTP1_0) {
		if (header_req->connection==KEEP_ALIVE) {
			if (searchTree(root, "Content-Length") == NULL) {
				return 400;
			}
		}else{
			header_req->connection=CLOSE;
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

// **Host header**
// Overlap with Request-target for URI
// If version 1.1 and no Host header => 400 Bad Request
// If request-target include authority component and Host have a value different to this component => 400 Bad Request
// If several Host header => 400 Bad Request

int checkHostHeader(tree_node* root, _headers_request* header_req) {
	populate_version(root,header_req);
	populate_host(root,header_req);
	if (header_req->version == HTTP1_1) {
		if (header_req->host == NULL) {
			return 400;
		}
	}
	return 0;
}

//? ca devrait être bon
bool isAccepted(tree_node* root, char* mime_type) {
	char* accepted=getFieldValueFromFieldName(root, "Accept");
	// printf("accepted : %s\n",accepted);
	if(accepted==NULL) {
		free(accepted);
		return true;
	}
	if(have_separators(accepted,"*/*")){
		free(accepted);
		return true;
	}
	if(have_separators(accepted,mime_type)){
		free(accepted);
		return true;
	}
	char* wilded=copyStringUntilSlash(mime_type);
	strcat(wilded,"/*");
	if(have_separators(accepted,wilded)){
		free(accepted);
		free(wilded);
		return true;
	}
	free(accepted);
	free(wilded);
	return false;
}
