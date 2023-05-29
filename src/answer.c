#include "answer.h"
#include <unistd.h>
#include "../utils/manip.h"
#include "../utils/mime.h"
#include "api.h"
#include "semantic.h"
#define SHOWHEAD	0
#define BUFFER_SIZE 1024

void send_status(int status, int clientId) {
	char status_char[5];
	sprintf(status_char, "%d ", status);
	writeDirectClient(clientId, status_char, 4);
	// writeDirectClient(clientId," OK\r\n",SHOWHEAD?7:5);
	switch (status) {
	case 100: writeClient(clientId, "Continue"); break;
	case 101: writeClient(clientId, "Switching Protocols"); break;
	case 102: writeClient(clientId, "Processing"); break;
	case 103: writeClient(clientId, "Early Hints"); break;
	case 200: writeClient(clientId, "OK"); break;
	case 201: writeClient(clientId, "Created"); break;
	case 202: writeClient(clientId, "Accepted"); break;
	case 203: writeClient(clientId, "Non-Authoritative Information"); break;
	case 204: writeClient(clientId, "No Content"); break;
	case 205: writeClient(clientId, "Reset Content"); break;
	case 206: writeClient(clientId, "Partial Content"); break;
	case 207: writeClient(clientId, "Multi-Status"); break;
	case 208: writeClient(clientId, "Already Reported"); break;
	case 226: writeClient(clientId, "IM Used"); break;
	case 300: writeClient(clientId, "Multiple Choices"); break;
	case 301: writeClient(clientId, "Moved Permanently"); break;
	case 302: writeClient(clientId, "Found"); break;
	case 303: writeClient(clientId, "See Other"); break;
	case 304: writeClient(clientId, "Not Modified"); break;
	case 305: writeClient(clientId, "Use Proxy"); break;
	case 306: writeClient(clientId, "Switch Proxy"); break;
	case 307: writeClient(clientId, "Temporary Redirect"); break;
	case 308: writeClient(clientId, "Permanent Redirect"); break;
	case 400: writeClient(clientId, "Bad Request"); break;
	case 401: writeClient(clientId, "Unauthorized"); break;
	case 402: writeClient(clientId, "Payment Required"); break;
	case 403: writeClient(clientId, "Forbidden"); break;
	case 404: writeClient(clientId, "Not Found"); break;
	case 405: writeClient(clientId, "Method Not Allowed"); break;
	case 406: writeClient(clientId, "Not Acceptable"); break;
	case 407: writeClient(clientId, "Proxy Authentication Required"); break;
	case 408: writeClient(clientId, "Request Timeout"); break;
	case 409: writeClient(clientId, "Conflict"); break;
	case 410: writeClient(clientId, "Gone"); break;
	case 411: writeClient(clientId, "Length Required"); break;
	case 412: writeClient(clientId, "Precondition Failed"); break;
	case 413: writeClient(clientId, "Payload Too Large"); break;
	case 414: writeClient(clientId, "URI Too Long"); break;
	case 415: writeClient(clientId, "Unsupported Media Type"); break;
	case 416: writeClient(clientId, "Range Not Satisfiable"); break;
	case 417: writeClient(clientId, "Expectation Failed"); break;
	case 418: writeClient(clientId, "I'm a teapot"); break;
	case 421: writeClient(clientId, "Misdirected Request"); break;
	case 422: writeClient(clientId, "Unprocessable Entity"); break;
	case 423: writeClient(clientId, "Locked"); break;
	case 424: writeClient(clientId, "Failed Dependency"); break;
	case 426: writeClient(clientId, "Upgrade Required"); break;
	case 428: writeClient(clientId, "Precondition Required"); break;
	case 429: writeClient(clientId, "Too Many Requests"); break;
	case 431: writeClient(clientId, "Request Header Fields Too Large"); break;
	case 451: writeClient(clientId, "Unavailable For Legal Reasons"); break;
	case 500: writeClient(clientId, "Internal Server Error"); break;
	case 501: writeClient(clientId, "Not Implemented"); break;
	case 502: writeClient(clientId, "Bad Gateway"); break;
	case 503: writeClient(clientId, "Service Unavailable"); break;
	case 504: writeClient(clientId, "Gateway Timeout"); break;
	case 505: writeClient(clientId, "HTTP Version Not Supported"); break;
	case 506: writeClient(clientId, "Variant Also Negotiates"); break;
	case 507: writeClient(clientId, "Insufficient Storage"); break;
	case 508: writeClient(clientId, "Loop Detected"); break;
	case 510: writeClient(clientId, "Not Extended"); break;
	case 511: writeClient(clientId, "Network Authentication Required"); break;
	}
	writeDirectClient(clientId, "\r\n",2);
	if (SHOWHEAD) {
		writeDirectClient(clientId, "\r\n", 2);
	}
	printf("answer back\n");
}

char* beautify_url(tree_node* root, _headers_request* headers_request) {
	tree_node* node = (tree_node*)searchTree(root, "request_target")->node;
	char* url = getElementValue(node, node->length_string);
	url = remove_dot_segments(url_decode(url));
	char* host = headers_request->host;
	char* url2;
	
	if (host != NULL) {
		url2=malloc(strlen(url) + 5 + strlen(host) + 10);
		strcpy(url2, "www");
		if (strcmp(host, "localhost") != 0 && strcmp(host, "127.0.0.1") != 0) {
			strcat(url2, "/");
			strcat(url2, host);
		}
	}else{
		url2=malloc(strlen(url) + 5 + 10);
	}
	strcat(url2, url);
	if (url2[strlen(url2) - 1] == '/') {
		strcat(url2, "index.html");
	}

	return url2;
}

void answerback(tree_node* root, _headers_request* headers_request, _Reponse* reponse) {
	// check if it's a GET request
	tree_node* node = (tree_node*)searchTree(root, "method")->node;
	char* method = getElementValue(node, node->length_string);
	if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) {
		// check the query url
		char* url = beautify_url(root, );
		printf("File path: %s\n", url);
		// check if the file exists

		if (access(url, F_OK) == 0) {
			//!!! ATTENTION !!! could be just folders
			send_status(200, clientId);
			if(keepAlive(root)){
				writeDirectClient(clientId,"Connection: keep-alive\r\n",24);
			}else{
				writeDirectClient(clientId,"Connection: close\r\n",19);
			}
			// get the mime type
			char* mime_type = (char*)get_mime_type(url);
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
			int file_size = get_file_size(url);
			printf("file size: %d\n", file_size);
			char file_size_str[10];
			sprintf(file_size_str, "%d", file_size);
			writeDirectClient(clientId, "Content-Length: ", 16);
			writeDirectClient(clientId, file_size_str, strlen(file_size_str));
			writeDirectClient(clientId, "\r\n", 2);
			// send file content by pack of 1024 bytes
			writeDirectClient(clientId, "\r\n", 2);
			if (strcmp(method, "HEAD") == 0) {
				// return send_end(clientId);
				return;
			}
			char buffer[BUFFER_SIZE] = {0};
			FILE* file = fopen(url, "rb");
			int buffer_size = 0;
			while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
				writeDirectClient(clientId, buffer, buffer_size);
			}
			fclose(file);
		} else {
			send_status(404, clientId);
			// send_end(clientId);
			writeDirectClient(clientId,"\r\n\r\n",4);
		}
	}
	// send_end(clientId);
}

void populateRespFromReq(_headers_request* headers_request,_Reponse* response){
	// Populate connection field
	response->headers_response.connection=headers_request->connection;
	
}

// Send a chunked body
void sendChunkedBody(FILE * file, int clientId){
	char buffer[BUFFER_SIZE] = {0};
	int buffer_size = 0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		char size[10];
		sprintf(size,"%x\r\n",buffer_size);
		writeDirectClient(clientId,size,strlen(size));
		writeDirectClient(clientId,buffer,buffer_size);
		writeDirectClient(clientId,"\r\n",2);
	}
	writeDirectClient(clientId,"0\r\n\r\n",5);
}
