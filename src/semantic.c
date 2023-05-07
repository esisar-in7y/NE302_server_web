#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

//Put parser here
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
int main(){
    _Token* root=getRootTree(); 
    return checkSemantics(root);
}


int checkSemantics(_Token *root){
    // On check METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented
    _Token* t = searchTree(root,"method");
    tree_node* node = t->node;
    char* method = getElementValue(node,node->length_string);
    if(strcmp(method,"GET") != 0 && strcmp(method,"HEAD") != 0 && strcmp(method,"POST") != 0){
        return 501;
    }
    // On check VERSION = HTTP/1.0 or HTTP/1.1 if not in VERSION => 505 HTTP Version Not Supported
    t = searchTree(root,"HTTP_version");
    node = t->node;
    char* version = getElementValue(node,node->length_string);
    if(strcmp(version,"HTTP/1.0") != 0 && strcmp(version,"HTTP/1.1") != 0){
        return 505;
    }
 

    // On check si le header Transfer-Encoding est présent et on vérifie sa sémantique
    t = searchTree(root,"Transfer_Encoding_header");
    if(t != NULL){        
        
        // If 1.0 and Transfer-Encoding header => 400 Bad Request
        t = searchTree(root,"HTTP_version");
        node = t->node;
        char* version = getElementValue(node,node->length_string);
        if(strcmp(version,"HTTP/1.0") == 0){
            return 400;
        }

        //If it's a transfer-coding it doesn't understand => 501 Not Implemented
        t = searchTree(root,"transfert_coding");
        // Parcourir la liste chainée de transfert_coding et vérifier que chaque élément est chunked gzip compress deflate
        for (_Token *current_token = t; current_token != NULL; current_token = current_token->next){
            node = t->node;
            char* transferEncoding = getElementValue(node,node->length_string);
            if(strcmp(transferEncoding,"chunked") != 0 && strcmp(transferEncoding,"gzip") != 0 && strcmp(transferEncoding,"compress") != 0 && strcmp(transferEncoding,"deflate") != 0){
                return 501;
            }
        }

        // If chunked more than once => 400 Bad Request
        int count = 0;
        for (_Token *current_token = t; current_token != NULL; current_token = current_token->next){
            node = t->node;
            char* transferEncoding = getElementValue(node,node->length_string);
            if(strcmp(transferEncoding,"chunked") == 0){
                count++;
            }
        }
        if(count > 1){
            return 400;
        }

        // If the last transfer coding is not chunked => 400 Bad Request
        t = searchTree(root,"transfert_coding");
        while(t->next != NULL){
            t = t->next;
        }
        node = t->node;
        char* transferEncoding = getElementValue(node,node->length_string);
        if(strcmp(transferEncoding,"chunked") != 0){
            return 400;
        }
   
        // Referer Header
        if  (searchTree(root,"Referer_header") != NULL){
            //If the URL contains a fragment (indicated by a '#' symbol) or userinfo (indicated by a '@' symbol) => 400 Bad Request
            
        }



    }
}