#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "Tree.h"
#include "Differentiator.h"

void get_G(Tree* tree, char** buf);
TreeNode* create_node(TypeElem type, Data elem);

#endif //PARSING_H_INCLUDED
