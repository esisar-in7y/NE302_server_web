#include "../utils/global.h"
#include "../utils/manip.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../lib/request.h"
#include "semantic.h"
#include "../utils/mime.h"
#include "api.h"   
#include "../utils/req.c"

#define SHOWHEAD 0
#define PORT 8000
#define BUFFER_SIZE 1024
void send_end(int clientId){
	writeDirectClient(clientId, "\r\n\r\n", 4);
}

void answerback(tree_node* root,int status,unsigned int clientId){
	// check if it's a GET request
	tree_node* node = (tree_node*) searchTree(root, "method")->node;
	char* method = getElementValue(node, node->length_string);
	if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) {
		// check the query url
		node = (tree_node*) searchTree(root, "request_target")->node;
		char* url = getElementValue(node, node->length_string);
		url = remove_dot_segments(url_decode(url));
		// to url: add www if / is the first char otherwise add www/
		char* url2 = malloc(strlen(url) + 4);
		strcpy(url2, "www");
		strcat(url2, url);
		printf("File path: %s\n", url2);
		// check if the file exists
		// writeDirectClient(clientId, "Content-Type: text/html\r\n\r\n", 27);return writeDirectClient(clientId, aaaaaaaaaaaaaaa(), strlen(aaaaaaaaaaaaaaa()));
		if(access(url2, F_OK) == 0){
			//!!! ATTENTION !!! could be just folders
			send_status(200,clientId);
			// get the mime type
			char* mime_type = (char*) get_mime_type(url2);
			printf("mime type: %s\n", mime_type);
			writeDirectClient(clientId, "Content-Type: ", 14);
			writeDirectClient(clientId, mime_type, strlen(mime_type));
			writeDirectClient(clientId, "\r\n", 2);
			// get the file size
			int file_size = get_file_size(url2);
			printf("file size: %d\n", file_size);
			char file_size_str[10];
			sprintf(file_size_str, "%d", file_size);
			writeDirectClient(clientId, "Content-Length: ", 16);
			writeDirectClient(clientId, file_size_str, strlen(file_size_str));
			writeDirectClient(clientId, "\r\n", 2);
			// send file content by pack of 1024 bytes
			writeDirectClient(clientId, "\r\n", 2);
			if(strcmp(method, "HEAD") == 0){
				return;
			}
			char buffer [BUFFER_SIZE]={0};
			FILE* file = fopen(url2, "rb");
			int buffer_size=0;
			while ((buffer_size=fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
				writeDirectClient(clientId, buffer, buffer_size);
			}
			fclose(file);
		}else{
			send_status(404,clientId);
			send_end(clientId);
		}
	}
}
void send_status(int status, int clientId){
	char status_char[10];
	sprintf(status_char, "%d", status);
	writeDirectClient(clientId,status_char,3);
	writeDirectClient(clientId," OK\r\n\r\n",SHOWHEAD?7:5);
	printf("answer back\n");
}

// ceci decrit la methode experimentale de renvoi de la reponse Cf API
#define false 0
#if TST==0
int main(int argc, char *argv[])
{
	message *requete; 
	printf("Serveur HTTP demarre sur le port %d\n",PORT);

	while ( 1 ) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest. 
		if ((requete=getRequest(PORT)) == NULL ) return -1; 

		// Affichage de debug 
		printf("#########################################\nDemande recue depuis le client %d\n",requete->clientId); 
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n",requete->len,requete->buf);
		char* answer="";
		int status=0;
		writeDirectClient(requete->clientId,"HTTP/1.0 ",9);
		if(parseur(requete->buf,requete->len)==0){
			send_status(400,requete->clientId);
		}else{
			printf("get root\n");
			tree_node* root = (tree_node*) getRootTree();
			// tree_node_print_all(root,0);
			printf("popu resp\n");
			status=checkSemantics(root);
			if(status>0){
				send_status(status,requete->clientId);
			}else{
				answerback(root,status,requete->clientId);
			}
		}
		endWriteDirectClient(requete->clientId);
		requestShutdownSocket(requete->clientId); 
		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer... 
		freeRequest(requete);
	}
	return (1);
}
#endif
// echo -ne 'GET /sites/www.fake.com/index.html HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0\r\n\r\n' | ncat -C --hex-dump out localhost 8101
