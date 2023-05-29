#include "../utils/global.h"
#include "headers.h"


int getstatus(tree_node* root, _headers_request* header_req);
int checkReferer(tree_node* root, _headers_request* header_req);
int checkVersion(tree_node* root, _headers_request* header_req);
int checkConnection(tree_node* root, _headers_request* header_req);
int checkTransfertEncoding(tree_node* root, _headers_request* header_req);
int checkAcceptEncoding(tree_node* root, _headers_request* header_req) ;
int checkHostHeader(tree_node* root, _headers_request* header_req) ;
bool isAccepted(tree_node* root, char* mime_type);