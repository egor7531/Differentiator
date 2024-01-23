#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parsing.h"
#include "Utility.h"

#define SYNTAX_ASSERT(condition) syntax_assert(condition, __func__ , tree, buf);

TreeNode* get_E(Tree* tree, char** buf);
TreeNode* get_T(Tree* tree, char** buf);
TreeNode* get_POW(Tree* tree, char** buf);
TreeNode* get_P(Tree* tree, char** buf);
TreeNode* get_ID(Tree* tree, char** buf);
TreeNode* get_N(Tree* tree, char** buf);

void syntax_assert(bool flag, const char* nameFunc, Tree* tree, char** buf)
{
    if(!flag)
    {
        printf("\"%s\" error: %s", nameFunc, *buf);
        tree_dtor(tree);
        abort();
    }
}

void get_G(Tree* tree, char** buf)
{
    assert(tree != nullptr);
    assert(buf != nullptr);

    tree->root = get_E(tree, buf);
    SYNTAX_ASSERT(**buf == '\0' || **buf == '\r' || **buf == '\n');
}

TreeNode* get_E(Tree* tree, char** buf)
{
    TreeNode* node1 = get_T(tree, buf);

    while(**buf == '+' || **buf == '-')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_T(tree, buf);

        Data elem;
        switch(op)
        {
            case '+':
                elem.op = OP_ADD;
                break;
            case '-':
                elem.op = OP_SUB;
                break;
            default:
                SYNTAX_ASSERT(false);
                break;
        }

        TreeNode* node = create_node(OPERATOR, elem, node1, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_T(Tree* tree, char** buf)
{
    TreeNode* node1 = get_POW(tree, buf);

    while(**buf == '*' || **buf == '/')
    {
        char op = **buf;
        (*buf)++;

        TreeNode* node2 = get_POW(tree, buf);

        Data elem;
        switch(op)
        {
            case '*':
                elem.op = OP_MUL;
                break;
            case '/':
                elem.op = OP_DIV;
                break;
            default:
                SYNTAX_ASSERT(false);
                break;
        }

        TreeNode* node = create_node(OPERATOR, elem, node1, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_POW(Tree* tree, char** buf)
{
    TreeNode* node1 = get_P(tree, buf);

    while(**buf == '^')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_P(tree, buf);

        Data elem;
        elem.op = OP_POW;
        TreeNode* node = create_node(OPERATOR, elem, node1, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_P(Tree* tree, char** buf)
{
    TreeNode* node = nullptr;

    if(**buf == '(')
    {
        (*buf)++;
        node = get_E(tree, buf);
        SYNTAX_ASSERT(**buf == ')');
        (*buf)++;
        return node;
    }

    node = get_ID(tree, buf);
    if(node == nullptr)
        node = get_N(tree, buf);

    return node;
}

TreeNode* get_ID(Tree* tree, char** buf)
{
    TreeNode* node = nullptr;
    const int MAX_SIZE_ID = 20;
    char id[MAX_SIZE_ID] = {};
    int p = 0;

    if(!('a' <= **buf && **buf <= 'z' || 'A' <= **buf && **buf <= 'Z'))
        return nullptr;

    while('a' <= **buf && **buf <= 'z' || 'A' <= **buf && **buf <= 'Z' || **buf == '_'
                        || **buf == '_' || **buf == '$' || '0' <= **buf && **buf <= '9')
    {
        id[p] = **buf;
        (*buf)++;
        p++;
    }

    Data elem;

    if(**buf == '(')
    {
        (*buf)++;

        #define DEF_OP(name, icon, binary_op, code, der)    \
            if(!strcmp(id, icon) && !binary_op)             \
                elem.op = name;

        #include "Operators.h"

        #undef DEF_OP

        node = get_E(tree, buf);
        SYNTAX_ASSERT(**buf == ')');
        (*buf)++;

        return create_node(OPERATOR, elem, node, nullptr);
    }

    elem.id = id;
    return create_node(IDENTIFIER, elem, nullptr, nullptr);
}

TreeNode* get_N(Tree* tree, char** buf)
{
    double val = 0;
    char* oldBuf = *buf;
    while('0' <= **buf && **buf <= '9')
    {
        val = val * 10 + (**buf - '0');
        (*buf)++;
    }
    if(**buf == '.')
    {
        (*buf)++;
        double order = 10;
        while('0' <= **buf && **buf <= '9')
        {
            val = val + (**buf - '0') / order;
            order *= 10;
            (*buf)++;
        }
    }
    SYNTAX_ASSERT(oldBuf < *buf);

    Data elem;
    elem.value = val;

    return create_node(NUM, elem, nullptr, nullptr);
}
