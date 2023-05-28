#ifndef MANIP_H
#define MANIP_H
#include "global.h"
#include "tree.h"
#include "../src/api.h"
#include "../lib/request.h"
void debug_http(char* string, int line);
char *copy_sub_str(const unsigned char *source,uint16_t start,uint16_t length);
void print_sub_str( unsigned char *string, int start_index, int end_index);
int  strncasecmp(const char *, const char *, size_t);
char* trim_space(char *str);
char* url_decode(const char* src);
char *remove_dot_segments(const char *input);
bool isin(char* str, char* list[]);
bool have_separators(char* string, char *value);
char* get_first_value(tree_node* root,char* search);
char* getFieldValueFromFieldName(tree_node* root, char* field_name);

/** 
\fn void writeClient(int i,char *buf)
* \brief Procedure (expérimentale) [alternative à sendReponse] d'envoi d'un buffer au client i. Il est parfois pratique d'écrire au client au fur et à mesure du traitement de la requête.
*  La librairie ne peut pas determiner toute seule la fin de la réponse. Si vous utilisez cette fonction il faut OBLIGATOIREMENT utiliser la fonction endWriteDirectClient quand la réponse est finie.
*  L'intérêt ici est de ne pas avoir à stocker dans des buffers la totalité de la réponse. 
* \param i Le client 
* \param buf Le message à envoyer (non recopié par la bibliothèque)
*/
void writeClient(int i,char *buf);

#endif