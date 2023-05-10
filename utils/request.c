#include "../lib/request.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

message* getRequest(short int port) {
	static int sockfd = -1;
	if (sockfd == -1) {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1) {
			return NULL;
		}

		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);

		if (bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) {
			close(sockfd);
			sockfd = -1;
			return NULL;
		}

		if (listen(sockfd, MAXCLIENT) != 0) {
			close(sockfd);
			sockfd = -1;
			return NULL;
		}
	}

	message* msg = (message*)malloc(sizeof(message));
	if (msg == NULL) {
		return NULL;
	}

	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientfd = accept(sockfd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (clientfd == -1) {
		free(msg);
		return NULL;
	}

	char buf[1024];
	int numBytes = recv(clientfd, buf, sizeof(buf), 0);
	if (numBytes == -1) {
		close(clientfd);
		free(msg);
		return NULL;
	}

	msg->buf = (char*)malloc(numBytes);
	if (msg->buf == NULL) {
		close(clientfd);
		free(msg);
		return NULL;
	}
	memcpy(msg->buf, buf, numBytes);
	msg->len = numBytes;
	msg->clientId = clientfd;
	msg->clientAddress = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	if (msg->clientAddress == NULL) {
		close(clientfd);
		free(msg->buf);
		free(msg);
		return NULL;
	}
	memcpy(msg->clientAddress, &clientAddress, sizeof(struct sockaddr_in));

	return msg;
}

void freeRequest(message* r) {
	if (r != NULL) {
		if (r->buf != NULL) {
			free(r->buf);
		}
		if (r->clientAddress != NULL) {
			free(r->clientAddress);
		}
		close(r->clientId);
		free(r);
	}
}

void sendResponse(message* r) {
	if (r != NULL) {
		send(r->clientId, r->buf, r->len, 0);
		freeRequest(r);
	}
}

void writeDirectClient(int i, char* buf, unsigned int len) {
	send(i, buf, len, 0);
}

void endWriteDirectClient(int i) {
	shutdown(i, SHUT_WR);
}

void requestShutdownSocket(int i) {
	shutdown(i, SHUT_RDWR);
}
