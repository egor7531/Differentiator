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
#include "Calculate_derivative.h"
#include "Node.h"

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

    NodeData* elem      = (NodeData*)(node->elem);
    NodeData* elemLeft  = (NodeData*)(node->leftNode->elem);
    NodeData* elemRight = (NodeData*)(node->rightNode->elem);

    if(elemLeft->type == NUM && elemRight->type == NUM)
    {
        elem->type = NUM;
        switch(elem->elem.op)
        {
            #define ARG1 elemLeft->elem.value
            #define ARG2 elemRight->elem.value

            #define DEF_OP(name, icon, binary_op, code, der)    \
                case name:                                      \
                    elem->elem.value = code;                    \
                    break;

            #include "Operators.h"

            #undef DEF_OP
            #undef ARG1
            #undef ARG2

            default:
                tree_dtor(expression);
                assert(!"Unknown operator");
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
                break;
        }
    }
}

double derivative_at_point(Tree* expression, TreeNode* node, const double point)
{
    if(node == nullptr)
        return NAN;

    NodeData* elem = (NodeData*)(node->elem);
    if(elem->type == NUM)
        return elem->elem.value;
    else if(elem->type == IDENTIFIER)
        return point;

    double valueLeft  = derivative_at_point(expression, node->leftNode, point);
    double valueRight = derivative_at_point(expression, node->rightNode, point);

    switch(elem->elem.op)
    {
        #define ARG1 valueLeft
        #define ARG2 valueRight

        #define DEF_OP(name, icon, binary_op, code, der)    \
            case name:                                      \
                return code;
            default:
            tree_dtor(expression);
            assert("Unknown operator");

        #include "Operators.h"

        #undef DEF_OP
        #undef ARG1
        #undef ARG2
    }

    return NAN;
}

Tree* get_expression(const char* nameFile)
{
    assert(nameFile != nullptr);

    const char* nameFileDotEx  = "Expression.dot";
    const char* nameFilePngEx  = "Expression.png";

    char* buf = get_file_content(nameFile);
    Tree* expression = tree_ctor(elem_ctor, elem_dtor, write_elem);
    get_G(expression, &buf);
    optimize_expression(expression, expression->root);
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);

    return expression;
}

void get_derivative(const char* nameFile)
{
    assert(nameFile != nullptr);

    const char* nameFileTxtDer = "Derivative.txt";
    const char* nameFileDotDer = "Derivative.dot";
    const char* nameFilePngDer = "Derivative.png";

    Tree* expression = get_expression(nameFile);
    Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
    derivative->root = calculate_derivative(expression, expression->root);
    optimize_expression(derivative, derivative->root);
    tree_graphic_dump(derivative,  nameFileDotDer,  nameFilePngDer);
    print_expression(nameFileTxtDer, derivative);

    tree_dtor(expression);
    tree_dtor(derivative);
}

void get_taylor_series(const char* nameFile, const int order, const double point)
{
    assert(nameFile != nullptr);
    assert(order >= 0);

    const char* nameFileTxtTS  = "TaylorSeries.txt";
    const char* nameFileDotTS  = "TaylorSeries.dot";
    const char* nameFilePngTS  = "TaylorSeries.png";

    const char* VARIABLE = "x";

    Tree* expression = get_expression(nameFile);
    Tree* taylorSeries = tree_ctor(elem_ctor, elem_dtor, write_elem);

    Data elem;
    int factorial = 1;

    elem.value = derivative_at_point(expression, expression->root, point);
    TreeNode* node1 = create_node(NUM, elem, nullptr, nullptr);

    elem.value = 0;
    TreeNode* node = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_ADD;
    taylorSeries->root = create_node(OPERATOR, elem, node1, node);

    for(int i = 1; i <= order; i++)
    {
        Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
        derivative->root = calculate_derivative(expression, expression->root);
        optimize_expression(derivative, derivative->root);
        elem.value = derivative_at_point(derivative, derivative->root, point);
        tree_dtor(expression);
        expression = derivative;

        TreeNode* node111 = create_node(NUM, elem, nullptr, nullptr);
        factorial *= i;
        elem.value = factorial;
        TreeNode* node112 = create_node(NUM, elem, nullptr, nullptr);
        elem.op = OP_DIV;
        TreeNode* node11 = create_node(OPERATOR, elem, node111, node112);

        elem.id = strdup(VARIABLE);
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

    optimize_expression(taylorSeries, taylorSeries->root);
    print_expression(nameFileTxtTS, taylorSeries);
    tree_graphic_dump(taylorSeries,  nameFileDotTS,  nameFilePngTS);
    tree_dtor(taylorSeries);
}
