#include "../utils/global.h"
#include "api.h"
int checkSemantics(_Token* root);
int checkConnection(_Token* root);
bool isin(char* str, char* list[]);
int checkAcceptEncoding(_Token* root);
int checkHostHeader(_Token* root);