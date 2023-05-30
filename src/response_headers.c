#include "headers.h"

void send_header(int clientId, char* data1, char* data2) {
	writeClient(clientId, data1);
	writeClient(clientId, data2);
	writeDirectClient(clientId, "\r\n", 2);
}

void send_headers(_Response* response) {
	if (response->headers_response.version != 0 && response->headers_response.status_code > 0) {
		if (response->headers_response.version == HTTP1_1) {
			writeDirectClient(response->clientId, "HTTP/1.1 ", 9);
		} else if (response->headers_response.version == HTTP1_0) {
			writeDirectClient(response->clientId, "HTTP/1.0 ", 9);
		}
		send_status(response->headers_response.status_code, response->clientId);
	} else {
		printf("NO first header:%d %d\n", response->headers_response.version, __LINE__);
	}

	if (response->headers_response.content_type != NULL) {
		send_header(response->clientId, "Content-Type: ", response->headers_response.content_type);
	}
	if (response->headers_response.range != NULL) {
		char range_str[60]={0};
		if (response->headers_response.range->start < 0) {
			sprintf(range_str, "bytes */%ld", response->headers_response.range->size);
		} else {
			if(response->headers_response.content_length == NULL){
				response->headers_response.content_length = (long*)malloc(sizeof(long));
			}
			*(response->headers_response.content_length) = response->headers_response.range->end - response->headers_response.range->start + 1;
			sprintf(range_str, "bytes %ld-%ld/%ld", response->headers_response.range->start, response->headers_response.range->end, response->headers_response.range->size);
		}
		send_header(response->clientId, "Content-Range: ", range_str);
		
	}
	if (response->headers_response.content_length != NULL) {
		char content_length_str[30]={0};
		sprintf(content_length_str, "%ld", *(response->headers_response.content_length));
		send_header(response->clientId, "Content-Length: ", content_length_str);
	} else {
		switch (response->headers_response.transfert_encoding) {
		case BR: writeClient(response->clientId, "Transfer-Encoding: br\r\n"); break;
		case GZIP: writeClient(response->clientId, "Transfer-Encoding: gzip\r\n"); break;
		case DEFLATE: writeClient(response->clientId, "Transfer-Encoding: deflate\r\n"); break;
		case CHUNKED: writeClient(response->clientId, "Transfer-Encoding: chunked\r\n"); break;
		case IDENTITY: writeClient(response->clientId, "Transfer-Encoding: identity\r\n"); break;
		default: break;
		}
	}

	if (response->headers_response.connection == KEEP_ALIVE) {
        send_header(response->clientId, "Connection: ", "keep-alive");
    } else if (response->headers_response.connection == CLOSE) {
        send_header(response->clientId, "Connection: ", "close");
    }

	// if (response->headers_response.ranges != NULL) {
	// 	struct _Ranges* current = response->headers_response.ranges;
	// 	// TODO faire comme plus bas en dessous
	// 	while (current != NULL) {
	// 		char range_str[50];
	// 		sprintf(range_str, "bytes=%d-%d", current->range->start, current->range->end);
	// 		send_header(response->clientId, "Content-Range: ", range_str);
	// 		current = current->next;
	// 	}
	// }
	

	if (response->headers_response.server_timings != NULL) {
		printf("SErver timings:\n");
		struct _Server_timings* current = response->headers_response.server_timings;
		while (current != NULL) {
			char timing_str[50]={0};
			sprintf(timing_str, "%s;dur=%d", current->timings->name, current->timings->value);
			send_header(response->clientId, "Server-Timing: ", timing_str);
			current = current->next;
		}
	}
	writeDirectClient(response->clientId, "\r\n", 2);
}