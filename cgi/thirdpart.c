#include "thirdpart.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fastcgi.h"

#define FCGI_SOCKET_PORT 9000

char* run_cgi(char* request) {
    int fd = createSocket(FCGI_SOCKET_PORT);

    // Send FastCGI request
    sendGetValue(fd);
    sendBeginRequest(fd, 10, FCGI_RESPONDER, FCGI_KEEP_CONN);
    sendStdin(fd, 10, request, strlen(request));
    sendData(fd, 10, request, strlen(request));

    // Read FastCGI response
    char* response = NULL;
    size_t response_size = 0;
    ssize_t read_size;

    while ((read_size = read(fd, response + response_size, FCGI_HEADER_SIZE)) > 0) {
        response_size += read_size;
        response = realloc(response, response_size + FCGI_HEADER_SIZE);

        FCGI_Header* header = (FCGI_Header*)(response + response_size - read_size);

        if (header->type == FCGI_STDOUT || header->type == FCGI_STDERR) {
            ssize_t content_length = ntohs(header->contentLength);
            ssize_t padding_length = header->paddingLength;

            // Read content data
            while (content_length > 0) {
                read_size = read(fd, response + response_size, content_length);
                if (read_size <= 0)
                    break;

                response_size += read_size;
                response = realloc(response, response_size + content_length);
                content_length -= read_size;
            }

            // Read padding data
            while (padding_length > 0) {
                read_size = read(fd, response + response_size, padding_length);
                if (read_size <= 0)
                    break;

                response_size += read_size;
                response = realloc(response, response_size + padding_length);
                padding_length -= read_size;
            }
        } else if (header->type == FCGI_END_REQUEST) {
            break;
        }
    }

    // Close the socket
    close(fd);

    return response;
}
