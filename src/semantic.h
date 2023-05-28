#include "../utils/global.h"
#include "populate_headers.h"


int getstatus(tree_node* root, _headers_request* header_req);
// int checkVersion(tree_node* root);
// int checkConnection(tree_node* root);
// int checkAcceptEncoding(tree_node* root);
// int checkHostHeader(tree_node* root);
char* getHost(tree_node* root);
// bool isAccepted(tree_node* root, char* mime_type);
// bool keepAlive(tree_node* root);