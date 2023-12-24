#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Differentiator.h"
#include "Tree.h"
#include "TreeDump.h"
#include "File.h"
#include "Parsing.h"

void* elem_ctor(void* elem);
void  elem_dtor(void* elem);
void  write_elem(FILE* fp, void* elem);

void optimize_expression(Tree* expression, TreeNode** node);
TreeNode* calculate_derivative(TreeNode* expressionNode);
void print_derivative(const char* nameFile, const Tree* tree);
void print_nodes(FILE* fp, const TreeNode* node);

const int MAX_SIZE_OBJECT = 50;

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
    else if(((NodeData*)elem)->type == VARIABLE)
    {
        data->type = VARIABLE;
        data->elem.variable = strdup(((NodeData*)elem)->elem.variable);
    }
    else
        assert("Unknown type");

    return data;
}

void elem_dtor(void* elem)
{
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == VARIABLE)
    {
        free(((NodeData*)elem)->elem.variable);
        free(elem);
    }
}

void write_elem(FILE* fp, void* elem)
{
    assert(fp != nullptr);
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == NUM)
        fprintf(fp, "%.2lf", ((NodeData*)elem)->elem.value);
    else if(((NodeData*)elem)->type == VARIABLE)
        fprintf(fp, "%s", ((NodeData*)elem)->elem.variable);
    else
    {
        switch(((NodeData*)elem)->elem.op)
        {
            case OP_ADD:
                fprintf(fp, "%c", '+');
                break;
            case OP_SUB:
                fprintf(fp, "%c", '-');
                break;
            case OP_DIV:
                fprintf(fp, "%c", '/');
                break;
            case OP_MUL:
                fprintf(fp, "%c", '*');
                break;
            case OP_POW:
                fprintf(fp, "%c", '^');
                break;
            case OP_ln:
                fprintf(fp, "%s", "ln");
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
}

int compare_numbers(double x1, double x2)
{
    const double DELTA = 0.00001;

    if(abs(x1 - x2) < DELTA)
        return 0;
    return x1 - x2;
}
void optimize_expression(Tree* expression, TreeNode** node)
{
    assert(expression != nullptr);

    if((*node)->leftNode == nullptr || (*node)->rightNode == nullptr)
        return;

    optimize_expression(expression, &((*node)->leftNode));
    optimize_expression(expression, &((*node)->rightNode));

    NodeData* elem = (NodeData*)((*node)->elem);
    NodeData* elemLeft = (NodeData*)((*node)->leftNode->elem);
    NodeData* elemRight = (NodeData*)((*node)->rightNode->elem);

    if(elemLeft->type == NUM && elemRight->type == NUM)
    {
        elem->type = NUM;
        switch(elem->elem.op)
        {
            case OP_ADD:
                elem->elem.value = elemLeft->elem.value + elemRight->elem.value;
                break;
            case OP_SUB:
                elem->elem.value = elemLeft->elem.value - elemRight->elem.value;
                break;
            case OP_MUL:
                elem->elem.value = elemLeft->elem.value * elemRight->elem.value;
                break;
            case OP_DIV:
                elem->elem.value = elemLeft->elem.value / elemRight->elem.value;
                break;
            case OP_POW:
                elem->elem.value = pow(elemLeft->elem.value,elemRight->elem.value);
                break;
            default:
                assert("Unknown operator");
                break;
        }

        tree_nodes_delete(expression, &(*node)->leftNode);
        tree_nodes_delete(expression, &(*node)->rightNode);
    }
    else if(elemRight->type == NUM && compare_numbers(elemRight->elem.value, 0) == 0)
    {
        switch(elem->elem.op)
        {
            case OP_MUL:
                tree_nodes_delete(expression, &(*node)->leftNode);
                tree_nodes_delete(expression, &(*node)->rightNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_POW:
                tree_nodes_delete(expression, &(*node)->leftNode);
                tree_nodes_delete(expression, &(*node)->rightNode);
                elem->type = NUM;
                elem->elem.value = 1;
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
    else if(elemLeft->type == NUM && compare_numbers(elemLeft->elem.value, 0) == 0)
    {
        switch(elem->elem.op)
        {
            case OP_MUL:
                tree_nodes_delete(expression, &(*node)->rightNode);
                tree_nodes_delete(expression, &(*node)->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_DIV:
                tree_nodes_delete(expression, &(*node)->rightNode);
                tree_nodes_delete(expression, &(*node)->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_POW:
                tree_nodes_delete(expression, &(*node)->rightNode);
                tree_nodes_delete(expression, &(*node)->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
}

TreeNode* derivative_variable()
{
    Data elem;
    elem.value = 1;
    return create_node(NUM, elem, nullptr, nullptr);
}

TreeNode* derivative_num()
{
    Data elem;
    elem.value = 0;
    return create_node(NUM, elem, nullptr, nullptr);
}

TreeNode* derivative_sum(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    return create_node(OPERATOR, ((NodeData*)(expressionNode->elem))->elem,
                                calculate_derivative(expressionNode->leftNode),
                                calculate_derivative(expressionNode->rightNode));
}

TreeNode* derivative_mul(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node11 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                  ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                  expressionNode->leftNode->leftNode,
                                  expressionNode->leftNode->rightNode);

    TreeNode* node21 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    elem.op = OP_MUL;
    TreeNode* node1 = create_node(OPERATOR, elem, node11,
                                calculate_derivative(expressionNode->rightNode));

    elem.op = OP_MUL;
    TreeNode* node2 = create_node(OPERATOR, elem, node21,
                                calculate_derivative(expressionNode->leftNode));

    elem.op = OP_ADD;
    return create_node(OPERATOR, elem, node1, node2);
}

TreeNode* derivative_div(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node121 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                  ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                  expressionNode->leftNode->leftNode,
                                  expressionNode->leftNode->rightNode);

    TreeNode* node111 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    TreeNode* node21 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    elem.value = 2;
    TreeNode* node22 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_POW;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, node22);

    elem.op = OP_MUL;
    TreeNode* node11 = create_node(OPERATOR, elem, node111,
                                    calculate_derivative(expressionNode->leftNode));

    elem.op = OP_MUL;
    TreeNode* node12 = create_node(OPERATOR, elem, node121,
                                    calculate_derivative(expressionNode->rightNode));

    elem.op = OP_SUB;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    elem.op = OP_DIV;
    return create_node(OPERATOR, elem, node1, node2);
}

TreeNode* derivative_pow(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node11 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                  ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                  expressionNode->leftNode->leftNode,
                                  expressionNode->leftNode->rightNode);

    TreeNode* node12 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    TreeNode* node2111 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                    ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                    expressionNode->rightNode->leftNode,
                                    expressionNode->rightNode->rightNode);

    TreeNode* node2112 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);

    TreeNode* node2211 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);

    elem.op = OP_POW;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    elem.op = OP_DIV;
    TreeNode* node211 = create_node(OPERATOR, elem, node2111, node2112);

    elem.op = OP_ln;
    TreeNode* node221 = create_node(OPERATOR, elem, node2211, nullptr);

    elem.op = OP_MUL;
    TreeNode* node21 = create_node(OPERATOR, elem, node211,
                                    calculate_derivative(expressionNode->leftNode));

    elem.op = OP_MUL;
    TreeNode* node22 = create_node(OPERATOR, elem, node221,
                                    calculate_derivative(expressionNode->rightNode));

    elem.op = OP_ADD;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, node22);

    elem.op = OP_MUL;
    return create_node(OPERATOR, elem, node1, node2);
}

TreeNode* calculate_derivative(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    if(((NodeData*)(expressionNode->elem))->type == NUM)
        return derivative_num();
    else if(((NodeData*)(expressionNode->elem))->type == VARIABLE)
        return derivative_variable();
    else if(((NodeData*)(expressionNode->elem))->type == OPERATOR)
    {
        switch(((NodeData*)(expressionNode->elem))->elem.op)
        {
            case OP_ADD:
                return derivative_sum(expressionNode);
            case OP_SUB:
                return derivative_sum(expressionNode);
            case OP_MUL:
                return derivative_mul(expressionNode);
            case OP_DIV:
                return derivative_div(expressionNode);
            case OP_POW:
                return derivative_pow(expressionNode);
            default:
                assert("Unknown operator");
        }
    }
    else
        assert("Unknown type");

    return nullptr;
}

void print_derivative(const char* nameFile, const Tree* tree)
{
    assert(nameFile != nullptr);
    assert(tree != nullptr);

    FILE* fp = fopen(nameFile, "wb");
    if(fp == nullptr)
        return;

    print_nodes(fp, tree->root);
    fclose(fp);
}

void print_nodes(FILE* fp, const TreeNode* node)
{
    assert(fp != nullptr);

    if(node == nullptr)
        return;
    print_nodes(fp, node->leftNode);
    write_elem(fp, node->elem);
    print_nodes(fp, node->rightNode);
}

void get_derivative(const char* nameFile)
{
    const char* nameFileDotEx  = "Expression.dot";
    const char* nameFilePngEx  = "Expression.png";
    const char* nameFileTxtDer = "Derivative.txt";
    const char* nameFileDotDer = "Derivative.dot";
    const char* nameFilePngDer = "Derivative.png";

    char* buf = get_file_content(nameFile);
    Tree* expression = tree_ctor(elem_ctor, elem_dtor, write_elem);
    get_G(expression, &buf);
    optimize_expression(expression, &(expression->root));
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);
    Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
    derivative->root = calculate_derivative(expression->root);
    optimize_expression(derivative, &derivative->root);
    tree_graphic_dump(derivative,  nameFileDotDer,  nameFilePngDer);
    print_derivative(nameFileTxtDer, derivative);

    tree_dtor(expression);
    tree_dtor(derivative);
}
