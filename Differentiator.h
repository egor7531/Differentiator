#ifndef DIFFERENTIATOR_H_INCLUDED
#define DIFFERENTIATOR_H_INCLUDED

#include "Tree.h"

enum Operators
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_LN,
    OP_SIN,
    OP_COS,
    OP_SQRT,
    OP_EXP
};

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
