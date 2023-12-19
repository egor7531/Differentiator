#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parsing.h"
#include "Differentiator.h"

TreeNode* get_E(char** buf);
TreeNode* get_T(char** buf);
TreeNode* get_POW(char** buf);
TreeNode* get_P(char** buf);
TreeNode* get_ID(char** buf);
TreeNode* get_N(char** buf);
void syntax_assert(bool flag, const char* nameFunc, char** buf);

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
    tree->root = get_E(buf);
    syntax_assert(**buf == '\0' || **buf == '\r' || **buf == '\n', "get_G", buf);
}

TreeNode* get_E(char** buf)
{
    TreeNode* node1 = get_T(buf);
    TreeNode* node = nullptr;

    while(**buf == '+' || **buf == '-')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_T(buf);

        node = (TreeNode*)calloc(1, sizeof(TreeNode));
        if(node == nullptr)
            return nullptr;

        node->elem = (NodeData*)calloc(1, sizeof(NodeData));

        if(((NodeData*)(node->elem)) == nullptr)
            return nullptr;

        ((NodeData*)(node->elem))->type = OPERATOR;

        switch(op)
        {
            case '+':
                ((NodeData*)(node->elem))->elem.op = OP_ADD;
                break;
            case '-':
                ((NodeData*)(node->elem))->elem.op = OP_SUB;
                break;
            default:
                syntax_assert(false, "get_E", buf);
                break;
        }

        if(((NodeData*)(node1->elem))->type == VARIABLE)
        {
            TreeNode* nodePOW = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodePOW->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodePOW->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodePOW->elem))->type = OPERATOR;
            ((NodeData*)(nodePOW->elem))->elem.op = OP_POW;

            TreeNode* nodeExp = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodeExp->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodeExp->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodeExp->elem))->type = NUM;
            ((NodeData*)(nodeExp->elem))->elem.value = 1;

            tree_link_node(node, nodePOW);
            tree_link_node(node, node2);
            tree_link_node(nodePOW, node1);
            tree_link_node(nodePOW, nodeExp);
        }
        else if(((NodeData*)(node2->elem))->type == VARIABLE)
        {
            TreeNode* nodePOW = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodePOW->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodePOW->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodePOW->elem))->type = OPERATOR;
            ((NodeData*)(nodePOW->elem))->elem.op = OP_POW;

            TreeNode* nodeExp = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodeExp->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodeExp->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodeExp->elem))->type = NUM;
            ((NodeData*)(nodeExp->elem))->elem.value = 1;

            tree_link_node(node, node1);
            tree_link_node(node, nodePOW);
            tree_link_node(nodePOW, node2);
            tree_link_node(nodePOW, nodeExp);
        }
        else
        {
            tree_link_node(node, node1);
            tree_link_node(node, node2);
        }

        /*tree_link_node(node, node1);
        tree_link_node(node, node2);*/
        node1 = node;
    }

    if(node != nullptr)
        return node;
    return node1;
}

TreeNode* get_T(char** buf)
{
    TreeNode* node1 = get_POW(buf);
    TreeNode* node = nullptr;

    while(**buf == '*' || **buf == '/')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_POW(buf);

        node = (TreeNode*)calloc(1, sizeof(TreeNode));
        if(node == nullptr)
            return nullptr;

        node->elem = (NodeData*)calloc(1, sizeof(NodeData));

        if(((NodeData*)(node->elem)) == nullptr)
            return nullptr;

        ((NodeData*)(node->elem))->type = OPERATOR;

        switch(op)
        {
            case '*':
                ((NodeData*)(node->elem))->elem.op = OP_MUL;
                break;
            case '/':
                ((NodeData*)(node->elem))->elem.op = OP_DIV;
                break;
            default:
                syntax_assert(false, "get_T", buf);
                break;
        }

        if(((NodeData*)(node1->elem))->type == VARIABLE)
        {
            TreeNode* nodePOW = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodePOW->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodePOW->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodePOW->elem))->type = OPERATOR;
            ((NodeData*)(nodePOW->elem))->elem.op = OP_POW;

            TreeNode* nodeExp = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodeExp->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodeExp->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodeExp->elem))->type = NUM;
            ((NodeData*)(nodeExp->elem))->elem.value = 1;

            tree_link_node(node, nodePOW);
            tree_link_node(node, node2);
            tree_link_node(nodePOW, node1);
            tree_link_node(nodePOW, nodeExp);
        }
        else if(((NodeData*)(node2->elem))->type == VARIABLE)
        {
            TreeNode* nodePOW = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodePOW->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodePOW->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodePOW->elem))->type = OPERATOR;
            ((NodeData*)(nodePOW->elem))->elem.op = OP_POW;

            TreeNode* nodeExp = (TreeNode*)calloc(1, sizeof(TreeNode));
            if(nodePOW == nullptr)
                return nullptr;

            nodeExp->elem = (NodeData*)calloc(1, sizeof(NodeData));

            if(((NodeData*)(nodeExp->elem)) == nullptr)
                return nullptr;

            ((NodeData*)(nodeExp->elem))->type = NUM;
            ((NodeData*)(nodeExp->elem))->elem.value = 1;

            tree_link_node(node, node1);
            tree_link_node(node, nodePOW);
            tree_link_node(nodePOW, node2);
            tree_link_node(nodePOW, nodeExp);
        }
        else
        {
            tree_link_node(node, node1);
            tree_link_node(node, node2);
        }

        /*tree_link_node(node, node1);
        tree_link_node(node, node2);*/
        node1 = node;
    }

    if(node != nullptr)
        return node;
    return node1;
}

TreeNode* get_POW(char** buf)
{
    TreeNode* node1 = get_P(buf);
    TreeNode* node = nullptr;

    while(**buf == '^')
    {
        char op = **buf;
        (*buf)++;
        TreeNode* node2 = get_P(buf);

        node = (TreeNode*)calloc(1, sizeof(TreeNode));
        if(node == nullptr)
            return nullptr;

        node->elem = (NodeData*)calloc(1, sizeof(NodeData));

        if(((NodeData*)(node->elem)) == nullptr)
            return nullptr;

        ((NodeData*)(node->elem))->type = OPERATOR;
        ((NodeData*)(node->elem))->elem.op = OP_POW;

        tree_link_node(node, node1);
        tree_link_node(node, node2);

        node1 = node;
    }

    if(node != nullptr)
        return node;
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

    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node == nullptr)
        return nullptr;

    node->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(node->elem)) == nullptr)
        return nullptr;
    ((NodeData*)(node->elem))->type = VARIABLE;
    ((NodeData*)(node->elem))->elem.variable = strdup(id);

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
        int order = 10;
        while('0' <= **buf && **buf <= '9')
        {
            val = val + (**buf - '0') / order;
            order *= 10;
            (*buf)++;
        }
    }
    syntax_assert(oldBuf < *buf, "get_N", buf);

    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node == nullptr)
        return nullptr;

    node->elem = calloc(1, sizeof(NodeData));

    if(((NodeData*)(node->elem)) == nullptr)
        return nullptr;
    ((NodeData*)(node->elem))->type = NUM;
    ((NodeData*)(node->elem))->elem.value = val;

    return node;
}
