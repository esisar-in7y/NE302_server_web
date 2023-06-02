#include <zlib.h>
#include "../utils/global.h"
#include "../lib/request.h"
#include "../utils/manip.h"
#include "../utils/mime.h"
#include "../utils/structures.h"
#include "api.h"
#ifndef ANSWER_H
#define ANSWER_H
#include "semantic.h"
void send_status(int status, int clientId);
bool send_data(tree_node* root, _headers_request* headers_request, _Response* reponse);
void send_response(_Response* reponse);
void populateRespFromReq(_headers_request* headers_request,_Response* response);
void sendChunkedBody(FILE * file, int clientId);
void sendDeflateBody(FILE* file, int clientId);
#endif