#include "../utils/global.h"

// Put parser here
#include "api.h"
/*
typedef struct _token {
	void *node;  		// node type opaque pointant vers un noeud de votre structure de données.
	struct _token *next; 	// next pointe vers le prochain token.
} _Token;
void add_token(_Token **head, void *node);
// Fonction qui retourne un pointeur (type opaque) vers la racine de l'arbre construit.
void *getRootTree();

// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.
// Par convention si start == NULL alors on commence à la racine
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start
_Token *searchTree(void *start,char *name);

// fonction qui renvoie un pointeur vers char indiquant l'etiquette du noeud. (le nom de la rulename, intermediaire ou terminal)
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementTag(void *node,int *len);

// fonction qui renvoie un pointeur vers char indiquant la valeur du noeud. (la partie correspondnant à la rulename dans la requete HTTP )
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementValue(void *node,int *len);

// Fonction qui supprime et libere la liste chainée de reponse.
void purgeElement(_Token **r);

// Fonction qui supprime et libere toute la mémoire associée à l'arbre .
void purgeTree(void *root);

// L'appel à votre parser un char* et une longueur à parser.
int parseur(char *req, int len); */
int main() {
	_Token* root = getRootTree();
	return checkSemantics(root);
}

int checkSemantics(_Token* root) {
	// On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
	_Token* t = searchTree(root, "method");
	tree_node* node = t->node;
	char* method = getElementValue(node, node->length_string);
	if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0 && strcmp(method, "POST") != 0) {
		return 501;
	}
	// On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
	t = searchTree(root, "HTTP_version");
	node = t->node;
	char* version = getElementValue(node, node->length_string);
	if (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1") != 0) {
		return 505;
	}

	// On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
	t = searchTree(root, "Transfer_Encoding_header");
	if (t != NULL) {
		// If 1.0 and Transfer-Encoding header => 400 Bad Request
		t = searchTree(root, "HTTP_version");
		node = t->node;
		char* version = getElementValue(node, node->length_string);
		if (strcmp(version, "HTTP/1.0") == 0) {
			return 400;
		}

		// If it's a transfer-coding it doesn't understand => 501 Not Implemented
		t = searchTree(root, "transfert_coding");
		// Parcourir la liste chainée de transfert_coding et vérifier que chaque élément est chunked gzip compress deflate
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = t->node;
			char* transferEncoding = getElementValue(node, node->length_string);
			if (strcmp(transferEncoding, "chunked") != 0 && strcmp(transferEncoding, "gzip") != 0 && strcmp(transferEncoding, "compress") != 0 && strcmp(transferEncoding, "deflate") != 0) {
				return 501;
			}
		}

		// If chunked more than once => 400 Bad Request
		int count = 0;
		for (_Token* current_token = t; current_token != NULL; current_token = current_token->next) {
			node = t->node;
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
		node = t->node;
		char* transferEncoding = getElementValue(node, node->length_string);
		if (strcmp(transferEncoding, "chunked") != 0) {
			return 400;
		}

        // Referer Header
        t = searchTree(root,"header_field");
        _Token* fieldName;
        while(t->next != NULL){
            fieldName = searchTree(t,"field_name");
            node = fieldName->node;
            if (strcmp(getElementValue(node,node->length_string),"Referer") == 0){
                 //If the URL contains a fragment (indicated by a '#' symbol) or userinfo (indicated by a '@' symbol) => 400 Bad Request
                t = searchTree(root,"field_value");
                node = t->node;
                char* absolutePath = getElementValue(node,node->length_string);
                if(strchr(absolutePath,'#') != NULL || strchr(absolutePath,'@') != NULL){
                    return 400;
                }
                break;
            }
            t = t->next;
        }
	}
}

int checkConnection(_Token* root) {
	_Token* tok = searchTree(root, "HTTP_version");
	_Token* tok2 = searchTree(root, "Connection");
	_Token* tok3 = searchTree(root, "Proxy-Connection");
	tree_node* node = tok->node;
	tree_node* node2 = tok2->node;
	tree_node* node3 = tok3->node;
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
	return 200;
}

// **Accept-encoding header**

// Verif one of the content codings listed is the representation's content coding (si q!=0)  or \*/\*else =>  415 Unsupported Media Type
// Without Accept-encoding => everything is considered as acceptable so do nothing
// If representation has no content coding => acceptable

int checkAcceptEncoding(_Token* root) {
	_Token* tok = searchTree(root, "HTTP_version");
	_Token* tok2 = searchTree(root, "Accept-Encoding");
	tree_node* node = tok->node;
	tree_node* node2 = tok2->node;
	char* version = getElementValue(node, node->length_string);
	char* acceptEncoding = getElementValue(node2, node2->length_string);
	if (acceptEncoding != NULL) {
		if (searchTree(root, "Content-Encoding") == NULL) {
			return 415;
		}
	}
	return 200;
}