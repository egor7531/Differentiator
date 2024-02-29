#include <assert.h>
#include <string.h>
#include <math.h>

#include "Node.h"

void* elem_ctor(void* elem)
{
    assert(elem != nullptr);

    NodeData* data = (NodeData*)calloc(1, sizeof(NodeData));
    if(data == nullptr)
        return nullptr;

    if(((NodeData*)elem)->type == OPERATOR)
    {
        data->type = OPERATOR;
        data->elem.op = ((NodeData*)elem)->elem.op;
    }
    else if(((NodeData*)elem)->type == NUM)
    {
        data->type = NUM;
        data->elem.value = ((NodeData*)elem)->elem.value;
    }
    else if(((NodeData*)elem)->type == IDENTIFIER)
    {
        data->type = IDENTIFIER;
        data->elem.id = strdup(((NodeData*)elem)->elem.id);
    }
    else
        assert(!"Unknown type");

    return data;
}

void elem_dtor(void* elem)
{
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == IDENTIFIER)
        free(((NodeData*)elem)->elem.id);
    free(elem);
}

void write_elem(FILE* fp, void* elem)
{
    assert(fp != nullptr);
    assert(elem != nullptr);

    NodeData* data = (NodeData*)elem;

    if(data->type == NUM)
        fprintf(fp, "%.2lf", data->elem.value);
    else if(data->type == IDENTIFIER)
        fprintf(fp, "%s", data->elem.id);
    else if(data->type == OPERATOR)
    {
        switch(data->elem.op)
        {
            #define DEF_OP(name, icon, binary_op, code, der)       \
            case name:                                             \
                fprintf(fp, "%s", icon);                           \
                break;
            default:
                assert(!"Unknown operator");
                break;

            #include "Operators.h"

            #undef DEF_OP
        }
    }
    else
        assert(!"Unknown type");
}

void print_nodes(FILE* fp, const TreeNode* node)
{
    assert(fp != nullptr);

    if(node == nullptr)
        return;

    if(((NodeData*)(node->elem))->type == OPERATOR)
    {
        if(node->leftNode == nullptr || node->rightNode == nullptr)
        {
            write_elem(fp, node->elem);
            fprintf(fp, "(");
            print_nodes(fp, node->leftNode);
            fprintf(fp, ")");
        }
        else
        {
            fprintf(fp, "(");
            print_nodes(fp, node->leftNode);
            write_elem(fp, node->elem);
            print_nodes(fp, node->rightNode);
            fprintf(fp, ")");
        }
    }
    else
        write_elem(fp, node->elem);
}

void print_expression(const char* nameFile, const Tree* tree)
{
    assert(nameFile != nullptr);
    assert(tree != nullptr);

    FILE* fp = fopen(nameFile, "wb");
    if(fp == nullptr)
        return;

    print_nodes(fp, tree->root);
    fclose(fp);
}

TreeNode* copy_node(TreeNode* node)
{
    assert(node != nullptr);

    TreeNode* copyNode = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(copyNode == nullptr)
        return nullptr;

    copyNode->elem = (NodeData*)calloc(1, sizeof(NodeData));
    if(copyNode->elem == nullptr)
        return nullptr;

    NodeData* data = (NodeData*)(node->elem);

    ((NodeData*)(copyNode->elem))->type = data->type;

    if(data->type == NUM)
        ((NodeData*)(copyNode->elem))->elem.value = data->elem.value;
    else if(data->type == IDENTIFIER)
        ((NodeData*)(copyNode->elem))->elem.id = strdup(data->elem.id);
    else if(data->type == OPERATOR)
        ((NodeData*)(copyNode->elem))->elem.op = data->elem.op;
    else
        assert(!"Unknown type");

    if(node->leftNode != nullptr)
        tree_link_node(copyNode, copy_node(node->leftNode));

    if(node->rightNode != nullptr)
        tree_link_node(copyNode, copy_node(node->rightNode));

    return copyNode;
}

TreeNode* create_node(TypeElem type, Data elem, TreeNode* leftNode, TreeNode* rightNode)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node == nullptr)
        return nullptr;

    node->elem = (NodeData*)calloc(1, sizeof(NodeData));
    if(node->elem == nullptr)
        return nullptr;

    ((NodeData*)(node->elem))->type = type;
    if(type == NUM)
        ((NodeData*)(node->elem))->elem.value = elem.value;
    else if(type == IDENTIFIER)
        ((NodeData*)(node->elem))->elem.id = strdup(elem.id);
    else if(type == OPERATOR)
        ((NodeData*)(node->elem))->elem.op = elem.op;
    else
        assert(!"Unknown type");

    if(leftNode != nullptr)
        tree_link_node(node, leftNode);

    if(rightNode != nullptr)
        tree_link_node(node, rightNode);

    return node;
}
