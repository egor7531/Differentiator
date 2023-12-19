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

Operators define_operator(char* command);
NodeData* get_object(char** buf);
double calculate_expression(TreeNode* node);
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
    else
    {
        data->type = VARIABLE;
        data->elem.variable = strdup(((NodeData*)elem)->elem.variable);
    }

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

Operators define_operator(char* command)
{
    assert(command != nullptr);

    if(!strcmp(command, "add"))
        return OP_ADD;
    if(!strcmp(command, "sub"))
        return OP_SUB;
    if(!strcmp(command, "div"))
        return OP_DIV;
    if(!strcmp(command, "mul"))
        return OP_MUL;
    if(!strcmp(command, "pow"))
        return OP_POW;

    return NO_OP;
}

NodeData* get_object(char** buf)
{
    assert(buf != nullptr);

    NodeData* data = (NodeData*)calloc(1, sizeof(NodeData));
    if(data == nullptr)
        return nullptr;

    char* object = (char*)calloc(MAX_SIZE_OBJECT, sizeof(char));
    if(object == nullptr)
        return nullptr;

    sscanf(*buf, "%s", object);

    if(isdigit(object[0]))
    {
        data->elem.value = atof(object);
        data->type = NUM;
    }
    else if(!strcmp(object, "x"))
    {
        data->elem.variable = strdup(object);
        data->type = VARIABLE;
    }
    else
    {
        data->elem.op = define_operator(object);
        data->type = OPERATOR;
    }

    *buf += strlen(object) + 1;
    free(object);

    return data;
}

double calculate_expression(TreeNode* node)
{
    if(node == nullptr)
        return NAN;

    if(((NodeData*)(node->elem))->type == NUM)
        return ((NodeData*)(node->elem))->elem.value;

    double valueLeft  = calculate_expression(node->leftNode);
    double valueRight = calculate_expression(node->rightNode);

    switch(((NodeData*)(node->elem))->elem.op)
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
        default:
            assert("Unknown operator");
    }

    return NAN;
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

void derivative_variable(TreeNode** derivative)
{
    *derivative = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(*derivative == nullptr)
        return;

    (*derivative)->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)((*derivative)->elem)) == nullptr)
        return;

    ((NodeData*)((*derivative)->elem))->type = NUM ;
    ((NodeData*)((*derivative)->elem))->elem.value = 1;
}

void derivative_num(TreeNode** derivative)
{
    *derivative = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(*derivative == nullptr)
        return;

    (*derivative)->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)((*derivative)->elem)) == nullptr)
        return;

    ((NodeData*)((*derivative)->elem))->type = NUM;
    ((NodeData*)((*derivative)->elem))->elem.value = 0;
}

void derivative_sum(TreeNode** derivative, TreeNode* expression)
{
    assert(expression != nullptr);

    *derivative = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(*derivative == nullptr)
        return;

    (*derivative)->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)((*derivative)->elem)) == nullptr)
        return;

    ((NodeData*)((*derivative)->elem))->type = OPERATOR;
    ((NodeData*)((*derivative)->elem))->elem.op = ((NodeData*)(expression->elem))->elem.op;

    calculate_derivative(&(*derivative)->leftNode, expression->leftNode);
    calculate_derivative(&(*derivative)->rightNode, expression->rightNode);
}

void derivative_pow(TreeNode** derivative, TreeNode* expression)
{
    assert(expression != nullptr);

    *derivative = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(*derivative == nullptr)
        return;

    (*derivative)->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)((*derivative)->elem)) == nullptr)
        return;

    ((NodeData*)((*derivative)->elem))->type = OPERATOR;
    ((NodeData*)((*derivative)->elem))->elem.op = OP_MUL;

    TreeNode *derivative2 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative2 == nullptr)
        return;

    derivative2->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(derivative2->elem)) == nullptr)
        return;

    ((NodeData*)(derivative2->elem))->type = OPERATOR;
    ((NodeData*)(derivative2->elem))->elem.op = OP_ADD;

    TreeNode *derivative21 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative21 == nullptr)
        return;

    derivative21->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(derivative21->elem)) == nullptr)
        return;

    ((NodeData*)(derivative21->elem))->type = OPERATOR;
    ((NodeData*)(derivative21->elem))->elem.op = OP_MUL;

    TreeNode *derivative22 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative22 == nullptr)
        return;

    derivative22->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(derivative22->elem)) == nullptr)
        return;

    ((NodeData*)(derivative22->elem))->type = OPERATOR;
    ((NodeData*)(derivative22->elem))->elem.op = OP_MUL;

    tree_link_node(*derivative, expression);
    tree_link_node(*derivative, derivative2);
    tree_link_node(derivative2, derivative21);
    tree_link_node(derivative2, derivative22);

    TreeNode *derivative211 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative211 == nullptr)
        return;

    derivative211->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(derivative211->elem)) == nullptr)
        return;

    ((NodeData*)(derivative211->elem))->type = OPERATOR;
    ((NodeData*)(derivative211->elem))->elem.op = OP_DIV;

    TreeNode *derivative222 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative222 == nullptr)
        return;

    derivative222->elem = (NodeData*)calloc(1, sizeof(NodeData));

    if(((NodeData*)(derivative222->elem)) == nullptr)
        return;

    ((NodeData*)(derivative222->elem))->type = OPERATOR;
    ((NodeData*)(derivative222->elem))->elem.op = OP_ln;

    TreeNode* derivative2111 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative2111 == nullptr)
        return;

    derivative2111->elem = expression->rightNode->elem;

    TreeNode* derivative2112 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative2112 == nullptr)
        return;

    derivative2112->elem = expression->leftNode->elem;

    TreeNode* derivative2221 = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(derivative2221 == nullptr)
        return;

    derivative2221->elem = expression->leftNode->elem;

    tree_link_node(derivative21, derivative211);
    tree_link_node(derivative211, derivative2111);
    tree_link_node(derivative211, derivative2112);
    tree_link_node(derivative22, derivative222);
    tree_link_node(derivative222, derivative2221);

    calculate_derivative(&derivative21->rightNode, expression->leftNode);
    calculate_derivative(&derivative22->rightNode, expression->rightNode);
}

void calculate_derivative(TreeNode** derivative, TreeNode* expression)
{
    if(((NodeData*)(expression->elem))->type == NUM)
        derivative_num(derivative);
    else if(((NodeData*)(expression->elem))->type == VARIABLE)
        derivative_variable(derivative);
     else
    {
        switch(((NodeData*)(expression->elem))->elem.op)
        {
            case OP_ADD:
                derivative_sum(derivative, expression);
                break;
            case OP_SUB:
                derivative_sum(derivative, expression);
                break;
            case OP_POW:
                derivative_pow(derivative, expression);
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
