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
	case 100: writeDirectClient(clientId, "Continue\r\n", 10); break;
	case 101: writeDirectClient(clientId, "Switching Protocols\r\n", 21); break;
	case 102: writeDirectClient(clientId, "Processing\r\n", 12); break;
	case 103: writeDirectClient(clientId, "Early Hints\r\n", 13); break;
	case 200: writeDirectClient(clientId, "OK\r\n", 4); break;
	case 201: writeDirectClient(clientId, "Created\r\n", 9); break;
	case 202: writeDirectClient(clientId, "Accepted\r\n", 10); break;
	case 203: writeDirectClient(clientId, "Non-Authoritative Information\r\n", 31); break;
	case 204: writeDirectClient(clientId, "No Content\r\n", 12); break;
	case 205: writeDirectClient(clientId, "Reset Content\r\n", 15); break;
	case 206: writeDirectClient(clientId, "Partial Content\r\n", 17); break;
	case 207: writeDirectClient(clientId, "Multi-Status\r\n", 14); break;
	case 208: writeDirectClient(clientId, "Already Reported\r\n", 18); break;
	case 226: writeDirectClient(clientId, "IM Used\r\n", 9); break;
	case 300: writeDirectClient(clientId, "Multiple Choices\r\n", 18); break;
	case 301: writeDirectClient(clientId, "Moved Permanently\r\n", 19); break;
	case 302: writeDirectClient(clientId, "Found\r\n", 7); break;
	case 303: writeDirectClient(clientId, "See Other\r\n", 11); break;
	case 304: writeDirectClient(clientId, "Not Modified\r\n", 14); break;
	case 305: writeDirectClient(clientId, "Use Proxy\r\n", 11); break;
	case 306: writeDirectClient(clientId, "Switch Proxy\r\n", 14); break;
	case 307: writeDirectClient(clientId, "Temporary Redirect\r\n", 20); break;
	case 308: writeDirectClient(clientId, "Permanent Redirect\r\n", 20); break;
	case 400: writeDirectClient(clientId, "Bad Request\r\n", 13); break;
	case 401: writeDirectClient(clientId, "Unauthorized\r\n", 14); break;
	case 402: writeDirectClient(clientId, "Payment Required\r\n", 18); break;
	case 403: writeDirectClient(clientId, "Forbidden\r\n", 11); break;
	case 404: writeDirectClient(clientId, "Not Found\r\n", 11); break;
	case 405: writeDirectClient(clientId, "Method Not Allowed\r\n", 20); break;
	case 406: writeDirectClient(clientId, "Not Acceptable\r\n", 16); break;
	case 407: writeDirectClient(clientId, "Proxy Authentication Required\r\n", 31); break;
	case 408: writeDirectClient(clientId, "Request Timeout\r\n", 17); break;
	case 409: writeDirectClient(clientId, "Conflict\r\n", 10); break;
	case 410: writeDirectClient(clientId, "Gone\r\n", 6); break;
	case 411: writeDirectClient(clientId, "Length Required\r\n", 17); break;
	case 412: writeDirectClient(clientId, "Precondition Failed\r\n", 21); break;
	case 413: writeDirectClient(clientId, "Payload Too Large\r\n", 19); break;
	case 414: writeDirectClient(clientId, "URI Too Long\r\n", 14); break;
	case 415: writeDirectClient(clientId, "Unsupported Media Type\r\n", 24); break;
	case 416: writeDirectClient(clientId, "Range Not Satisfiable\r\n", 23); break;
	case 417: writeDirectClient(clientId, "Expectation Failed\r\n", 20); break;
	case 418: writeDirectClient(clientId, "I'm a teapot\r\n", 14); break;
	case 421: writeDirectClient(clientId, "Misdirected Request\r\n", 21); break;
	case 422: writeDirectClient(clientId, "Unprocessable Entity\r\n", 22); break;
	case 423: writeDirectClient(clientId, "Locked\r\n", 8); break;
	case 424: writeDirectClient(clientId, "Failed Dependency\r\n", 19); break;
	case 426: writeDirectClient(clientId, "Upgrade Required\r\n", 18); break;
	case 428: writeDirectClient(clientId, "Precondition Required\r\n", 23); break;
	case 429: writeDirectClient(clientId, "Too Many Requests\r\n", 19); break;
	case 431: writeDirectClient(clientId, "Request Header Fields Too Large\r\n", 33); break;
	case 451: writeDirectClient(clientId, "Unavailable For Legal Reasons\r\n", 31); break;
	case 500: writeDirectClient(clientId, "Internal Server Error\r\n", 23); break;
	case 501: writeDirectClient(clientId, "Not Implemented\r\n", 17); break;
	case 502: writeDirectClient(clientId, "Bad Gateway\r\n", 13); break;
	case 503: writeDirectClient(clientId, "Service Unavailable\r\n", 21); break;
	case 504: writeDirectClient(clientId, "Gateway Timeout\r\n", 17); break;
	case 505: writeDirectClient(clientId, "HTTP Version Not Supported\r\n", 28); break;
	case 506: writeDirectClient(clientId, "Variant Also Negotiates\r\n", 26); break;
	case 507: writeDirectClient(clientId, "Insufficient Storage\r\n", 22); break;
	case 508: writeDirectClient(clientId, "Loop Detected\r\n", 15); break;
	case 510: writeDirectClient(clientId, "Not Extended\r\n", 14); break;
	case 511: writeDirectClient(clientId, "Network Authentication Required\r\n", 33); break;
	}
	if (SHOWHEAD) {
		writeDirectClient(clientId, "\r\n", 2);
	}
	printf("answer back\n");
}

char* beautify_url(tree_node* root) {
	tree_node* node = (tree_node*)searchTree(root, "request_target")->node;
	char* url = getElementValue(node, node->length_string);
	url = remove_dot_segments(url_decode(url));
	char* host = getHost(root);
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

void answerback(tree_node* root, int status, unsigned int clientId) {
	// check if it's a GET request
	tree_node* node = (tree_node*)searchTree(root, "method")->node;
	char* method = getElementValue(node, node->length_string);
	if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) {
		// check the query url
		char* url = beautify_url(root);
		printf("File path: %s\n", url);
		// check if the file exists

		if (access(url, F_OK) == 0) {
			//!!! ATTENTION !!! could be just folders
			send_status(200, clientId);
			if(keepAlive(root)){
				writeDirectClient(clientId,"Connection: keep-alive\r\n",24);
				writeDirectClient(clientId,"Keep-Alive: timeout=60, max=10\r\n",32);
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