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
    OP_SQRT,
    OP_SIN,
    OP_COS
};

enum TypeElem
{
    OPERATOR,
    NUM
};

union Data
{
    double  value;
    char*   op;
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
NodeData* get_object(char** buf);

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
        data->elem.op = strdup(((NodeData*)elem)->elem.op);
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
    assert(elem != nullptr);

    free(((NodeData*)elem)->elem.op);
    free(elem);
}

void  write_elem(FILE* fp, void* elem)
{
    assert(fp != nullptr);
    assert(elem != nullptr);

    if(((NodeData*)elem)->type == NUM)
        fprintf(fp, "%.2lf", ((NodeData*)elem)->elem.value);
    else
        fprintf(fp, "%s", ((NodeData*)elem)->elem.op);
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
        data->elem.op = strdup(object);
        data->type = OPERATOR;
    }

    *buf += strlen(object) + 1;
    free(object);

    return data;
}
