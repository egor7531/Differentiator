#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parsing.h"

TreeNode* get_E(char** buf);
TreeNode* get_T(char** buf);
TreeNode* get_POW(char** buf);
TreeNode* get_P(char** buf);
TreeNode* get_ID(char** buf);
TreeNode* get_N(char** buf);
void syntax_assert(bool flag, const char* nameFunc, char** buf);

TreeNode* create_node(TypeElem type, Data elem)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node == nullptr)
        return nullptr;

    node->elem = (NodeData*)calloc(1, sizeof(NodeData));
    if(((NodeData*)(node->elem)) == nullptr)
        return nullptr;

    ((NodeData*)(node->elem))->type = type;
    if(type == NUM)
        ((NodeData*)(node->elem))->elem.value = elem.value;
    else if(type == VARIABLE)
        ((NodeData*)(node->elem))->elem.variable = strdup(elem.variable);
    else if(type == OPERATOR)
        ((NodeData*)(node->elem))->elem.op = elem.op;
    else
        assert("Unknown type");

    return node;
}

void syntax_assert(bool flag, const char* nameFunc, char** buf)
{
    if(!flag)
    {
        printf("\"%s\" error: %s", nameFunc, *buf);
        abort();
    }
}

void get_G(Tree* tree, char** buf)
{
    assert(tree != nullptr);
    assert(buf != nullptr);

    tree->root = get_E(buf);
    syntax_assert(**buf == '\0' || **buf == '\r' || **buf == '\n', "get_G", buf);
}

TreeNode* get_E(char** buf)
{
    TreeNode* node1 = get_T(buf);

    while(**buf == '+' || **buf == '-')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_T(buf);

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
                syntax_assert(false, "get_E", buf);
                break;
        }

        TreeNode* node = create_node(OPERATOR, elem);

        tree_link_node(node, node1);
        tree_link_node(node, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_T(char** buf)
{
    TreeNode* node1 = get_POW(buf);

    while(**buf == '*' || **buf == '/')
    {
        char op = **buf;
        (*buf)++;

        TreeNode* node2 = get_POW(buf);

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
                syntax_assert(false, "get_T", buf);
                break;
        }

        TreeNode* node = create_node(OPERATOR, elem);

        tree_link_node(node, node1);
        tree_link_node(node, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_POW(char** buf)
{
    TreeNode* node1 = get_P(buf);

    while(**buf == '^')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_P(buf);

        Data elem;
        elem.op = OP_POW;
        TreeNode* node = create_node(OPERATOR, elem);

        tree_link_node(node, node1);
        tree_link_node(node, node2);
        node1 = node;
    }

    return node1;
}

TreeNode* get_P(char** buf)
{
    TreeNode* node = nullptr;

    if(**buf == '(')
    {
        (*buf)++;
        node = get_E(buf);
        syntax_assert(**buf == ')', "get_P", buf);
        (*buf)++;
        return node;
    }

    node = get_ID(buf);
    if(node == nullptr)
        node = get_N(buf);

    return node;
}

TreeNode* get_ID(char** buf)
{
    const int MAX_SIZE_ID = 20;
    char id[MAX_SIZE_ID] = {};
    int p = 0;
    if('a' <= **buf && **buf <= 'z' || 'A' <= **buf && **buf <= 'Z')
    {
        id[p] = **buf;
        (*buf)++;
    }
    else
        return nullptr;

    while('a' <= **buf && **buf <= 'z' || 'A' <= **buf && **buf <= 'Z' || **buf == '_'
                || **buf == '_' || **buf == '$' || '0' <= **buf && **buf <= '9')
    {
        id[p] = **buf;
        *buf++;
    }

    Data elem;
    elem.variable = id;
    TreeNode* node = create_node(VARIABLE, elem);

    return node;
}

TreeNode* get_N(char** buf)
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
    syntax_assert(oldBuf < *buf, "get_N", buf);

    Data elem;
    elem.value = val;
    TreeNode* node = create_node(NUM, elem);

    return node;
}
