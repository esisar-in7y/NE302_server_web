#include "../lib/request.h"
#include "headers.h"

void send_header(int clientId, char* data1,char* data2) {
    writeClient(clientId, data1);
    writeClient(clientId, data2);
    writeDirectClient(clientId, "\n\n", 2);
}

void send_headers(int clientId,_headers_response* header_response) {
	if (header_response->version != NULL && header_response->status_code != NULL) {
		if (header_response->version == HTTP1_1) {
			writeDirectClient(clientId, "HTTP/1.1 ", 9);
		}else if (header_response->version == HTTP1_0) {
            writeDirectClient(clientId, "HTTP/1.0 ", 9);
        }
        send_status(header_response->status_code, clientId);
	}

    if (header_response->content_type != NULL) {
        send_header(clientId, "Content-Type: ", header_response->content_type);
    }

    if(header_response->content_length != NULL){
        char content_length_str[10];
        sprintf(content_length_str, "%d", *header_response->content_length);
        send_header(clientId, "Content-Length: ", content_length_str);
    }

    if(header_response->connection != NULL){
        if(*header_response->connection == KEEP_ALIVE){
            send_header(clientId, "Connection: ", "keep-alive");
        }else{
            send_header(clientId, "Connection: ", "close");
        }
    }

    if(header_response->range != NULL){
        char range_str[50];
        sprintf(range_str, "bytes %d-%d/%d", header_response->range->start, header_response->range->end, header_response->range->total);
        send_header(clientId, "Content-Range: ", range_str);
    }

    if(header_response->server_timings != NULL){
        _Server_timings* current = header_response->server_timings;
        while(current != NULL){
            char timing_str[50];
            sprintf(timing_str, "%s;dur=%d", current->timing->name, current->timing->value);
            send_header(clientId, "Server-Timing: ", timing_str);
            current = current->next;
        }
    }
}