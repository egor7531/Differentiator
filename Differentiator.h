#ifndef DIFFERENTIATOR_H_INCLUDED
#define DIFFERENTIATOR_H_INCLUDED

#include "Tree.h"

enum Operators
{
    NO_OP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_ln
};

enum TypeElem
{
    OPERATOR,
    NUM,
    VARIABLE
};

union Data
{
    double      value;
    Operators   op;
    char*       variable;
};

struct NodeData
{
    TypeElem    type;
    Data        elem;
};

void get_derivative(const char* nameFile);
void get_taylor_series(const char* nameFile);
TreeNode* create_node(TypeElem type, Data elem);

#endif //DIFFERENTIATOR_H_INCLUDED