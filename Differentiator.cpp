#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Tree.h"
#include "TreeDump.h"
#include "File.h"

enum Operators
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW
};

enum TypeElem
{
    OPERATOR,
    NUM
};

union Data
{
    double      value;
    Operators   op;
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

double pow(double x, int n);

const char* nameFile = "Expression.txt";
const char* nameFileDot = "Dump.dot";
const char* nameFilePng = "Dump.png";

const int MAX_SIZE_OBJECT = 50;

int main()
{
    char* buf = get_file_content(nameFile);
    Tree* tree = tree_ctor(elem_ctor, elem_dtor, write_elem);

    write_expression_in_tree(tree, &tree->root, &buf);
    tree_graphic_dump(tree, nameFileDot, nameFilePng);

    printf("%lf", calculate_expression(tree->root));

    tree_dtor(tree);
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
    else
    {
        data->type = NUM;
        data->elem.value = ((NodeData*)elem)->elem.value;
    }

    return data;
}

void elem_dtor(void* elem)
{
    free(elem);
}

void  write_elem(FILE* fp, void* elem)
{
    assert(fp != nullptr);
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == NUM)
        fprintf(fp, "%.2lf", ((NodeData*)elem)->elem.value);
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
        return 0;

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

    return -1;
}

double pow(double x, int n)
{
    assert(n < 0);

    if(n == 1)
        return 1;

    return x * pow(x, n - 1);
}
