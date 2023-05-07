#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "api.h"

// **Connection header**

// If Version 1.0 and keep Alive option, must have Transfer-Encoding or Content-Length, if not 400 Bad Request

// If Version 1.1 and no close option, must have Transfer-Encoding or Content-Length, if not 400 Bad Request

// If is a proxy and Version 1.0 and keep Alive option or Version 1.1 and no close option => 400 Bad Request

int checkVersion(_Token* root) {}

int checkConnection(_Token* root) {
	_Token* tok = searchTree(root, "HTTP_version");
	_Token* tok2 = searchTree(root, "Connection");
	_Token* tok3 = searchTree(root, "Proxy-Connection");
	char* version = getElementValue(tok->node, tok->len);
	char* connection = getElementValue(tok2->node, tok2->len);
	char* proxyConnection = getElementValue(tok3->node, tok3->len);
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