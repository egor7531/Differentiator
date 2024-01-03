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
    else if(((NodeData*)elem)->type == IDENTIFIER)
    {
        data->type = IDENTIFIER;
        data->elem.id = strdup(((NodeData*)elem)->elem.id);
    }
    else
        assert("Unknown type");

    return data;
}

void elem_dtor(void* elem)
{
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == IDENTIFIER)
    {
        free(((NodeData*)elem)->elem.id);
        free(elem);
    }
}

void write_elem(FILE* fp, void* elem)
{
    assert(fp != nullptr);
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == NUM)
        fprintf(fp, "%.2lf", ((NodeData*)elem)->elem.value);
    else if(((NodeData*)elem)->type == IDENTIFIER)
        fprintf(fp, "%s", ((NodeData*)elem)->elem.id);
    else if(((NodeData*)elem)->type == OPERATOR)
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
            case OP_LN:
                fprintf(fp, "%s", "ln");
                break;
            case OP_SIN:
                fprintf(fp, "%s", "sin");
                break;
            case OP_COS:
                fprintf(fp, "%s", "cos");
                break;
            case OP_SQRT:
                fprintf(fp, "%s", "sqrt");
                break;
            case OP_EXP:
                fprintf(fp, "%s", "exp");
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
    else
        assert("Unknown type");
}

int compare_numbers(double x1, double x2)
{
    const double DELTA = 0.00001;

    if(x1 - x2 > DELTA)
        return 1;
    else if(x2 - x1 > DELTA)
        return -1;
    return 0;
}

void optimize_expression(Tree* expression, TreeNode* node)
{
    assert(expression != nullptr);

    if(node->leftNode == nullptr || node->rightNode == nullptr)
        return;

    optimize_expression(expression, node->leftNode);
    optimize_expression(expression, node->rightNode);

    NodeData* elem = (NodeData*)(node->elem);
    NodeData* elemLeft = (NodeData*)(node->leftNode->elem);
    NodeData* elemRight = (NodeData*)(node->rightNode->elem);

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

        tree_node_delete(expression, node->leftNode);
        tree_node_delete(expression, node->rightNode);
    }
    else if(elemRight->type == NUM && compare_numbers(elemRight->elem.value, 0) == 0)
    {
        switch(elem->elem.op)
        {
            case OP_ADD:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->leftNode;
                else
                    tree_link_node(node->parentNode, node->leftNode);
                break;
            case OP_SUB:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->leftNode;
                else
                    tree_link_node(node->parentNode, node->leftNode);
                break;
            case OP_MUL:
                tree_nodes_delete(expression, node->leftNode);
                tree_nodes_delete(expression, node->rightNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_POW:
                tree_nodes_delete(expression, node->leftNode);
                tree_nodes_delete(expression, node->rightNode);
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
            case OP_ADD:
                tree_node_delete(expression, node->leftNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->rightNode;
                else
                    tree_link_node(node->parentNode, node->rightNode);
                break;
            case OP_MUL:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_DIV:
                tree_nodes_delete(expression, node->rightNode);
                tree_nodes_delete(expression, node->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            case OP_POW:
                tree_nodes_delete(expression, node->rightNode);
                tree_nodes_delete(expression, node->leftNode);
                elem->type = NUM;
                elem->elem.value = 0;
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
    else if(elemRight->type == NUM && compare_numbers(elemRight->elem.value, 1) == 0)
    {
        switch(elem->elem.op)
        {
            case OP_MUL:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->leftNode;
                else
                    tree_link_node(node->parentNode, node->leftNode);
                break;
            case OP_DIV:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->leftNode;
                else
                    tree_link_node(node->parentNode, node->leftNode);
                break;
            case OP_POW:
                tree_node_delete(expression, node->rightNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->leftNode;
                else
                    tree_link_node(node->parentNode, node->leftNode);
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
    else if(elemLeft->type == NUM && compare_numbers(elemLeft->elem.value, 1) == 0)
    {
        switch(elem->elem.op)
        {
            case OP_MUL:
                tree_node_delete(expression, node->leftNode);
                tree_node_delete(expression, node);
                if(node == expression->root)
                    expression->root = node->rightNode;
                else
                    tree_link_node(node->parentNode, node->rightNode);
                break;
            case OP_POW:
                tree_nodes_delete(expression, node->rightNode);
                tree_nodes_delete(expression, node->leftNode);
                elem->type = NUM;
                elem->elem.value = 1;
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

TreeNode* derivative_const()
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
    TreeNode* node1 = create_node(OPERATOR, elem, nullptr, nullptr);
    tree_link_node(node1, node11);
    tree_link_node(node1, calculate_derivative(expressionNode->rightNode));

    elem.op = OP_MUL;
    TreeNode* node2 = create_node(OPERATOR, elem, nullptr, nullptr);
    tree_link_node(node2, node21);
    tree_link_node(node2, calculate_derivative(expressionNode->leftNode));

    elem.op = OP_ADD;
    TreeNode* node = create_node(OPERATOR, elem, nullptr, nullptr);
    tree_link_node(node, node1);
    tree_link_node(node, node2);
    return node;
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

    TreeNode* node1121 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    elem.value = 1;
    TreeNode* node1122 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_SUB;
    TreeNode* node112 = create_node(OPERATOR, elem, node1121, node1122);

    TreeNode* node111 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                  ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                  expressionNode->leftNode->leftNode,
                                  expressionNode->leftNode->rightNode);
    elem.op = OP_POW;
    TreeNode* node11 = create_node(OPERATOR, elem, node111, node112);

    TreeNode* node121 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                  expressionNode->rightNode->leftNode,
                                  expressionNode->rightNode->rightNode);

    elem.op = OP_MUL;
    TreeNode* node12 = create_node(OPERATOR, elem, node121,
                                    calculate_derivative(expressionNode->leftNode));

    elem.op = OP_MUL;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    TreeNode* node2212 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);

    elem.op = OP_LN;
    TreeNode* node221 = create_node(OPERATOR, elem, node2212, nullptr);

    elem.op = OP_MUL;
    TreeNode* node22 = create_node(OPERATOR, elem, node221,
                                    calculate_derivative(expressionNode->rightNode));

    TreeNode* node211 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);

    TreeNode* node212 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                    ((NodeData*)(expressionNode->rightNode->elem))->elem,
                                    expressionNode->rightNode->leftNode,
                                    expressionNode->rightNode->rightNode);
    elem.op = OP_POW;
    TreeNode* node21 = create_node(OPERATOR, elem, node211, node212);

    elem.op = OP_MUL;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, node22);

    elem.op = OP_ADD;
    return create_node(OPERATOR, elem, node1, node2);
}

TreeNode* derivative_ln(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);
    elem.op = OP_DIV;
    return create_node(OPERATOR, elem, calculate_derivative(expressionNode->leftNode), node);
}

TreeNode* derivative_sqrt(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    ((NodeData*)(expressionNode->elem))->elem.op = OP_POW;

    elem.value = 0.5;
    tree_link_node(expressionNode, create_node(NUM, elem, nullptr, nullptr));

    return derivative_pow(expressionNode);
}

TreeNode* derivative_exp(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node = create_node(((NodeData*)(expressionNode->elem))->type,
                                    ((NodeData*)(expressionNode->elem))->elem,
                                    expressionNode->leftNode,
                                    expressionNode->rightNode);
    elem.op = OP_MUL;
    return create_node(OPERATOR, elem, node, calculate_derivative(expressionNode->leftNode));
}

TreeNode* derivative_sin(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node1 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);
    elem.op = OP_COS;
    TreeNode* node = create_node(OPERATOR, elem, node1, nullptr);

    elem.op = OP_MUL;
    return create_node(OPERATOR, elem, node, calculate_derivative(expressionNode->leftNode));
}

TreeNode* derivative_cos(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node21 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem,
                                    expressionNode->leftNode->leftNode,
                                    expressionNode->leftNode->rightNode);
    elem.op = OP_SIN;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, nullptr);

    elem.value = -1;
    TreeNode* node1 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_MUL;
    TreeNode* node = create_node(OPERATOR, elem, node1, node2);

    elem.op = OP_MUL;
    return create_node(OPERATOR, elem, node, calculate_derivative(expressionNode->leftNode));
}

TreeNode* calculate_derivative(TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    if(((NodeData*)(expressionNode->elem))->type == OPERATOR)
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
            case OP_LN:
                return derivative_ln(expressionNode);
            case OP_SQRT:
                return derivative_sqrt(expressionNode);
            case OP_EXP:
                return derivative_exp(expressionNode);
            case OP_SIN:
                return derivative_sin(expressionNode);
            case OP_COS:
                return derivative_cos(expressionNode);
            default:
                assert("Unknown operator");
        }
    }
    else if(((NodeData*)(expressionNode->elem))->type == IDENTIFIER &&
                !strcmp(((NodeData*)(expressionNode->elem))->elem.id, "x"))
        return derivative_variable();
    else if(((NodeData*)(expressionNode->elem))->type == NUM ||
                ((NodeData*)(expressionNode->elem))->type == IDENTIFIER)
        return derivative_const();
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

void get_derivative(const char* nameFile)
{
    assert(nameFile != nullptr);

    const char* nameFileDotEx  = "Expression.dot";
    const char* nameFilePngEx  = "Expression.png";
    const char* nameFileTxtDer = "Derivative.txt";
    const char* nameFileDotDer = "Derivative.dot";
    const char* nameFilePngDer = "Derivative.png";

    char* buf = get_file_content(nameFile);
    Tree* expression = tree_ctor(elem_ctor, elem_dtor, write_elem);
    get_G(expression, &buf);
    optimize_expression(expression, expression->root);
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);

    Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
    derivative->root = calculate_derivative(expression->root);
    optimize_expression(derivative, derivative->root);
    tree_graphic_dump(derivative,  nameFileDotDer,  nameFilePngDer);
    print_derivative(nameFileTxtDer, derivative);

    tree_dtor(expression);
    tree_dtor(derivative);
}

double derivative_at_point(TreeNode* node, const double point)
{
    if(node == nullptr)
        return NAN;

    NodeData* elem = (NodeData*)(node->elem);
    if(elem->type == NUM)
        return elem->elem.value;
    else if(elem->type == IDENTIFIER)
        return point;

    double valueLeft  = derivative_at_point(node->leftNode, point);
    double valueRight = derivative_at_point(node->rightNode, point);

    switch(elem->elem.op)
    {
        case OP_ADD:
            return valueLeft + valueRight;
        case OP_SUB:
            return valueLeft - valueRight;
        case OP_DIV:
            return valueLeft / valueRight;
        case OP_MUL:
            return valueLeft * valueRight;
        case OP_POW:
            return pow(valueLeft, valueRight);
        case OP_SQRT:
            return sqrt(valueLeft);
        case OP_SIN:
            return sin(valueLeft);
        case OP_COS:
            return cos(valueLeft);
        case OP_EXP:
            return exp(valueLeft);
        case OP_LN:
            return log(valueLeft);
        default:
            assert("Unknown operator");
    }

    return NAN;
}

void get_taylor_series(const char* nameFile, const int order, const double point)
{
    assert(nameFile != nullptr);
    assert(order >= 0);

    const char* nameFileDotEx  = "Expression.dot";
    const char* nameFilePngEx  = "Expression.png";
    const char* nameFileTxtTS  = "TaylorSeries.txt";
    const char* nameFileDotTS  = "TaylorSeries.dot";
    const char* nameFilePngTS  = "TaylorSeries.png";
    const char* nameFileTxtDer = "Derivative.txt";

    int factorial = 1;

    char* buf = get_file_content(nameFile);
    Tree* expression = tree_ctor(elem_ctor, elem_dtor, write_elem);
    get_G(expression, &buf);
    optimize_expression(expression, expression->root);
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);

    Tree* taylorSeries = tree_ctor(elem_ctor, elem_dtor, write_elem);
    Data elem;
    if(order == 0)
    {
        elem.value = derivative_at_point(expression->root, point);
        taylorSeries->root = create_node(NUM, elem, nullptr, nullptr);
    }
    else
    {
        elem.value = derivative_at_point(expression->root, point);
        TreeNode* node1 = create_node(NUM, elem, nullptr, nullptr);
        elem.value = 0;
        TreeNode* node = create_node(NUM, elem, nullptr, nullptr);
        elem.op = OP_ADD;
        taylorSeries->root = create_node(OPERATOR, elem, nullptr, nullptr);
        tree_link_node(taylorSeries->root, node1);
        tree_link_node(taylorSeries->root, node);
        tree_graphic_dump(taylorSeries,  nameFileDotTS,  nameFilePngTS);

        for(int i = 1; i <= order; i++)
        {
            Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
            derivative->root = calculate_derivative(expression->root);
            optimize_expression(derivative, derivative->root);
            elem.value = derivative_at_point(derivative->root, point);

            tree_dtor(expression);
            expression = derivative;

            TreeNode* node111 = create_node(NUM, elem, nullptr, nullptr);
            factorial *= i;
            elem.value = factorial;
            TreeNode* node112 = create_node(NUM, elem, nullptr, nullptr);
            elem.op = OP_DIV;
            TreeNode* node11 = create_node(OPERATOR, elem, node111, node112);

            elem.id = strdup("x");
            TreeNode* node1211 = create_node(IDENTIFIER, elem, nullptr, nullptr);
            elem.value = point;
            TreeNode* node1212 = create_node(NUM, elem, nullptr, nullptr);
            elem.op = OP_SUB;
            TreeNode* node121 = create_node(OPERATOR, elem, node1211, node1212);

            elem.value = i;
            TreeNode* node122 = create_node(NUM, elem, nullptr, nullptr);
            elem.op = OP_POW;
            TreeNode* node12 = create_node(OPERATOR, elem, node121, node122);

            elem.op = OP_MUL;
            TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

            tree_link_node(node, node1);
            elem.value = 0;
            tree_link_node(node, create_node(NUM, elem, nullptr, nullptr));
            ((NodeData*)(node->elem))->type = OPERATOR;
            ((NodeData*)(node->elem))->elem.op = OP_ADD;
            node = node->rightNode;
        }
    }

    optimize_expression(taylorSeries, taylorSeries->root);
    print_derivative(nameFileTxtTS, taylorSeries);
    tree_graphic_dump(taylorSeries,  nameFileDotTS,  nameFilePngTS);
    tree_dtor(taylorSeries);
}
