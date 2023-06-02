#include "answer.h"
#include <unistd.h>
#include <zlib.h>

#include "../utils/manip.h"
#include "../utils/mime.h"
#include "../utils/structures.h"
#include "api.h"
#include "semantic.h"
#define SHOWHEAD	0
#define BUFFER_SIZE 1024
#define BIG_BUFFER_SIZE 10*BUFFER_SIZE
#define MAX_STREAM_SIZE 50*BIG_BUFFER_SIZE

void send_status(int status, int clientId) {
	char status_char[5];
	sprintf(status_char, "%d ", status);
	writeDirectClient(clientId, status_char, 4);
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
}

char* beautify_url(tree_node* root, _headers_request* headers_request) {
	char* url = get_first_value(root, "request_target");
	char* url2 = url_decode(url);
	better_free(url);
	char* url_updated = remove_dot_segments(url2);
	better_free(url2);
	populate_host(root,headers_request);
	char* host = headers_request->host;
	printf("host: %s\n", host);
	char* return_url = NULL;
	if (host != NULL) {
		return_url = calloc(1,strlen(url_updated) + 5 + strlen(host) + 10);
		strcpy(return_url, "www");
		if (strcmp(host, "localhost") != 0 && strcmp(host, "127.0.0.1") != 0) {
			strcat(return_url, "/");
			strcat(return_url, host);
		}
	} else {
		return_url = calloc(1,strlen(url_updated) + 5 + 10);
		strcpy(return_url, "www");
		if(url_updated[0]!='/'){
			strcpy(return_url, "/");
		}
	}
	strcat(return_url, url_updated);
	if (return_url[strlen(return_url) - 1] == '/') {
		strcat(return_url, "index.html");
	}
	better_free(url_updated);
	return return_url;
}
int compressToGzip(const char* input, int inputSize, char* output, int outputSize)
{
    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = (uInt)inputSize;
    zs.next_in = (Bytef *)input;
    zs.avail_out = (uInt)outputSize;
    zs.next_out = (Bytef *)output;

    // hard to believe they don't have a macro for gzip encoding, "Add 16" is the best thing zlib can do:
    // "Add 16 to windowBits to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper"
    deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
    deflate(&zs, Z_FINISH);
    deflateEnd(&zs);
    return zs.total_out;
}
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

int deflate_data(const char *input, size_t input_length, char **output, size_t *output_length) {
    int ret;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        return ret;
    }

    *output_length = deflateBound(&strm, input_length);
    *output = malloc(*output_length);
    if (*output == NULL) {
        (void)deflateEnd(&strm);
        return Z_MEM_ERROR;
    }

    strm.avail_in = input_length;
    strm.next_in = (Bytef *)input;
    strm.avail_out = *output_length;
    strm.next_out = (Bytef *)*output;

    ret = deflate(&strm, Z_FINISH);
    assert(ret != Z_STREAM_ERROR);

    *output_length = strm.total_out;

    (void)deflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


void sendDeflateBody(FILE* file, int clientId) {
	printf("deflate1\n");
    char* buffer = malloc(BUFFER_SIZE * sizeof(char));
    int buffer_size = 0;
    while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		printf("deflate\n");
        char* compressed_buffer = NULL;
        size_t compressed_size = 0;
        int ret = deflate_data(buffer, buffer_size, &compressed_buffer, &compressed_size);
        if (ret != Z_OK) {
            fprintf(stderr, "Error compressing data: %d\n", ret);
            free(compressed_buffer);
            break;
        }

        char size_str[30] = {0};
        snprintf(size_str, sizeof(size_str), "%zx\r\n", compressed_size);
        writeDirectClient(clientId, size_str, strlen(size_str));
        writeDirectClient(clientId, compressed_buffer, compressed_size);
        writeDirectClient(clientId, "\r\n", 2);

        free(compressed_buffer);
    }
    writeDirectClient(clientId, "0\r\n\r\n", 5);
    free(buffer);
}


void sendGzipBody(FILE* file, int clientId) {
	char* buffer = malloc(BUFFER_SIZE * sizeof(char));
	char* compressed = malloc(BUFFER_SIZE * sizeof(char));
	int compressed_size = 0;
	int buffer_size = 0;

	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		compressed_size = compressToGzip(buffer, buffer_size, compressed, BUFFER_SIZE);
		char size_str[30] = {0};
		snprintf(size_str, sizeof(size_str), "%x\r\n", compressed_size);
		writeDirectClient(clientId, size_str, strlen(size_str));
		writeDirectClient(clientId, compressed, compressed_size);
		writeDirectClient(clientId, "\r\n", 2);
	}
	writeDirectClient(clientId, "0\r\n\r\n", 5);
	free(compressed);
	free(buffer);
}

// Send a chunked body
void sendChunkedBody(FILE* file, int clientId) {
	char* buffer=malloc(BUFFER_SIZE*sizeof(char));
	int buffer_size = 0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		char size_str[30]={0};
		snprintf(size_str, sizeof(size_str), "%x\r\n", buffer_size);
		writeDirectClient(clientId, size_str,strlen(size_str));
		writeDirectClient(clientId, buffer, buffer_size);
		writeDirectClient(clientId, "\r\n", 2);
	}
	writeDirectClient(clientId, "0\r\n\r\n", 5);
	free(buffer);
}

void sendIdentity(FILE* file, int clientId) {
	char* buffer = malloc(BUFFER_SIZE*sizeof(char));
	int buffer_size = 0;
	while ((buffer_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		writeDirectClient(clientId, buffer, buffer_size);
	}
	free(buffer);
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
				free(url);
				return false;
			}
			// On rajoute content type
			response->headers_response.content_type = malloc((strlen(mime_type)+1)*sizeof(char));
			strcpy(response->headers_response.content_type, mime_type);
			// get the file size
			long int file_size = 0;
			FILE* file = fopen(url, "rb");
			if (file == NULL) {
				printf("Failed to open file: %s\n", url);
				free(url);
				return -1;
			}
			free(url);
			if (
				response->headers_response.transfert_encoding != CHUNKED &&
				response->headers_response.transfert_encoding != GZIP
			) {
				fseek(file, 0L, SEEK_END);
				file_size = ftell(file);
				printf("file size: %ld\n", file_size);
				response->headers_response.content_length = (long int*)calloc(1, sizeof(long int*));
				*response->headers_response.content_length = file_size;
			}

			if (response->headers_response.status_code == 0) {
				response->headers_response.status_code = 200;
			}
			if (headers_request->methode == HEAD) {
				fclose(file);
				return false;
			}
			if(headers_request->ranges!=NULL){
				response->headers_response.status_code = 206;
				//populate range response
				_Range* range= headers_request->ranges->range;
				int start = range->start;
				int end = range->end;
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
					end = MIN(start+MAX_STREAM_SIZE,file_size);
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
				fclose(file);
				return true;
			}else{
				//send headers
				send_headers(response);
				// send data
				if (response->headers_response.transfert_encoding == CHUNKED) {
					sendChunkedBody(file, clientId);
				} else if(response->headers_response.transfert_encoding == DEFLATE){
					sendDeflateBody(file, response->clientId);
				}else{
					sendIdentity(file, response->clientId);
				}
				fclose(file);
				return true;
			}
		} else {
			response->headers_response.status_code = 404;
			response->headers_response.connection = CLOSE;
		}
		free(url);
	} else if (headers_request->methode == POST) {
		response->headers_response.connection=CLOSE;
		writeDirectClient(response->clientId, "HTTP/1.0 ", 9);
		send_status(405, response->clientId);
		writeDirectClient(response->clientId,"\r\n",2);
		return true;
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
// printf("ranges:%d|%d|%d|%d\n",headers_request->ranges!=NULL,headers_request->accept_encoding.GZIP == true,headers_request->accept_encoding.IDENTITY == true,headers_request->version == HTTP1_1 && headers_request->accept_encoding.CHUNKED == false);
	if(headers_request->ranges!=NULL){
		response->headers_response.transfert_encoding = IDENTITY;
	} 
	else if(headers_request->accept_encoding.DEFLATE == true){
		response->headers_response.transfert_encoding = DEFLATE;
	}
	else if (headers_request->accept_encoding.IDENTITY == true) {
		response->headers_response.transfert_encoding = IDENTITY;
	} else if (headers_request->version == HTTP1_1 && headers_request->accept_encoding.CHUNKED == false) {
		response->headers_response.transfert_encoding = CHUNKED;
	} else {
		response->headers_response.transfert_encoding = IDENTITY;
	}
#endif
	//
}
