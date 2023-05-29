#ifndef ANSWER_H
#define ANSWER_H
#include "../utils/global.h"
#include "../lib/request.h"
void send_status(int status, int clientId);
void answerback(tree_node* root, _headers_request* headers_request, _Response* reponse);
void populateRespFromReq(_headers_request* headers_request,_Response* response);
void sendChunkedBody(FILE * file, int clientId);
#endif