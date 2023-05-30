#include "answer.h"
#include <unistd.h>

#include "../utils/manip.h"
#include "../utils/mime.h"
#include "../utils/structures.h"
#include "api.h"
#include "semantic.h"
#define SHOWHEAD	0
#define BUFFER_SIZE 1024
#define BIG_BUFFER_SIZE 5*BUFFER_SIZE

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
	int sum=0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {//! ca pete lorsque fichier trop gros (2Mio)
		char size_str[30]={0};
		snprintf(size_str, sizeof(size_str), "%x\r\n", buffer_size);
		sum+=buffer_size;
		printf("sum: %d size : %s\n",sum, size_str);
		writeDirectClient(clientId, size_str,strlen(size_str));
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
			long int file_size=0;
			if (response->headers_response.transfert_encoding != CHUNKED) {
				file_size = get_file_size(url);
				printf("file size: %ld\n", file_size);
				response->headers_response.content_length = (long int*)calloc(1,sizeof(long int*));
				*response->headers_response.content_length = file_size-1;
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
				response->headers_response.range=malloc(sizeof(_Range));
				response->headers_response.range->size=file_size;

				if(start==-1){
					start = 0;
				}else if(start<0){
					response->headers_response.status_code = 416;
					response->headers_response.range->start=-1;
					response->headers_response.range->end=-1;
					return false;
				}
				if(end==-1){
					end = MIN(start+300*BUFFER_SIZE,file_size);
				}else if(end>file_size){
					response->headers_response.status_code = 416;
					response->headers_response.range->start=-1;
					response->headers_response.range->end=-1;
					return false;
				}
				response->headers_response.range->start=start;
				response->headers_response.range->end=end;
				if(response->headers_response.content_length==NULL){
					response->headers_response.content_length=(long*)malloc(sizeof(long));
				}
				*response->headers_response.content_length=end-start+1;
				printf("ranges:%d-%d\n",start,end);
				send_headers(response);
				//TODO mettre ca dans la fonction send identity
				fseek(file, start, SEEK_SET);
				unsigned long waiting_bytes = *response->headers_response.content_length;
				printf("size:%ld\n",waiting_bytes);
				char* stream_buffer=(char *)malloc(BIG_BUFFER_SIZE);
				size_t buffer_size = 0;
				while (waiting_bytes>0 && (buffer_size = fread(stream_buffer,1,MIN(BIG_BUFFER_SIZE,waiting_bytes), file)) > 0) {
					writeDirectClient(clientId, stream_buffer, buffer_size);
					if(waiting_bytes>buffer_size){
						waiting_bytes -= buffer_size;
					}else{
						waiting_bytes = 0;
					}
				}
				free(stream_buffer);
				return true;
			}else{
				printf("No range\n");
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
	if(headers_request->ranges!=NULL){
		response->headers_response.transfert_encoding = IDENTITY;
	} else if (headers_request->accept_encoding.IDENTITY == true) {
		response->headers_response.transfert_encoding = IDENTITY;
	} else if (headers_request->version == HTTP1_1 && headers_request->accept_encoding.CHUNKED == false) {
		response->headers_response.transfert_encoding = CHUNKED;
	} else {
		response->headers_response.transfert_encoding = IDENTITY;
	}
#endif
	//
}
