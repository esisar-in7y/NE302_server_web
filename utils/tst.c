#include "global.h"
#include "tree.h"
#if TST==1
int main(void) {
    char* string="GET / HTTP/1.1";
    tree_node* main_node=tree_node_init(string);
    tree_node* main2=tree_node_add_child(main_node,string,0,3,HTTP_name);
    tree_node_add_child(main2,string,4,5,Host);
    tree_node_add_child(main_node,string,6,19,HTTP_version);
    tree_node_print_all(main_node,0);
    int type_test=get_type("Host");
    printf("type_test:%d\n",type_test);
    _Token* result=NULL;
    tree_node_find_type(main_node,type_test,&result);
    for(_Token* tmp=result;tmp!=NULL;tmp=tmp->next)
        tree_node_print(tmp->node,0);


    tree_node_free(main_node);
    return 0;
}
#endif