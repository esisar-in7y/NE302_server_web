#include "answer.h"
#include <unistd.h>

#include "../utils/manip.h"
#include "../utils/mime.h"
#include "../utils/structures.h"
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
	writeDirectClient(clientId, "\r\n", 2);
	if (SHOWHEAD) {
		writeDirectClient(clientId, "\r\n", 2);
	}
	printf("answer back\n");
}

char* beautify_url(tree_node* root, _headers_request* headers_request) {
	tree_node* node = (tree_node*)searchTree(root, "request_target")->node;
	char* url = getElementValue(node, (unsigned int*) (&node->length_string));
	url = remove_dot_segments(url_decode(url));
	char* host = headers_request->host;
	char* url2 = NULL;

	if (host != NULL) {
		url2 = calloc(1,strlen(url) + 5 + strlen(host) + 10);
		strcpy(url2, "www");
		if (strcmp(host, "localhost") != 0 && strcmp(host, "127.0.0.1") != 0) {
			strcat(url2, "/");
			strcat(url2, host);
		}
	} else {
		url2 = calloc(1,strlen(url) + 5 + 10);
		strcpy(url2, "www");
		if(url[0]!='/'){
			strcpy(url2, "/");
		}
	}
	strcat(url2, url);
	if (url2[strlen(url2) - 1] == '/') {
		strcat(url2, "index.html");
	}

	return url2;
}



// Send a chunked body
void sendChunkedBody(FILE* file, int clientId) {
	char buffer[BUFFER_SIZE]={0};
	int buffer_size = 0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		char size[30];
		snprintf(size, sizeof(size), "%x\r\n", buffer_size);
		writeClient(clientId, size);
		writeDirectClient(clientId, buffer, buffer_size);
		writeDirectClient(clientId, "\r\n", 2);
	}
	writeDirectClient(clientId, "0\r\n\r\n", 5);
}

void sendIdentity(FILE* file, int clientId) {
	char buffer[BUFFER_SIZE]={0};
	int buffer_size = 0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		writeDirectClient(clientId, buffer, buffer_size);
	}
}

bool send_data(tree_node* root, _headers_request* headers_request, _Response* response) {
	// check if it's a GET HEAD request
	if (headers_request->methode == GET || headers_request->methode == HEAD) {
		// check the query url
		char* url = beautify_url(root, headers_request);
		printf("File path: %s\n", url);
		// check if the file exists
		unsigned int clientId = response->clientId;
		if (access(url, F_OK) == 0) {
			//!!! ATTENTION !!! could be just folder

			// get the mime type
			char* mime_type = (char*)get_mime_type(url);
			if (!isAccepted(root, mime_type)) {
				response->headers_response.status_code = 415;
				return false;
			}
			// On rajoute content type
			response->headers_response.content_type=malloc(strlen(mime_type));
			strcpy(response->headers_response.content_type,mime_type);
			// get the file size
			if (response->headers_response.transfert_encoding != CHUNKED) {
				int file_size = get_file_size(url);
				printf("file size: %d\n", file_size);
				response->headers_response.content_length = (int*)calloc(1,sizeof(int*));
				*response->headers_response.content_length = file_size;
			}

			if (response->headers_response.status_code == 0) {
				response->headers_response.status_code = 200;
			}
			if (headers_request->methode == HEAD) {
				return false;
			}
			if(headers_request->ranges!=NULL){
				response->headers_response.status_code = 206;
				//populate range response
				_Range* range= headers_request->ranges->range;
				int start = range->start;
				int end = range->end;
				FILE* file=fopen(url,"rb");// TODO safe open
				if(start==-1){
					start = 0;
				}
				if(end==-1){
					fseek(file, 0L, SEEK_END);
					end = ftell(file);
				}

				response->headers_response.range=malloc(sizeof(_Range));
				response->headers_response.range->start=start;
				response->headers_response.range->end=end;
				send_headers(response);
				fseek(file, start, SEEK_SET);
				int size = end - start;
				char buffer[BUFFER_SIZE] = {0};
				int buffer_size = 0;
				while ((buffer_size = fread(buffer, 1, MIN(BUFFER_SIZE,size), file)) > 0) {
					writeDirectClient(clientId, buffer, buffer_size);
					size -= buffer_size;
					if (size <= 0) {
						break;
					}
				}
				return true;
				//get file size

			}else{
				//send headers
				send_headers(response);
				// send data
				FILE* file=fopen(url,"rb");
				if (response->headers_response.transfert_encoding == CHUNKED) {
					sendChunkedBody(file, clientId);
				} else {
					sendIdentity(file, response->clientId);
				}
				fclose(file);
				return true;
			}
		} else {
			response->headers_response.status_code = 404;
			response->headers_response.connection = CLOSE;
		}
	} else if (headers_request->methode == POST) {
	}
	// send_end(clientId);
	return false;
}

void send_response(_Response* response){
	send_headers(response);
	if(response->body!=NULL){
		writeClient(response->clientId,response->body);
	}
}

void populateRespFromReq(_headers_request* headers_request, _Response* response) {
	response->headers_response.version = headers_request->version;
	// Populate connection field
	response->headers_response.connection = headers_request->connection;
	// choose encoding
#if FORCE_IDENTITY == 1
	response->headers_response.transfert_encoding = IDENTITY;
#else
	if (headers_request->accept_encoding.IDENTITY == true) {
		response->headers_response.transfert_encoding = IDENTITY;
	} else if (headers_request->version == HTTP1_1 && headers_request->accept_encoding.CHUNKED == false) {
		response->headers_response.transfert_encoding = CHUNKED;
	} else {
		response->headers_response.transfert_encoding = IDENTITY;
	}
#endif
	//
}
