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
		char* host = getHost(root);
		printf("Host: %s\n", host);
		char* url2 = malloc(strlen(url) + 5 + strlen(host)+10);
		strcpy(url2, "www");
		if(strcmp(host, "localhost") != 0 && strcmp(host, "127.0.0.1") != 0){
			strcat(url2, "/");
			strcat(url2, host);
		}
		strcat(url2, url);
		if(url2[strlen(url2)-1] == '/'){
			strcat(url2, "index.html");
		}
		printf("File path: %s\n", url2);
		// check if the file exists
		//! hum
		writeDirectClient(clientId, "Content-Type: text/html\r\n\r\n", 27);
		return writeDirectClient(clientId, aaaaaaaaaaaaaaa(), strlen(aaaaaaaaaaaaaaa()));

		if(access(url2, F_OK) == 0){
			//!!! ATTENTION !!! could be just folders
			send_status(200,clientId);
			// get the mime type
			char* mime_type = (char*) get_mime_type(url2);
			// if(!isAccepted(root, mime_type)){
			// 	send_status(406,clientId);
			// 	send_end(clientId);
			// 	return;
			// }
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
				// return send_end(clientId);
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
	// send_end(clientId);
}
void send_status(int status, int clientId){
	char status_char[10];
	sprintf(status_char, "%d", status);
	writeDirectClient(clientId,status_char,3);
	// writeDirectClient(clientId," OK\r\n\r\n",SHOWHEAD?7:5);
	switch(status){
		case 200:
		writeDirectClient(clientId,"<html><body><h1>200 OK</h1></body></html>",SHOWHEAD?40:0);
		break;
		case 201:
		writeDirectClient(clientId,"<html><body><h1>201 Created</h1></body></html>",SHOWHEAD?45:0);
		break;
		case 202:
		writeDirectClient(clientId,"<html><body><h1>202 Accepted</h1></body></html>",SHOWHEAD?46:0);
		break;
		case 203:
		writeDirectClient(clientId,"<html><body><h1>203 Non-Authoritative Information</h1></body></html>",SHOWHEAD?69:0);
		break;
		case 204:
		writeDirectClient(clientId,"<html><body><h1>204 No Content</h1></body></html>",SHOWHEAD?50:0);
		break;
		case 205:
		writeDirectClient(clientId,"<html><body><h1>205 Reset Content</h1></body></html>",SHOWHEAD?53:0);
		break;
		case 206:
		writeDirectClient(clientId,"<html><body><h1>206 Partial Content</h1></body></html>",SHOWHEAD?55:0);
		break;
		case 300:
		writeDirectClient(clientId,"<html><body><h1>300 Multiple Choices</h1></body></html>",SHOWHEAD?56:0);
		break;
		case 301:
		writeDirectClient(clientId,"<html><body><h1>301 Moved Permanently</h1></body></html>",SHOWHEAD?59:0);
		break;
		case 302:
		writeDirectClient(clientId,"<html><body><h1>302 Found</h1></body></html>",SHOWHEAD?45:0);
		break;
		case 303:
		writeDirectClient(clientId,"<html><body><h1>303 See Other</h1></body></html>",SHOWHEAD?48:0);
		break;
		case 304:
		writeDirectClient(clientId,"<html><body><h1>304 Not Modified</h1></body></html>",SHOWHEAD?50:0);
		break;
		case 305:
		writeDirectClient(clientId,"<html><body><h1>305 Use Proxy</h1></body></html>",SHOWHEAD?47:0);
		break;
		case 307:
		writeDirectClient(clientId,"<html><body><h1>307 Temporary Redirect</h1></body></html>",SHOWHEAD?60:0);
		break;
		case 400:
		writeDirectClient(clientId,"<html><body><h1>400 Bad Request</h1></body></html>",SHOWHEAD?47:0);
		break;
		case 401:
		writeDirectClient(clientId,"<html><body><h1>401 Unauthorized</h1></body></html>",SHOWHEAD?49:0);
		break;
		case 402:
		writeDirectClient(clientId,"<html><body><h1>402 Payment Required</h1></body></html>",SHOWHEAD?52:0);
		break;
		case 403:
		writeDirectClient(clientId,"<html><body><h1>403 Forbidden</h1></body></html>",SHOWHEAD?44:0);
		break;
		case 404:
		writeDirectClient(clientId,"<html><body><h1>404 Not Found</h1></body></html>",SHOWHEAD?45:0);
		break;
		case 405:
		writeDirectClient(clientId,"<html><body><h1>405 Method Not Allowed</h1></body></html>",SHOWHEAD?56:0);
		break;
		case 406:
		writeDirectClient(clientId,"<html><body><h1>406 Not Acceptable</h1></body></html>",SHOWHEAD?49:0);
		break;
		case 407:
		writeDirectClient(clientId,"<html><body><h1>407 Proxy Authentication Required</h1></body></html>",SHOWHEAD?70:0);
		break;
		case 408:
		writeDirectClient(clientId,"<html><body><h1>408 Request Time-out</h1></body></html>",SHOWHEAD?52:0);
		break;
		case 409:
		writeDirectClient(clientId,"<html><body><h1>409 Conflict</h1></body></html>",SHOWHEAD?42:0);
		break;
		case 410:
		writeDirectClient(clientId,"<html><body><h1>410 Gone</h1></body></html>",SHOWHEAD?38:0);
		break;
		case 411:
		writeDirectClient(clientId,"<html><body><h1>411 Length Required</h1></body></html>",SHOWHEAD?50:0);
		break;
		case 412:
		writeDirectClient(clientId,"<html><body><h1>412 Precondition Failed</h1></body></html>",SHOWHEAD?57:0);
		break;
		case 413:
		writeDirectClient(clientId,"<html><body><h1>413 Request Entity Too Large</h1></body></html>",SHOWHEAD?65:0);
		break;
		case 414:
		writeDirectClient(clientId,"<html><body><h1>414 Request-URI Too Large</h1></body></html>",SHOWHEAD?61:0);
		break;
		case 415:
		writeDirectClient(clientId,"<html><body><h1>415 Unsupported Media Type</h1></body></html>",SHOWHEAD?60:0);
		break;
		case 416:
		writeDirectClient(clientId,"<html><body><h1>416 Requested range not satisfiable</h1></body></html>",SHOWHEAD?73:0);
		break;
		case 417:
		writeDirectClient(clientId,"<html><body><h1>417 Expectation Failed</h1></body></html>",SHOWHEAD?54:0);
		break;
		case 500:
		writeDirectClient(clientId,"<html><body><h1>500 Internal Server Error</h1></body></html>",SHOWHEAD?60:0);
		break;
		case 501:
		writeDirectClient(clientId,"<html><body><h1>501 Not Implemented</h1></body></html>",SHOWHEAD?52:0);
		break;
		case 502:
		writeDirectClient(clientId,"<html><body><h1>502 Bad Gateway</h1></body></html>",SHOWHEAD?44:0);
		break;
		case 503:
		writeDirectClient(clientId,"<html><body><h1>503 Service Unavailable</h1></body></html>",SHOWHEAD?54:0);
		break;
		case 504:
		writeDirectClient(clientId,"<html><body><h1>504 Gateway Time-out</h1></body></html>",SHOWHEAD?52:0);
		break;
		case 505:
		writeDirectClient(clientId,"<html><body><h1>505 HTTP Version not supported</h1></body></html>",SHOWHEAD?66:0);
		break;
	}
	printf("answer back\n");
}
//TODO connection keep alive
// ceci decrit la methode experimentale de renvoi de la reponse Cf API
#define false 0
#if TST==0
int main(int argc, char *argv[])
{
	message *requete; 
	printf("Serveur HTTP demarre sur le port %d\n",PORT);

	while (1) {
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
			endWriteDirectClient(requete->clientId);
			requestShutdownSocket(requete->clientId); 
		}else{
			printf("get root\n");
			tree_node* root = (tree_node*) getRootTree();
			// tree_node_print_all(root,0);
			printf("popu resp\n");
			status=checkSemantics(root);
			if(status>0){
				send_status(status,requete->clientId);
				send_end(requete->clientId);
				// Gérer le header connection pour savoir si on garder la connexion ouverte ou non
				endWriteDirectClient(requete->clientId);
				requestShutdownSocket(requete->clientId);
			}else{
				send_status(200,requete->clientId);
				answerback(root,status,requete->clientId);
				// Fermer la connexion avec le client
				endWriteDirectClient(requete->clientId);
				if (!keepAlive(root)){
					#ifdef DEBUG
						debug("Not keep alive", __LINE__);
					#endif
					requestShutdownSocket(requete->clientId); 
				}else{
					#ifdef DEBUG
						debug("Keep alive", __LINE__);
					#endif
				}
			}
		}
		
		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer... 
		freeRequest(requete);
	}
	return (1);
}
#endif
// echo -ne 'GET /sites/www.fake.com/index.html HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0\r\n\r\n' | ncat -C --hex-dump out localhost 8101
// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n' | ncat -C --hex-dump out localhost 8000
// curl -Iv http://localhost:8000/www.toto.com/index.htm 2>&1 | grep -i 'connection #0'
