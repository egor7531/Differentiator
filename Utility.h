#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "Tree.h"
#include "Differentiator.h"

void* elem_ctor(void* elem);
void elem_dtor(void* elem);
void write_elem(FILE* fp, void* elem);
void print_expression(const char* nameFile, const Tree* tree);
TreeNode* copy_node(TreeNode* node);
TreeNode* create_node(TypeElem type, Data elem, TreeNode* leftNode, TreeNode* rightNode);
int compare_numbers(double x1, double x2);

#endif //UTILITY_H_INCLUDED
