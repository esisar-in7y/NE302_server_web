#include "fastcgi.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../src/answer.h"
#include "../utils/tree.h"
// =========================================================================================================== //

size_t readSocket(int fd, char* buf, size_t len) {
	size_t readlen = 0;
	ssize_t nb = 0;

	if (len == 0) return 0;

	do {
		// try to read
		do {
			nb = read(fd, buf + readlen, len - readlen);
		} while (nb == -1 && errno == EINTR);
		if (nb > 0) readlen += nb;
	} while ((nb > 0) && (len != readlen));

	if (nb < 0) readlen = -1;
	return readlen;
}
//============================================================================================================ //

void readData(int fd, FCGI_Header* h, size_t* len) {
	size_t nb;
	*len = 0;

	nb = sizeof(FCGI_Header) - FASTCGILENGTH;
	if ((readSocket(fd, (char*)h, nb) == nb)) {
		h->requestId = htons(h->requestId);
		h->contentLength = htons(h->contentLength);
		*len += nb;
		nb = h->contentLength + h->paddingLength;
		if ((readSocket(fd, (char*)h->contentData, nb) == nb)) {
			*len += nb;
		} else {
			*len = 0;
		}
	}
}

// =========================================================================================================== //
void writeSocket(int fd, FCGI_Header* h, unsigned int len) {
	int w;

	h->contentLength = htons(h->contentLength);
	h->paddingLength = htons(h->paddingLength);

	while (len) {
		// try to write
		do {
			w = write(fd, h, len);
		} while (w == -1 && errno == EINTR);
		len -= w;
	}
}

// =========================================================================================================== //
void writeLen(int len, char** p) {
	if (len > 0x7F) {
		*((*p)++) = (len >> 24) & 0x7F;
		*((*p)++) = (len >> 16) & 0xFF;
		*((*p)++) = (len >> 8) & 0xFF;
		*((*p)++) = (len)&0xFF;
	} else
		*((*p)++) = (len)&0x7F;
}

// =========================================================================================================== //
int addNameValuePair(FCGI_Header* h, char* name, char* value) {
	char* p;
	unsigned long long nameLen = 0, valueLen = 0;

	if (name) nameLen = strlen(name);
	if (value) valueLen = strlen(value);

	if ((valueLen > FASTCGIMAXNVPAIR) || (valueLen > FASTCGIMAXNVPAIR)) return -1;
	if ((h->contentLength + ((nameLen > 0x7F) ? 4 : 1) + ((valueLen > 0x7F) ? 4 : 1)) > FASTCGILENGTH) return -1;

	p = (h->contentData) + h->contentLength;
	if (nameLen <= 0x7F) {
		*p++ = (char)nameLen;
	} else {
		*p++ = (char)((nameLen >> 24) | 0x80);
		*p++ = (char)(nameLen >> 16);
		*p++ = (char)(nameLen >> 8);
		*p++ = (char)nameLen;
	}
	if (valueLen <= 0x7F) {
		*p++ = (char)valueLen;
	} else {
		*p++ = (char)((valueLen >> 24) | 0x80);
		*p++ = (char)(valueLen >> 16);
		*p++ = (char)(valueLen >> 8);
		*p++ = (char)valueLen;
	}
	strncpy(p, name, nameLen);
	p += nameLen;
	if (value) strncpy(p, value, valueLen);
	h->contentLength += nameLen + ((nameLen > 0x7F) ? 4 : 1);
	h->contentLength += valueLen + ((valueLen > 0x7F) ? 4 : 1);
	return 0;
}

// =========================================================================================================== //

void sendGetValue(int fd) {
	FCGI_Header h={0};

	h.version = FCGI_VERSION_1;
	h.type = FCGI_GET_VALUES;
	h.requestId = htons(FCGI_NULL_REQUEST_ID);
	h.contentLength = 0;
	h.paddingLength = 0;
	addNameValuePair(&h, FCGI_MAX_CONNS, NULL);
	addNameValuePair(&h, FCGI_MAX_REQS, NULL);
	addNameValuePair(&h, FCGI_MPXS_CONNS, NULL);
	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));
}

// =========================================================================================================== //
void sendBeginRequest(int fd, unsigned short requestId, unsigned short role, unsigned char flags) {
	FCGI_Header h={0};
	FCGI_BeginRequestBody* begin;

	h.version = FCGI_VERSION_1;
	h.type = FCGI_BEGIN_REQUEST;
	h.requestId = htons(requestId);
	h.contentLength = sizeof(FCGI_BeginRequestBody);
	h.paddingLength = 0;
	begin = (FCGI_BeginRequestBody*)&(h.contentData);
	begin->role = htons(role);
	begin->flags = flags;
	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));
}
// =========================================================================================================== //
void sendAbortRequest(int fd, unsigned short requestId) {
	FCGI_Header h={0};

	h.version = FCGI_VERSION_1;
	h.type = htons(FCGI_ABORT_REQUEST);
	h.requestId = requestId;
	h.contentLength = 0;
	h.paddingLength = 0;
	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));
}
#define sendStdin(fd, id, stdin, len) sendWebData(fd, FCGI_STDIN, id, stdin, len)
#define sendData(fd, id, data, len)	  sendWebData(fd, FCGI_DATA, id, data, len)

//============================================================================================================ //

void sendWebData(int fd, unsigned char type, unsigned short requestId, char* data, unsigned int len) {
	FCGI_Header h={0};

	if (len > FASTCGILENGTH) return;

	h.version = FCGI_VERSION_1;
	h.type = type;
	h.requestId = htons(requestId);
	h.contentLength = len;
	h.paddingLength = 0;
	memcpy(h.contentData, data, len);
	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));
}

// =========================================================================================================== //
static int createSocket(int port) {
	int fd=0;
	struct sockaddr_in serv_addr={0};

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation failed\n");
		return (-1);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(port);

	if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed\n");
		return (-1);
	}

	return fd;
}
// =========================================================================================================== //

void send_direct_header_cgi(tree_node* root, FCGI_Header* h, char* header, char* query) {
	char* tmp;
	tmp = get_first_value(root, query);
	if (tmp != NULL) {
		addNameValuePair(h, header, tmp);
		printf("%s:%s\n", header, tmp);
		better_free(tmp);
	} else {
		printf("%s:NULL\n", header);
	}
}
void send_indirect_header_cgi(tree_node* root, FCGI_Header* h, char* header, char* query) {
	char* tmp;
	tmp = getFieldValueFromFieldName(root, query);
	if (tmp != NULL) {
		addNameValuePair(h, header, tmp);
		printf("%s:%s\n", header, tmp);
		better_free(tmp);
	} else {
		printf("%s:NULL\n", header);
	}
}

typedef struct response_data {
	char* data;
	size_t length;
	struct response_data* next;
} response_data;

size_t get_http_body_length(char* http_string, long len) {
	if (http_string == NULL) return -1;
	char* body_start = strstr(http_string, "\r\n\r\n");
	if (body_start == NULL) return -1;
	return len - (body_start - http_string) - 4;
}

void send_length(int contentLength, char* contentData, message* requete) {
	size_t total_length = get_http_body_length(contentData, contentLength);
	char* total_length_string = malloc(40);
	sprintf(total_length_string, "%ld", total_length);
	writeDirectClient(requete->clientId, "Content-length: ", 16);
	writeDirectClient(requete->clientId, total_length_string, strlen(total_length_string));
	writeDirectClient(requete->clientId, "\r\n", 2);
	better_free(total_length_string);
}

void fill_headers(tree_node* root, FCGI_Header* h) {
	char* abs_path = get_first_value(root, "absolute_path");
	char* script_f_name = calloc(1, strlen(abs_path) + 20);
	strcat(script_f_name, "/var/www/html");
	strcat(script_f_name, abs_path);
	addNameValuePair(h, "SCRIPT_FILENAME", script_f_name);
	better_free(script_f_name);
	better_free(abs_path);
	send_direct_header_cgi(root, h, "REQUEST_METHOD", "method");
	send_direct_header_cgi(root, h, "QUERY_STRING", "query");
	send_direct_header_cgi(root, h, "CONTENT_LENGTH", "Content_Length");
	send_indirect_header_cgi(root, h, "HTTP_ACCEPT", "Accept");
	send_direct_header_cgi(root, h, "CONTENT_TYPE", "Content_Type");
	send_indirect_header_cgi(root, h, "HTTP_ACCEPT_LANGUAGE", "Accept-Language");
	send_indirect_header_cgi(root, h, "HTTP_ACCEPT_ENCODING", "Accept-Encoding");
	send_indirect_header_cgi(root, h, "HTTP_USER_AGENT", "User-Agent");
}

int sendFCGI(tree_node* root, message* requete) {
	bool keepalive = false;
	int fd;
	size_t len;
	FCGI_Header h={0};

	// Establish a connection to the FastCGI server
	fd = createSocket(9000);
	uint16_t ID = rand();
	// Send a FCGI_BEGIN_REQUEST message to start a new request
	sendBeginRequest(fd, ID, FCGI_RESPONDER, FCGI_KEEP_CONN);

	// Send a FCGI_PARAMS message with required environment variables
	h.version = FCGI_VERSION_1;
	h.type = FCGI_PARAMS;
	h.requestId = htons(ID);
	h.contentLength = 0;
	h.paddingLength = 0;
	fill_headers(root, &h);

	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));

	// Send an empty FCGI_PARAMS message
	h.contentLength = 0;
	h.paddingLength = 0;
	writeSocket(fd, &h, FCGI_HEADER_SIZE + (h.contentLength) + (h.paddingLength));

	//    BODY=> "message_body" direct
	char* length_string = get_first_value(root, "Content_Length");
	if (length_string != NULL) {
		int length = 0;
		sscanf(length_string, "%d", &length);
		char* data = get_first_value(root, "message_body");
		printf("send data:%d|%s\n", length, data);
		sendWebData(fd, FCGI_STDIN, ID, data, length);
	}
	sendWebData(fd, FCGI_STDIN, ID, NULL, 0);
	better_free(length_string);

	// Read the response from the server
	char* version = get_first_value(root, "HTTP_version");
	printf("version:%s\n", version);
	if (strcmp(version, "HTTP/1.1") == 0) {
		writeDirectClient(requete->clientId, "HTTP/1.1 ", 9);
		keepalive = true;
	} else if (strcmp(version, "HTTP/1.0") == 0) {
		writeDirectClient(requete->clientId, "HTTP/1.0 ", 9);
	}
	better_free(version);
	char* connection = get_first_value(root, "Connection");
	if (connection != NULL) {
		if (strcasecmp(connection, "close") == 0) {
			keepalive = false;
		} else if (strcasecmp(connection, "keep-alive") == 0 && strcasecmp(connection, "keepalive") == 0 && strcasecmp(connection, "keep alive") == 0) {
			keepalive = true;
		}
	}
	better_free(connection);
	response_data* response_list = NULL;
	response_data* current_response = NULL;
	bool first = true;
	unsigned long content_length = 0;
	do {
		readData(fd, &h, &len);
		if (h.type == FCGI_STDOUT) {
			if (len > 0) {
				if (first) {
					first = false;
					if (strstr(h.contentData, "Status: ")) {
						char* pointer = h.contentData + 8;
						writeDirectClient(requete->clientId, pointer, h.contentLength - 8);
					} else {
						send_status(200, requete->clientId);
						if (keepalive) {
							response_data* new_response = malloc(sizeof(response_data));
							new_response->data = malloc(h.contentLength);
							memcpy(new_response->data, h.contentData, h.contentLength);
							new_response->length = h.contentLength;
							new_response->next = NULL;
							response_list = new_response;
							current_response = new_response;
							content_length += h.contentLength;
						} else {
							writeDirectClient(requete->clientId, h.contentData, h.contentLength);
						}
					}
				} else {
					if (keepalive) {
						response_data* new_response = malloc(sizeof(response_data));
						new_response->data = malloc(h.contentLength);
						memcpy(new_response->data, h.contentData, h.contentLength);
						new_response->length = h.contentLength;
						new_response->next = NULL;
						current_response->next = new_response;
						current_response = new_response;
						content_length += h.contentLength;
					} else {
						writeDirectClient(requete->clientId, h.contentData, h.contentLength);
					}
				}
			}
		}
	} while ((len != 0) && (h.type != FCGI_END_REQUEST));

	// Send the response to the client
	if (keepalive && response_list != NULL) {
		send_length(content_length, response_list->data, requete);
		response_data* current = response_list;
		// send the rest of the data and free it
		while (current != NULL) {
			writeDirectClient(requete->clientId, current->data, current->length);
			response_data* next = current->next;
			better_free(current->data);
			better_free(current);
			current = next;
		}
	}
	shutdown(fd, SHUT_RDWR);
	return keepalive;
}
