#include "parser.h"
//TODO update perent length on child add
int parseur(char *req,int bytes)
{
    printf("size:%d req:%s\n",bytes,req);
    tree_node*root = tree_node_init(req);
    bool rep=validate_message(root);
    printf("rep:%d\n",rep);
    tree_node_print_all(root,0);
    if(!rep){
        printf("error\n");
        // tree_node_free(root);
        return 0;
    }
    return 1;
}
// nombre = 1*DIGIT 
tree_node* validate_number(tree_node* parent){
    int index=get_start(parent);
    if(!isdigit(parent->string[index])){
        printf("not a number:%c\n",parent->string[index]);
        return NULL;
    }
    tree_node* node_nombre=tree_node_add_child(parent,parent->string,index,0,"nombre");
    tree_node_add_child(node_nombre,parent->string,index,1,"digit");
    index++;
    while(isdigit(parent->string[index])){
        tree_node_add_child(node_nombre,parent->string,index,1,"digit");
        index++;
    }
    return node_nombre;
}
// ponct = "," / "." / "!" / "?" / ":"
tree_node* validate_ponct(tree_node *parent){
    int index = get_start(parent);
    switch (parent->string[index])
    {
    case ',':
    case '.':
    case '!':
    case '?':
    case ':':
        return tree_node_add_child(parent, parent->string, index, 1, "ponct");
    default:
        return NULL;
    }
}
// separateur = SP / HTAB / "-" / "_" 
tree_node* validate_separateur(tree_node *parent){
    int index = get_start(parent);
    switch (parent->string[index])
    {
    case ' ':
    case '\t':
    case '-':
    case '_':
        return tree_node_add_child(parent, parent->string, index, 1, "separateur");
    default:
        return NULL;
    }
}
// debut = "start" 
tree_node* validate_debut(tree_node *parent){
    int index = get_start(parent);
    if(strncasecmp(parent->string+index,"start",5)==0){
        return tree_node_add_child(parent, parent->string, index, 5, "debut");
    }
    return NULL;
}
// fin = "fin" 
tree_node* validate_fin(tree_node *parent){
    int index = get_start(parent);
    if(strncasecmp(parent->string+index,"fin",3)==0){
        return tree_node_add_child(parent, parent->string, index, 3, "fin");
    }
    return NULL;
}
// mot = 1*ALPHA separateur  
tree_node* validate_mot(tree_node *parent){
    int index = get_start(parent);
    printf("mot:%s\n",parent->string+index);
    tree_node* node_mot=tree_node_add_child(parent, parent->string, index, 2, "mot");
    if(!isalpha(parent->string[index])){
        printf("alpha !ok\n");
        tree_node_free(node_mot);
        return NULL;
    }
    tree_node_add_child(node_mot, parent->string, index, 1, "alpha");
    index++;
    while (isalpha(parent->string[index]))
    {
        tree_node_add_child(node_mot, parent->string, index, 1, "alpha");
        index++;
    }
    
    if(validate_separateur(node_mot)==NULL){
        tree_node_free(node_mot);
        return NULL;
    }
    
    return node_mot;
}
// mot ponct 
tree_node* validate_mot_ponct(tree_node *parent){
    tree_node* node_mot=NULL;
    tree_node* node_ponctuation=NULL;
    if((node_mot=validate_mot(parent))!=NULL){
        printf("mot ok\n");
        if((node_ponctuation=validate_ponct(parent))!=NULL){
            printf("ponct ok\n");
            return node_ponctuation;
        }
    }
    tree_node_free(node_mot);
    tree_node_free(node_ponctuation);
    return NULL;
}
// nombre separateur
tree_node* validate_number_separateur(tree_node *parent){
    tree_node* node_number=NULL;
    tree_node* node_separateur=NULL;    
    if((node_number=validate_number(parent))!=NULL){
        printf("mot ok\n");
        if((node_separateur=validate_separateur(parent))!=NULL){
            printf("ponct ok\n");
            return node_separateur;
        }
    }
    tree_node_free(node_number);
    tree_node_free(node_separateur);
    return NULL;
}
// message = debut 2*( mot ponct /nombre separateur ) [ponct] fin LF 
bool validate_message(tree_node *parent){
    if(validate_debut(parent)!=NULL){
        printf("debut ok\n");
        int i=0;
        while (validate_number_separateur(parent)!=NULL || validate_mot_ponct(parent)!=NULL){
            i++;
        }
        if(i<2) return false;
        validate_ponct(parent);//optional
        if(validate_fin(parent)!=NULL){
            printf("fin ok\n");
            if(parent->string[get_start(parent)]=='\n'){
                printf("LF ok\n");
                return true;
            }
        }
    }
    return false;
}
