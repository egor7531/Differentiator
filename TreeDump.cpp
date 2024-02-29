#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TreeDump.h"

void tree_status_errors(int err)
{
    assert(err > 0);

    const char* nameFile = "Errors.txt";
    FILE* fp = fopen(nameFile, "rb");
    if(fp == nullptr)
        return;

    if(err & TREE_IS_NULL)
        fprintf(fp, "Pointer on tree is nullptr\n");
    if(err & NODE_IS_NULL)
        fprintf(fp, "Pointer on node of tree is nullptr\n");
    if(err & ELEM_IS_NULL)
        fprintf(fp, "Pointer on elem of node is nullptr\n");
    if(err & FP_IS_NULL)
        fprintf(fp, "Pointer on file is nullptr\n");
    if(err & ERROR_WORK_SYSTEM)
        fprintf(fp, "Error work function \"system\"\n");
}

void get_connection_nodes(const Tree* tree, const TreeNode* node, FILE* fp)
{
    assert(fp != nullptr);
    assert(tree != nullptr);
    assert(tree->root != nullptr);

    fprintf(fp, "node%p [label = \"", node);
    tree->write_elem(fp, node->elem);
    fprintf(fp, "\"];\n");

    if(!node->leftNode)
        return;
    fprintf(fp, "node%p -> node%p;\n", node, node->leftNode);
    get_connection_nodes(tree, node->leftNode, fp);

    if(!node->rightNode)
        return;
    fprintf(fp, "node%p -> node%p;\n", node, node->rightNode);
    get_connection_nodes(tree, node->rightNode, fp);
}

void tree_graphic_dump(const Tree* tree, const char* nameFileDot, const char* nameFilePng)
{
    assert(tree != nullptr);

    FILE* fp = fopen(nameFileDot, "wb");
    if(fp == nullptr)
    {
        tree_status_errors(FP_IS_NULL);
        return;
    }

    fprintf(fp, "digraph Tree\n"
                "{\n"
                "rankdir = TB;\n");
    get_connection_nodes(tree, tree->root, fp);

    fprintf(fp, "}");
    fclose(fp);

    const int MAX_SIZE_COMMAND = 100;
    char command[MAX_SIZE_COMMAND] = "dot ";

    strcat(command, nameFileDot);
    strcat(command, " -T png -o ");
    strcat(command, nameFilePng);

    if(system(command) > 0)
    {
        tree_status_errors(ERROR_WORK_SYSTEM);
        return;
    }
}
