#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Tree.h"
#include "TreeDump.h"
#include "File.h"

enum Operators
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_SQRT,
    OP_SIN,
    OP_COS,
    NO_OP
};

enum TypeElem
{
    OPERATOR,
    NUM,
    VARIABLE
};

union Data
{
    double      value;
    Operators   op;
    char*       variable;
};

struct NodeData
{
    TypeElem    type;
    Data        elem;
};

void* elem_ctor(void* elem);
void  elem_dtor(void* elem);
void  write_elem(FILE* fp, void* elem);

void write_expression_in_tree(Tree* tree, TreeNode **node, char **buf);
Operators define_operator(char* command);
NodeData* get_object(char** buf);
double calculate_expression(TreeNode* node);
void optimize_derivative(TreeNode** node);
void get_derivative(Tree* tree, TreeNode* node);
void print_derivative(const char* nameFile, const Tree* tree);
void print_nodes(FILE* fp, const TreeNode* node);

const char* nameFileTxtEx  = "Expression.txt";
const char* nameFileDotEx  = "Expression.dot";
const char* nameFilePngEx  = "Expression.png";

const char* nameFileTxtDer = "Derivative.txt";
const char* nameFileDotDer = "Derivative.dot";
const char* nameFilePngDer = "Derivative.png";

const int MAX_SIZE_OBJECT = 50;

int main()
{
    char* buf = get_file_content(nameFileTxtEx);
    Tree* expression = tree_ctor(elem_ctor, elem_dtor, write_elem);

    write_expression_in_tree(expression, &expression->root, &buf);
    tree_graphic_dump(expression,  nameFileDotEx,  nameFilePngEx);

    get_derivative(expression, expression->root);
    tree_graphic_dump(expression,  nameFileDotDer,  nameFilePngDer);

    print_derivative(nameFileTxtDer, expression);

    tree_dtor(expression);
    return 0;
}

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
            case OP_SQRT:
                fprintf(fp, "%s", "sqrt");
                break;
            case OP_SIN:
                fprintf(fp, "%s", "sin");
                break;
            case OP_COS:
                fprintf(fp, "%s", "cos");
                break;
            default:
                assert("Unknown operator");
                break;
        }
    }
}

void write_expression_in_tree(Tree* tree, TreeNode **node, char **buf)
{
    assert(tree != nullptr);
    assert(buf != nullptr);

    char object[MAX_SIZE_OBJECT] = {};
    sscanf(*buf, "%s", object);
    *buf += strlen(object) + 1;

    if(!strcmp(object, "("))
    {
        NodeData* data = get_object(buf);
        *node = tree_node_new(tree, data);
        if(*node == nullptr)
        {
            printf("Node is null\n");
            return;
        }
        free(data);
    }
    else
    {
        sscanf(*buf, "%s", object);

        while(!strcmp(object, ")"))
        {
            *buf += strlen(object) + 1;
            sscanf(*buf, "%s", object);
        }
        return;
    }

    write_expression_in_tree(tree, &((*node)->leftNode), buf);
    write_expression_in_tree(tree, &((*node)->rightNode), buf);
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
    if(!strcmp(command, "sqrt"))
        return OP_SQRT;
    if(!strcmp(command, "sin"))
        return OP_SIN;
    if(!strcmp(command, "cos"))
        return OP_COS;

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
        case OP_SQRT:
            return sqrt(valueLeft);
        case OP_SIN:
            return sin(valueLeft);
        case OP_COS:
            return cos(valueLeft);
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

void get_derivative(Tree* tree, TreeNode* node)
{
    if(((NodeData*)(node->elem))->type == NUM)
        ((NodeData*)(node->elem))->elem.value = 0;
    else
    {
        switch(((NodeData*)(node->elem))->elem.op)
        {
            case OP_ADD:
                get_derivative(tree, node->leftNode);
                get_derivative(tree, node->rightNode);
                break;
            case OP_SUB:
                get_derivative(tree, node->leftNode);
                get_derivative(tree, node->rightNode);
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
    fprintf(fp, "( ");
    print_nodes(fp, node->leftNode);
    write_elem(fp, node->elem);
    fprintf(fp, " ");
    print_nodes(fp, node->rightNode);
    fprintf(fp, ") ");
}
