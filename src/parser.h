#include "../utils/global.h"
#include "../utils/tree.h"
#include "api.h"
// bool validateHttpMessage(tree_node *node);
int get_start(tree_node* node);
tree_node*  validate_number(tree_node* parent);
tree_node*  validate_ponct(tree_node *parent);
tree_node*  validate_separateur(tree_node *parent);
tree_node*  validate_debut(tree_node *parent);
tree_node*  validate_fin(tree_node *parent);
tree_node*  validate_mot(tree_node *parent);
bool validate_message(tree_node *parent);

