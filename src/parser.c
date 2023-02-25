#include "parser.h"
//TODO update perent length on child add
int parseur(char *req,int bytes)
{
    printf("size:%d req:%s\n",bytes,req);
    tree_node*root = tree_node_init(req);
    bool rep=validate_message(root);
    printf("rep:%d\n",rep);
    if(!rep){
        printf("error\n");
        tree_node_free(root);
        return -1;
    }
    tree_node_print_all(root,0);
    return 0;
}
int _get_start(tree_node* node,int lvl){
    int start = 0;
    int tmp=0;
    for(int i = 0; i < node->childs_count; i++){
        if(node->childs[i]){
            tmp=_get_start(node->childs[i],lvl+1);
            if(tmp>start){
                start=tmp;
            }
        }
    }
    tmp=node->start_string+(lvl>0?node->length_string:0);
    if(tmp>start){
        start=tmp;
    }
    // printf("lvl:%d \n",start);
    return start;
}
int get_start(tree_node* node){
    return _get_start(node,0);
}
// nombre = 1*DIGIT 
tree_node* validate_number(tree_node* parent){
    int index=get_start(parent);
    if(!isdigit(parent->string[index])){
        return NULL;
    }
    return tree_node_add_child(parent,parent->string,index,1,nombre);
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
        return tree_node_add_child(parent, parent->string, index, 1, ponct);

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
        return tree_node_add_child(parent, parent->string, index, 1, separateur);
    default:
        return NULL;
    }
}
// debut = "start" 
tree_node* validate_debut(tree_node *parent){
    int index = get_start(parent);
    if(strncmp(parent->string+index,"start",5)==0){
        return tree_node_add_child(parent, parent->string, index, 5, debut);
    }
    return NULL;
}
// fin = "fin" 
tree_node* validate_fin(tree_node *parent){
    int index = get_start(parent);
    if(strncmp(parent->string+index,"fin",3)==0){
        return tree_node_add_child(parent, parent->string, index, 3, fin);
    }
    return NULL;
}
// mot = 1*ALPHA separateur  
tree_node* validate_mot(tree_node *parent){
    int index = get_start(parent);
    printf("mot:%s\n",parent->string+index);
    tree_node* node_mot=tree_node_add_child(parent, parent->string, index, 2, mot);
    if(!isalpha(parent->string[index])){
        printf("alpha !ok\n");
        return NULL;
    }
    tree_node_add_child(node_mot, parent->string, index, 1, alpha);
    if(validate_separateur(node_mot)==NULL){
        return NULL;
    }
    return node_mot;
}
// message = debut 2*( mot ponct /nombre separateur ) [ponct] fin LF 
bool validate_message(tree_node *parent){
    if(validate_debut(parent)!=NULL){
        printf("debut ok\n");
        for (size_t i = 0; i < 2; i++)
        {
            if(validate_mot(parent)!=NULL){
                printf("mot ok\n");
                if(validate_ponct(parent)==NULL){
                    printf("ponct !ok\n");
                    return false;
                }else{
                    printf("ponct ok\n");
                }
            }else if(validate_number(parent)!=NULL){
                printf("number ok\n");
                if(validate_separateur(parent)==NULL){
                    printf("separateur !ok\n");
                    return false;
                }else{
                    printf("separateur ok\n");
                }
            }else{
                printf("mot !ok\n");
                return false;
            }
        }
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


// bool validateRequest(char *req)
// {
//     tree_node *head_tree = tree_node_init(req);
//     bool res = validateHttpMessage(head_tree);
//     if (!res)
//         tree_node_free(head_tree);

//     return res;
// }
// bool validateHttpMessage(tree_node *node)
// {
//     bool res = false;
//     if (validateStartLine(node))
//     {
//         if (validateHeaders(node))
//         {
//             if (validateBody(node))
//             {
//                 res = 1;
//             }
//         }
//     }
//     return res;
// }