#include "../utils/global.h"
#include "api.h"
int checkSemantics(tree_node* root);
int checkVersion(tree_node* root);
int checkConnection(tree_node* root);
bool isin(char* str, char* list[]);
int checkAcceptEncoding(tree_node* root);
int checkHostHeader(tree_node* root);
char* getHost(tree_node* root);
bool isAccepted(tree_node* root, char* mime_type);