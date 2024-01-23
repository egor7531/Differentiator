#ifndef DIFFERENTIATOR_H_INCLUDED
#define DIFFERENTIATOR_H_INCLUDED

#include "Tree.h"

#define DEF_OP(name, icon, binary_op, code, der)    \
    name,

enum Operators
{
    #include "Operators.h"
};

#undef DEF_OP

enum TypeElem
{
    OPERATOR,
    NUM,
    IDENTIFIER
};

union Data
{
    double      value;
    Operators   op;
    char*       id;
};

struct NodeData
{
    TypeElem    type;
    Data        elem;
};

void get_derivative(const char* nameFile);
void get_taylor_series(const char* nameFile, const int order, const double point);

#endif //DIFFERENTIATOR_H_INCLUDED
