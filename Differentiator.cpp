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

//void optimize_derivative(TreeNode** node);
void calculate_derivative(TreeNode** derivative, TreeNode* expression);
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

    free(((NodeData*)elem)->elem.variable);
    free(elem);
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

/*void optimize_derivative(TreeNode** node)
{
    if(*node == nullptr)
        return;

    if( ((NodeData*)((*node)->elem))->type == OPERATOR)
    {
        if( ((NodeData*)((*node)->leftNode->elem))->type == NUM &&
            ((NodeData*)((*node)->rightNode->elem))->type == NUM)
        {
            ((NodeData*)((*node)->elem))->type = NUM;

            double valueLeft = ((NodeData*)((*node)->leftNode->elem))->elem.value;
            double valueRight = ((NodeData*)((*node)->rightNode->elem))->elem.value;

            switch(((NodeData*)((*node)->elem))->elem.op)
            {
                case OP_ADD:
                    ((NodeData*)((*node)->elem))->elem.value = valueLeft + valueRight;
                    break;
                case OP_SUB:
                    ((NodeData*)((*node)->elem))->elem.value = valueLeft - valueRight;
                    break;
                case OP_DIV:
                    ((NodeData*)((*node)->elem))->elem.value = valueLeft / valueRight;
                    break;
                case OP_MUL:
                    ((NodeData*)((*node)->elem))->elem.value = valueLeft * valueRight;
                    break;
                case OP_POW:
                    ((NodeData*)((*node)->elem))->elem.value = pow(valueLeft, valueRight);
                    break;
                default:
                    assert("Unknown operator");
            }

            elem_dtor(((*node)->leftNode->elem));
            elem_dtor(((*node)->rightNode->elem));
            free(((*node)->leftNode));
            free(((*node)->rightNode));
            (*node)->leftNode = nullptr;
            (*node)->rightNode = nullptr;
        }
}*/

void derivative_variable(TreeNode** derivativeNode)
{
    Data elem;
    elem.value = 1;
    *derivativeNode = create_node(NUM, elem);
}

void derivative_num(TreeNode** derivativeNode)
{
    Data elem;
    elem.value = 0;
    *derivativeNode = create_node(NUM, elem);
}

void derivative_sum(TreeNode** derivativeNode, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    *derivativeNode = create_node(OPERATOR, ((NodeData*)(expressionNode->elem))->elem);

    calculate_derivative(&(*derivativeNode)->leftNode, expressionNode->leftNode);
    calculate_derivative(&(*derivativeNode)->rightNode, expressionNode->rightNode);
}

void derivative_pow(TreeNode** derivativeNode, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    elem.op = OP_MUL;
    *derivativeNode = create_node(OPERATOR, elem);

    elem.op = OP_POW;
    TreeNode* node1 = create_node(OPERATOR, elem);

    elem.op = OP_ADD;
    TreeNode* node2= create_node(OPERATOR, elem);

    tree_link_node(*derivativeNode, node1);
    tree_link_node(*derivativeNode, node2);

    TreeNode* node11 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                  ((NodeData*)(expressionNode->leftNode->elem))->elem);

    TreeNode* node12 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                  ((NodeData*)(expressionNode->rightNode->elem))->elem);

    tree_link_node(node1, node11);
    tree_link_node(node1, node12);

    elem.op = OP_MUL;
    TreeNode* node21 = create_node(OPERATOR, elem);

    elem.op = OP_MUL;
    TreeNode* node22 = create_node(OPERATOR, elem);

    tree_link_node(node2, node21);
    tree_link_node(node2, node22);

    elem.op = OP_DIV;
    TreeNode* node211 = create_node(OPERATOR, elem);

    elem.op = OP_ln;
    TreeNode* node221 = create_node(OPERATOR, elem);

    tree_link_node(node21, node211);
    tree_link_node(node22, node221);

    TreeNode* node2111 = create_node(((NodeData*)(expressionNode->rightNode->elem))->type,
                                    ((NodeData*)(expressionNode->rightNode->elem))->elem);

    TreeNode* node2112 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem);

    TreeNode* node2211 = create_node(((NodeData*)(expressionNode->leftNode->elem))->type,
                                    ((NodeData*)(expressionNode->leftNode->elem))->elem);

    tree_link_node(node211, node2111);
    tree_link_node(node211, node2112);
    tree_link_node(node221, node2211);

    calculate_derivative(&node21->rightNode, expressionNode->leftNode);
    calculate_derivative(&node22->rightNode, expressionNode->rightNode);
}

void calculate_derivative(TreeNode** derivativeNode, TreeNode* expressionNode)
{
    if(((NodeData*)(expressionNode->elem))->type == NUM)
        derivative_num(derivativeNode);
    else if(((NodeData*)(expressionNode->elem))->type == VARIABLE)
        derivative_variable(derivativeNode);
     else
    {
        switch(((NodeData*)(expressionNode->elem))->elem.op)
        {
            case OP_ADD:
                derivative_sum(derivativeNode, expressionNode);
                break;
            case OP_SUB:
                derivative_sum(derivativeNode, expressionNode);
                break;
            case OP_POW:
                derivative_pow(derivativeNode, expressionNode);
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
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
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);

    Tree* derivative = tree_ctor(elem_ctor, elem_dtor, write_elem);
    calculate_derivative(&derivative->root, expression->root);
    tree_graphic_dump(derivative,  nameFileDotDer,  nameFilePngDer);
    print_derivative(nameFileTxtDer, derivative);

    tree_dtor(expression);
    tree_dtor(derivative);
}
