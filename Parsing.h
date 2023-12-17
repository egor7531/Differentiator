#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "Tree.h"

void get_G(Tree* tree, char** buf);
void get_E(char** buf);
void get_T(char** buf);
void get_P(char** buf);
void get_N(char** buf);
void syntax_assert(bool flag, const char* nameFunc);

#endif //PARSING_H_INCLUDED
